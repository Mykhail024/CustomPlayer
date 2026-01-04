#include "FFmpegAudioDevice.h"

#include "AudioEffectManager.h"
#include "Log.h"
#include "libav.h"

char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};

constexpr static AVSampleFormat convertFormat(QAudioFormat::SampleFormat format)
{
    switch (format) {
    case (QAudioFormat::Int16):
        return AV_SAMPLE_FMT_S16;
    case (QAudioFormat::Int32):
        return AV_SAMPLE_FMT_S32;
    case (QAudioFormat::Float):
        return AV_SAMPLE_FMT_FLT;
    default:
        return AV_SAMPLE_FMT_FLT;
    }
}

FFmpegAudioDevice::FFmpegAudioDevice(const QString &file, QObject *parent) : QIODevice(parent), m_file(file)
{
    connect(this, &FFmpegAudioDevice::errorOccurred, this, &FFmpegAudioDevice::setErrorString);
    connect(this, &FFmpegAudioDevice::positionChange, this, [&](qreal pos) { m_positionMs = pos; });
    connect(this, &FFmpegAudioDevice::durationChange, this, [&](qreal dur) { m_durationMs = dur; });
}

bool FFmpegAudioDevice::open(QIODeviceBase::OpenMode mode)
{
    if (isOpen()) {
        Log_Warning("Device already opened");
        return true;
    }

    if (mode != QIODeviceBase::ReadOnly) {
        Log_Warning("Only ReadOnly mode is supported. Attempt to open file in a different mode was "
                    "ignored.");
    }

    int ret = avformat_open_input(&m_formatCtx, m_file.toStdString().c_str(), nullptr, nullptr);
    if (ret < 0) {
        emit errorOccurred(QString("Unable to open file %1 : %2").arg(m_file, av_make_error_string(errbuf, sizeof(errbuf), ret)));
        return false;
    }

    ret = avformat_find_stream_info(m_formatCtx, nullptr);
    if (ret < 0) {
        emit errorOccurred(QString("Unable to find stream info in file %1 : %2").arg(m_file, av_make_error_string(errbuf, sizeof(errbuf), ret)));
        return false;
    }

    int bstream_index = av_find_best_stream(m_formatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (bstream_index < 0) {
        emit errorOccurred(QString("Unable to find audio stream in %1").arg(m_file));
        return false;
    }

    m_stream = m_formatCtx->streams[bstream_index];

    const AVCodec *decoder = avcodec_find_decoder(m_stream->codecpar->codec_id);
    if (!decoder) {
        emit errorOccurred(QString("Failed to find decoder for %1").arg(m_file));
        return false;
    }

    m_codecCtx = avcodec_alloc_context3(decoder);
    if (!m_codecCtx) {
        emit errorOccurred(QString("Failed to allocate codec context"));
        return false;
    }

    avcodec_parameters_to_context(m_codecCtx, m_stream->codecpar);

    ret = avcodec_open2(m_codecCtx, decoder, nullptr);
    if (ret < 0) {
        emit errorOccurred(QString("Couldn't open decoder: %1").arg(av_make_error_string(errbuf, sizeof(errbuf), ret)));
        return false;
    }

    m_packet = av_packet_alloc();
    if (!m_packet) {
        emit errorOccurred("Failed to allocate packet");
        return false;
    }

    m_frame = av_frame_alloc();
    if (!m_frame) {
        emit errorOccurred("Failed to allocate frame");
        return false;
    }

    AVSampleFormat _format = convertFormat(format().sampleFormat());

    ret = swr_alloc_set_opts2(&m_resampler, &m_codecCtx->ch_layout, _format, m_codecCtx->sample_rate, &m_codecCtx->ch_layout,
                              m_codecCtx->sample_fmt, m_codecCtx->sample_rate, 0, nullptr);
    if (ret < 0 || !m_resampler) {
        emit errorOccurred(QString("Failed to allocate resampler context: %1").arg(av_make_error_string(errbuf, sizeof(errbuf), ret)));
    }

    if (swr_init(m_resampler) < 0) {
        emit errorOccurred("Failed to initialize resampler");
        return false;
    }

    m_bufferSamples = (m_codecCtx->sample_rate * m_bufferMsecSize) / 1000;
    m_fifo = av_audio_fifo_alloc(_format, m_codecCtx->ch_layout.nb_channels, m_bufferSamples);
    if (!m_fifo) {
        emit errorOccurred("Failed to allocate audio FIFO");
    }

    m_durationMs = av_rescale_q(m_stream->duration, m_stream->time_base, AVRational{1, 1000});
    emit durationChange(m_durationMs);

    return QIODevice::open(QIODevice::ReadOnly);
}

void FFmpegAudioDevice::close()
{
    if (m_formatCtx) {
        avformat_close_input(&m_formatCtx);
    }
    if (m_frame) {
        av_frame_free(&m_frame);
    }
    if (m_packet) {
        av_packet_free(&m_packet);
    }
    if (m_codecCtx) {
        avcodec_free_context(&m_codecCtx);
    }
    if (m_resampler) {
        swr_free(&m_resampler);
    }
}

FFmpegAudioDevice::~FFmpegAudioDevice()
{
    FFmpegAudioDevice::close();
    QIODevice::close();
}

qint64 FFmpegAudioDevice::readData(char *data, qint64 maxlen)
{
    if (!m_codecCtx || !m_fifo)
        return -1;

    const int sampleSize = av_get_bytes_per_sample(convertFormat(format().sampleFormat()));
    const int channels = m_codecCtx->ch_layout.nb_channels;
    const int samplesNeeded = maxlen / (sampleSize * channels);
    int samplesRead = 0;

    while (samplesRead < samplesNeeded) {
        int available = av_audio_fifo_size(m_fifo);

        if (m_eofReached && available == 0) {
            break;
        }

        if (available < m_bufferSamples && !m_eofReached) {
            bool decodingSuccessful = true;
            while (decodingSuccessful && av_audio_fifo_size(m_fifo) < m_bufferSamples) {
                decodingSuccessful = decodePacket();
            }
            available = av_audio_fifo_size(m_fifo);
        }

        if (available == 0) {
            break;
        }

        const int toRead = qMin(available, samplesNeeded - samplesRead);
        if (av_audio_fifo_read(m_fifo, reinterpret_cast<void **>(&data), toRead) != toRead) {
            emit errorOccurred("Failed to read from FIFO");
            break;
        }

        samplesRead += toRead;
        m_totalSamplesDecoded += toRead;

        m_positionMs = (m_totalSamplesDecoded * 1000) / m_codecCtx->sample_rate;
        emit positionChange(m_positionMs);
    }

    const qint64 bytesRead = samplesRead * sampleSize * channels;

    if (bytesRead > 0)
        AudioEffectManager::instance()->applyAll(data, bytesRead, format());

    return bytesRead;
}

bool FFmpegAudioDevice::decodePacket()
{
    int ret;

    while (true) {
        ret = avcodec_receive_frame(m_codecCtx, m_frame);
        if (ret == 0) {
            processFrame(m_frame);
        } else if (ret == AVERROR(EAGAIN)) {
            break;
        } else if (ret == AVERROR_EOF) {
            eof();
            return false;
        } else {
            emit errorOccurred(QString("Decoding error: %1").arg(av_make_error_string(errbuf, sizeof(errbuf), ret)));
            return false;
        }
    }

    while (!m_eofReached) {
        ret = av_read_frame(m_formatCtx, m_packet);
        if (ret < 0) {
            if (ret == AVERROR_EOF) {
                eof();
            }
            break;
        }

        if (m_packet->stream_index != m_stream->index) {
            av_packet_unref(m_packet);
            continue;
        }

        ret = avcodec_send_packet(m_codecCtx, m_packet);
        av_packet_unref(m_packet);

        if (ret < 0) {
            emit errorOccurred(QString("Error sending packet: %1").arg(av_make_error_string(errbuf, sizeof(errbuf), ret)));
            return false;
        }

        while (true) {
            ret = avcodec_receive_frame(m_codecCtx, m_frame);
            if (ret == 0) {
                processFrame(m_frame);
            } else if (ret == AVERROR(EAGAIN)) {
                break;
            } else if (ret == AVERROR_EOF) {
                eof();
                return false;
            } else {
                emit errorOccurred(QString("Decoding error: %1").arg(av_make_error_string(errbuf, sizeof(errbuf), ret)));
                return false;
            }
        }

        if (av_audio_fifo_size(m_fifo) >= m_bufferSamples) {
            break;
        }
    }

    return !m_eofReached;
}

bool FFmpegAudioDevice::processFrame(AVFrame *frame)
{
    if (!frame->data[0] || frame->nb_samples <= 0) {
        av_frame_unref(frame);
        return false;
    }

    uint8_t *convertedData[AV_NUM_DATA_POINTERS] = {nullptr};
    int out_samples = swr_get_out_samples(m_resampler, frame->nb_samples);
    int buffer_size = av_samples_alloc(convertedData, nullptr, m_codecCtx->ch_layout.nb_channels, out_samples,
                                       convertFormat(format().sampleFormat()), 0);

    if (buffer_size < 0) {
        emit errorOccurred("Failed to allocate conversion buffer");
        av_frame_unref(frame);
        return false;
    }

    out_samples = swr_convert(m_resampler, convertedData, out_samples, (const uint8_t **)frame->data, frame->nb_samples);

    if (out_samples < 0) {
        av_freep(&convertedData[0]);
        av_frame_unref(frame);
        emit errorOccurred("Failed to convert samples");
        return false;
    }

    if (av_audio_fifo_write(m_fifo, (void **)convertedData, out_samples) < out_samples) {
        av_freep(&convertedData[0]);
        av_frame_unref(frame);
        emit errorOccurred("Failed to write to FIFO");
        return false;
    }

    av_freep(&convertedData[0]);
    av_frame_unref(frame);
    return true;
}

qint64 FFmpegAudioDevice::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);
    return 0;
}

bool FFmpegAudioDevice::isSequential() const { return false; }

qint64 FFmpegAudioDevice::size() const
{
    if (!m_formatCtx)
        return 0;

    qint64 duration_sec = duration() / 1000;

    return duration_sec * m_codecCtx->sample_rate * m_codecCtx->ch_layout.nb_channels *
           av_get_bytes_per_sample(convertFormat(format().sampleFormat()));
}

bool FFmpegAudioDevice::seek(qint64 milliseconds)
{
    if (!m_formatCtx || milliseconds < 0)
        return false;

    int64_t timestamp = av_rescale_q(qBound(0, milliseconds, m_durationMs), AVRational{1, 1000}, m_stream->time_base);

    int ret = av_seek_frame(m_formatCtx, m_stream->index, timestamp, AVSEEK_FLAG_BACKWARD);
    if (ret < 0) {
        emit errorOccurred("Seek failed: " + QString(av_make_error_string(errbuf, sizeof(errbuf), ret)));
        return false;
    }

    avcodec_flush_buffers(m_codecCtx);
    av_audio_fifo_reset(m_fifo);

    m_positionMs = milliseconds;
    m_totalSamplesDecoded = (milliseconds * m_codecCtx->sample_rate) / 1000;
    emit positionChange(m_positionMs);

    return true;
}

void FFmpegAudioDevice::setBufferMsecSize(int msec)
{
    msec = qBound(50, msec, m_durationMs);

    m_bufferMsecSize = msec;
    if (m_codecCtx) {
        m_bufferSamples = (m_codecCtx->sample_rate * msec) / 1000;
    }
}

void FFmpegAudioDevice::eof()
{
    if (!m_ringEnabled) {
        m_eofReached = true;
        emit endOfFile();
    } else {
        seek(0);
    }
}

void FFmpegAudioDevice::enableRing(bool enable) { m_ringEnabled = enable; }

QAudioFormat FFmpegAudioDevice::format() const
{
    QAudioFormat fmt;
    fmt.setSampleFormat(QAudioFormat::Float);
    fmt.setSampleRate(m_codecCtx->sample_rate);
    fmt.setChannelCount(m_codecCtx->ch_layout.nb_channels);
    return fmt;
}

qint64 FFmpegAudioDevice::position() const { return m_positionMs; }

qint64 FFmpegAudioDevice::duration() const { return m_durationMs; }
