#pragma once

#include <QAudioFormat>
#include <QByteArray>
#include <QIODevice>
#include <QMutex>

class AVFormatContext;
class AVCodecContext;
class SwrContext;
class AVPacket;
class AVAudioFifo;
class AVFrame;
class AVStream;

class FFmpegAudioDevice : public QIODevice
{
        Q_OBJECT
    public:
        explicit FFmpegAudioDevice(const QString &file, QObject *parent = nullptr);
        ~FFmpegAudioDevice();

        qint64 size() const override;
        bool isSequential() const override;

        QAudioFormat format() const;

        qint64 position() const;
        qint64 duration() const;

    public slots:
        void enableRing(bool enable);
        void setBufferMsecSize(int msec);
        bool open(QIODeviceBase::OpenMode mode) override;
        void close() override;
        bool seek(qint64 milliseconds) override;

    signals:
        void errorOccurred(const QString &message);

        void durationChange(qint64 duration);
        void positionChange(qint64 position);
        void endOfFile();

    protected:
        qint64 readData(char *data, qint64 maxlen) override;
        qint64 writeData(const char *data, qint64 len) override;

    private slots:
        void eof();

    private:
        bool decodePacket();
        bool processFrame(AVFrame *frame);

        const QString m_file;
        qint64 m_totalBytes;
        qint64 m_totalSamplesDecoded = 0;
        qint64 m_durationMs = 0;
        qint64 m_positionMs = 0;
        int m_bufferMsecSize = 1000;
        qint64 m_bufferSamples = 0;
        bool m_eofReached = false;
        bool m_ringEnabled = false;

        AVFormatContext *m_formatCtx = nullptr;
        AVCodecContext *m_codecCtx = nullptr;
        SwrContext *m_resampler = nullptr;
        AVPacket *m_packet = nullptr;
        AVFrame *m_frame = nullptr;
        AVStream *m_stream = nullptr;

        AVAudioFifo *m_fifo;
};
