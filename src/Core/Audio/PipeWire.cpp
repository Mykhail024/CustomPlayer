#include <format>

#include <QTimer>
#include <QThread>
#include <QtMath>

#include <pipewire/loop.h>
#include <pipewire/properties.h>
#include <pipewire/stream.h>
#include <pipewire/thread-loop.h>

#include "Core/EventHandler.h"
#include "Core/Globals.h"
#include "Core/Log.h"
#include "Core/Tools.h"

#define format_cstr(str, ...) std::format(str, ##__VA_ARGS__).c_str()

#include "PipeWire.h"

#include "moc_PipeWire.cpp"

namespace Audio {
    PipeWire::PipeWire()
    {
        memset(&m_fileinfo, 0, sizeof(m_fileinfo));

        pw_init(nullptr, nullptr);

        m_loop = pw_thread_loop_new("Custom-Player-PipeWire", nullptr);
        if(!m_loop) {
            Log_Error("PipeWireOut: unable to create thread loop");
        }

        m_context = pw_context_new(
                pw_thread_loop_get_loop(m_loop),
                pw_properties_new(PW_KEY_CONFIG_NAME, "client-rt.conf", nullptr),
                0);
        if(!m_context) {
            Log_Error("PipeWireOut: unable to create context");
        }

        m_core = pw_context_connect(m_context, nullptr, 0);
        if(!m_core) {
            Log_Error("PipeWireOut: unable to connect context");
        }

        pw_loop_add_signal(pw_thread_loop_get_loop(m_loop), SIGINT, do_quit, this);
        pw_loop_add_signal(pw_thread_loop_get_loop(m_loop), SIGTERM, do_quit, this);

        pw_thread_loop_start(m_loop);
    }

    PipeWire::~PipeWire()
    {
        stopFadeIn();
        do_quit(this, 0);
        pw_thread_loop_lock(m_loop);
        clear();
        pw_thread_loop_unlock(m_loop);
        if(m_loop) {
            pw_thread_loop_destroy(m_loop);
        }
        pw_deinit();
    }

    void PipeWire::stop()
    {
        stopFadeIn();
        pw_thread_loop_lock(m_loop);
        if(m_stream) {
            pw_stream_set_active(m_stream, false);
        }
        pw_thread_loop_unlock(m_loop);
    }

    void PipeWire::on_process(void *userdata)
    {
        PipeWire* data = static_cast<PipeWire*>(userdata);

        struct pw_buffer *b;

        if(!(b = pw_stream_dequeue_buffer(data->m_stream))) {
            Log_Debug("PipeWireOut: out of buffers");
            return;
        }

        float *buf = static_cast<float*>(b->buffer->datas[0].data);
        if(!buf) {
            return;
        }

        uint32_t stride = sizeof(float) * data->m_fileinfo.channels;

        uint32_t n_frames = b->buffer->datas[0].maxsize / stride;
        if(b->requested) {
            n_frames = SPA_MIN(n_frames, b->requested);
        }

        sf_count_t current = 0;
        while(current < n_frames) {
            if(data->m_time_goto != -1) {
                sf_seek(data->m_file, data->m_time_goto, SEEK_SET);
                data->m_time_goto = -1;
            }

            sf_count_t ret = sf_readf_float(data->m_file,
                    &buf[current * data->m_fileinfo.channels],
                    n_frames - current);

            if(ret < 0) {
                Log_Warning("file reading error");
                goto error;
            }
            current += ret;

            if(current != n_frames) {
                if (sf_seek(data->m_file, 0, SEEK_CUR) == data->m_fileinfo.frames) {
                    if(globals()->loopState()) {
                        if (sf_seek(data->m_file, 0, SEEK_SET) < 0) {
                            Log_Warning("file seek error");
                            goto error;
                        }
                        emit eventHandler()->FadeIn(false);
                    } else {
                        if(!data->m_end_request) {
                            emit eventHandler()->EndSong();
                            data->m_end_request = true;
                        }
                        b->buffer->datas[0].chunk->offset = 0;
                        b->buffer->datas[0].chunk->stride = 0;
                        b->buffer->datas[0].chunk->size = 0;
                        pw_stream_queue_buffer(data->m_stream, b);
                        data->stop();
                        return;
                    }
                }
            }
        }
        if(data->m_time_goto == -1) {
            eventHandler()->PositionChange(sf_seek(data->m_file, 0, SEEK_CUR) * 1000 / data->m_fileinfo.samplerate);
        }

        for (uint32_t i = 0; i < n_frames * data->m_fileinfo.channels; i++) {
            buf[i] *= data->m_volume;
        }

        b->buffer->datas[0].chunk->offset = 0;
        b->buffer->datas[0].chunk->stride = stride;
        b->buffer->datas[0].chunk->size = n_frames * stride;
        pw_stream_queue_buffer(data->m_stream, b);
        return;

error:
        pw_thread_loop_stop(data->m_loop);
        b->buffer->datas[0].chunk->offset = 0;
        b->buffer->datas[0].chunk->stride = 0;
        b->buffer->datas[0].chunk->size = 0;
        pw_stream_queue_buffer(data->m_stream, b);
    }

    void PipeWire::do_quit(void *userdata, int signal_number)
    {
        PipeWire* data = static_cast<PipeWire*>(userdata);
        pw_thread_loop_stop(data->m_loop);
    }

    void PipeWire::clear()
    {
        if(m_started) {
            if(m_stream) {
                pw_stream_disconnect(m_stream);
                pw_stream_destroy(m_stream);
                m_stream = nullptr;
            }
            if(m_file) {
                sf_close(m_file);
                m_file = nullptr;
            }
            spa_hook_remove(&m_event_listener);
            m_started = false;
        }
    }

    const unsigned int getQuant(const int &rate)
    {
        return qMax(64, qMin(qCeil(2048 * rate / 48000.0f), 8192));
    }

    struct pw_properties* create_props(const sf_count_t &frames, const int &samplerate)
    {
        struct pw_properties *props =
            pw_properties_new(
                    PW_KEY_MEDIA_TYPE, "Audio",
                    PW_KEY_MEDIA_CATEGORY, "Playback",
                    PW_KEY_MEDIA_ROLE, "Music",
                    PW_KEY_APP_ID, "customplayer",
                    PW_KEY_APP_ICON_NAME, "customplayer",
                    PW_KEY_APP_NAME, "Custom Player",
                    PW_KEY_NODE_RATE, format_cstr("1/{}", samplerate),
                    PW_KEY_NODE_LATENCY, format_cstr("{}/{}", getQuant(samplerate), samplerate),
                    nullptr);
        return props;
    }

    bool PipeWire::play(const QString &filePath)
    {
        pw_thread_loop_lock(m_loop);

        SF_INFO info;
        SNDFILE *file;

        memset(&info, 0, sizeof(info));
        file = sf_open(filePath.toStdString().c_str(), SFM_READ, &info);
        if(!file) {
            Log_Warning("Error open file: " + filePath);
            pw_thread_loop_unlock(m_loop);
            return false;
        }

        if(!m_started) {
            m_stream = pw_stream_new(m_core, "CustomPlayerStream", create_props(info.frames, info.samplerate));
            if(!m_stream) {
                Log_Warning("Failed create pipewire stream");
                pw_thread_loop_unlock(m_loop);
                return false;
            }

            pw_stream_add_listener(m_stream, &m_event_listener, &stream_events, this);
            eventHandler()->VolumeChange(globals()->volume());
        }

        uint8_t buffer[1024];
        struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

        struct spa_audio_info_raw spa_audio_info =
            SPA_AUDIO_INFO_RAW_INIT(
                    .format = SPA_AUDIO_FORMAT_F32_LE,
                    .rate = (uint32_t)info.samplerate,
                    .channels = (uint32_t)info.channels
                    );

        const struct spa_pod *params[1] = {spa_format_audio_raw_build(&b,SPA_PARAM_EnumFormat, &spa_audio_info)};

        if(!m_started)
        {
            m_file = file;
            memmove(&m_fileinfo, &info, sizeof(info));
            if(pw_stream_connect(m_stream, PW_DIRECTION_OUTPUT, PW_ID_ANY, m_stream_flags, params, 1) != 0) {
                Log_Warning("Error connect stream");
            }
        } else {
            pw_stream_set_active(m_stream, false);
            pw_stream_flush(m_stream, false);

            struct spa_dict_item items[] = {
                SPA_DICT_ITEM_INIT(PW_KEY_NODE_RATE, format_cstr("1/{}", info.samplerate)),
                SPA_DICT_ITEM_INIT(PW_KEY_NODE_LATENCY, format_cstr("{}/{}", getQuant(info.samplerate), info.samplerate))
            };
            const spa_dict dict = SPA_DICT_INIT(items, 2);

            pw_stream_update_properties(m_stream, &dict);
            pw_stream_update_params(m_stream, params, 1);

            sf_close(m_file);

            m_file = file;
            memset(&m_fileinfo, 0, sizeof(m_fileinfo));
            memmove(&m_fileinfo, &info, sizeof(info));

            pw_stream_set_active(m_stream, true);
        }
        m_end_request = false;
        Log_Debug("Play: " + filePath);

        pw_thread_loop_unlock(m_loop);

        m_started = true;
        return true;
    }

    bool PipeWire::pause()
    {
        if(!m_stream) {
            return false;
        }
        pw_thread_loop_lock(m_loop);
        pw_stream_set_active(m_stream, false);
        pw_thread_loop_unlock(m_loop);

        return true;
    }

    bool PipeWire::resume()
    {
        if(!m_stream) {
            return false;
        }
        pw_thread_loop_lock(m_loop);
        pw_stream_set_active(m_stream, true);
        pw_thread_loop_unlock(m_loop);

        return true;
    }

    void PipeWire::setVolume(const float &volume)
    {
        if(globals()->softwareVolumeControl()) {
            setSoftwareVolume(volume);
        } else {
            if(m_volume != 1.0f) {
                m_volume = 1.0f;
            }
            setPipewireVolume(volume);
        }
    }

    void PipeWire::setPipewireVolume(float volume)
    {
        if(!m_stream) {
            return;
        }

        pw_thread_loop_lock(m_loop);
        float volumes[m_fileinfo.channels];
        if(m_fileinfo.channels == 2) {
            volumes[0] = volume;
            volumes[1] = volume;
        } else {
            for(int i = 0; i < m_fileinfo.channels; i++) {
                volumes[i] = volume;
            }
        }
        pw_stream_set_control(m_stream, SPA_PROP_channelVolumes, m_fileinfo.channels, volumes, nullptr);
        pw_thread_loop_unlock(m_loop);
    }

    void PipeWire::setSoftwareVolume(float volume)
    {
        m_volume = volume;
    }

    void PipeWire::goTo(const unsigned int &time)
    {
        m_time_goto = (time / 1000 * m_fileinfo.samplerate);
    }

    void PipeWire::fadeIn(unsigned int milliseconds)
    {
        m_fadeThread = new QThread;

        QObject::connect(m_fadeThread, &QThread::finished, [&]() {
                m_fadeThread->deleteLater();
                m_fadeThread = nullptr;
                });

        QObject::connect(m_fadeThread, &QThread::started, [&, milliseconds]() {
                    float currentVolume = 0.0f;
                    float targetVolume = m_volume;
                    float step = targetVolume / milliseconds;
                    while (currentVolume < targetVolume && !m_fadeThread->isInterruptionRequested()) {
                        setSoftwareVolume(currentVolume);
                        currentVolume += step;
                        QThread::msleep(1);
                    }
                    setSoftwareVolume(targetVolume);

                    m_fadeThread->quit();
                });

        m_fadeThread->start();
    }

    void PipeWire::stopFadeIn()
    {
        if (m_fadeThread && m_fadeThread->isRunning()) {
            m_fadeThread->requestInterruption();
            m_fadeThread->wait();
        }
    }
}
