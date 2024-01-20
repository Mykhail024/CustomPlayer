#pragma once

#include <QThread>
#include <QObject>

#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>
#include <spa/param/props.h>
#include <sndfile.h>

#include "AudioServer.h"

namespace Audio {
    class PipeWire : public AudioServer
    {
        Q_OBJECT
        public:
            PipeWire();
            ~PipeWire();


        public slots:
            void setVolume(const float &volume) override;
            void fadeIn(unsigned int milliseconds) override;
            void stopFadeIn() override;
            void goTo(const unsigned int &time) override;
            bool play(const QString &filePath) override;
            bool pause() override;
            bool resume() override;
            void stop() override;

        private:
            void setSoftwareVolume(float volume);
            void setPipewireVolume(float volume);
            static void on_process(void *userdata);
            static void do_quit(void *userdata, int signal_number);
            void clear();

            static constexpr struct pw_stream_events stream_events = {
                .version = PW_VERSION_STREAM_EVENTS,
                .process = on_process,
            };
            struct data {
                PipeWire *pipewire;

                SNDFILE *file;
                SF_INFO fileinfo;
            };

            float m_volume = 1.0f;
            unsigned long int m_time_goto = -1;
            bool m_started = false;
            bool m_end_request = false;

            struct pw_thread_loop *m_loop = nullptr;
            struct pw_stream *m_stream = nullptr;
            struct pw_context *m_context = nullptr;
            struct pw_core *m_core = nullptr;

            struct spa_hook m_event_listener;

            struct data m_data = {this};

            const static pw_stream_flags m_stream_flags = static_cast<pw_stream_flags>(
                    PW_STREAM_FLAG_AUTOCONNECT |
                    PW_STREAM_FLAG_MAP_BUFFERS |
                    PW_STREAM_FLAG_RT_PROCESS);

            QThread *m_fadeThread = nullptr;
    };
}
