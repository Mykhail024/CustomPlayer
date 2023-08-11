#pragma once

#include <vector>

#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>

#include "AudioServer.h"

namespace AudioServers
{
    class PipeWire : public AudioServer
    {
    public:
        PipeWire();
        ~PipeWire();
        void play(const std::string &filePath) override;
        void pause() override;
        void stop() override;
		void resume() override;
        void setVolume(int volume) override;
        void goTo(unsigned int second) override;
        void setLooping(bool isLooping) override;
		void registerTimeSliderCallback(Widgets::TimeSlider *slider) override;

        [[nodiscard]] bool isPlaying() const override { return (m_playback_state == PLAYING); };
        [[nodiscard]] int getVolume() const override { return m_volume; }
        [[nodiscard]] unsigned int getTotalTime() const override;
        [[nodiscard]] PLAYBACK_STATE getPlaybackState() const override { return m_playback_state; }
        [[nodiscard]] unsigned int getCurrentTime() const override;

    private:
        PLAYBACK_STATE m_playback_state = STOPPED;
        int m_volume = 50;
		float m_speed = 1.0;
        unsigned int m_total_time;
		bool m_is_looping = false;
		int m_index = 0;

        unsigned int m_channels;
        unsigned int m_sampleRate;

		Widgets::TimeSlider *timeSlider = nullptr;

		//PipeWire
		bool loop_initialized;

        struct pw_thread_loop *loop;
        struct pw_stream *stream;

        pw_stream_flags flags =
                static_cast<enum pw_stream_flags>(PW_STREAM_FLAG_AUTOCONNECT |
                                                  PW_STREAM_FLAG_MAP_BUFFERS |
                                                  PW_STREAM_FLAG_RT_PROCESS);

		struct data {
			struct pw_thread_loop *loop;
			struct pw_stream *stream;
			unsigned int *sampleRate;
			unsigned int *channels;
			int *volume;
			std::vector<int16_t> *data;
			int *index;
			bool *isLooping;
			Widgets::TimeSlider *slider;
		};
		struct data stream_data;

        std::vector<int16_t> audio_buffer;

		static void on_process(void *userdata);
		const struct pw_stream_events stream_events = {
			.process = on_process
		};
	};
}
