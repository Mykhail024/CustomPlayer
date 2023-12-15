#pragma once

#include <QThread>
#include <QObject>

#include <pipewire/pipewire.h>
#include <sndfile.h>
#include <spa/param/audio/format-utils.h>
#include <spa/param/props.h>

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
			float m_volume = 1.0f;
			unsigned long int time_goto = -1;

            bool started = false;

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

			struct pw_thread_loop *loop = nullptr;
			struct pw_stream *stream = nullptr;
			struct pw_context *context = nullptr;
			struct pw_core *core = nullptr;

			struct spa_hook event_listener;

			struct data data = {this};
            const struct spa_pod *params[1];

			pw_stream_flags stream_flags = static_cast<pw_stream_flags>(
					PW_STREAM_FLAG_AUTOCONNECT |
					PW_STREAM_FLAG_MAP_BUFFERS |
					PW_STREAM_FLAG_RT_PROCESS);

			QThread *fadeThread = nullptr;
	};
}
