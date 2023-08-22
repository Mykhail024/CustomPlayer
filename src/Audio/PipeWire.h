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
			bool play(std::string filePath) override;
			bool pause() override;
			bool resume() override;
			void setVolume(float volume) override;
			void goTo(int time) override;
			float getVolume() const override;
			void setLooped(bool is_loop) override;
			bool getLoopStatus() override { return m_is_looped; }
			void fadeIn(unsigned int milliseconds) override;
			void stopFadeIn();
			void stop() override;

		private:
			void setVolumeInternal(float volume);
			float m_volume = 1.0f;
			bool m_is_looped = false;
			int time_goto = -1;

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
