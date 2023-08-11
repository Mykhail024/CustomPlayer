#include <algorithm>
#include <iostream>
#include <vector>
#include <cassert>
#include <cstring>

#include <pipewire/stream.h>
#include <pipewire/thread-loop.h>
#include <spa/buffer/buffer.h>

#include "PipeWire.h"
#include "../TagReaders/TagReader.h"
#include "../Decoders/Decoder.h"

namespace AudioServers
{
	PipeWire::PipeWire()
	{
		loop_initialized = false;
		m_playback_state = STOPPED;
		pw_init(nullptr, nullptr);
	}

	PipeWire::~PipeWire()
	{
		stop();

		pw_deinit();
	}

	void PipeWire::play(const std::string &filePath)
	{
		if(loop_initialized)
		{
			pw_thread_loop_lock(loop);
			audio_buffer.erase(audio_buffer.begin(), audio_buffer.end());
            audio_buffer.shrink_to_fit();
		}
		else
		{
			loop = pw_thread_loop_new("pipewire-main-loop", nullptr);
		}

		auto id3v2 = TagReaders::id3v2_read(filePath);
		struct spa_audio_info_raw audio_info = SPA_AUDIO_INFO_RAW_INIT(
				.format = SPA_AUDIO_FORMAT_S16,
				.rate = id3v2.sampleRate,
				.channels = id3v2.channels
				);
		m_total_time = id3v2.length;


		uint8_t buffer[1024];
        const struct spa_pod *params[1];
		struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

		if(loop_initialized)
		{
			pw_stream_disconnect(stream);
		}
		else {
			stream = pw_stream_new_simple(
					pw_thread_loop_get_loop(loop),
					"Player",
					pw_properties_new(
						PW_KEY_MEDIA_TYPE, "Audio",
						PW_KEY_MEDIA_CATEGORY, "Playback",
						PW_KEY_MEDIA_ROLE, "Music",
						NULL),
					&stream_events,
					&stream_data);
		}


		m_channels = id3v2.channels;
		m_sampleRate = id3v2.sampleRate;
		audio_buffer = convertToPcm16(Decoders::decode_mp3(filePath).value());
		m_index = 0;
		if(!loop_initialized)
		{
            stream_data.data = &audio_buffer;
			stream_data.channels = &m_channels;
			stream_data.sampleRate = &m_sampleRate;
			stream_data.index = &m_index;
			stream_data.volume = &m_volume;
			stream_data.isLooping = &m_is_looping;
            stream_data.loop = loop;
            stream_data.stream = stream;
			stream_data.slider = timeSlider;
		}

        params[0] = spa_format_audio_raw_build(&b, SPA_PARAM_EnumFormat, &audio_info);

		pw_stream_connect(stream, PW_DIRECTION_OUTPUT, PW_ID_ANY, flags, params, 1);

		if(loop_initialized)
		{
			pw_thread_loop_unlock(loop);
		}

		pw_thread_loop_lock(loop);
		pw_thread_loop_start(loop);
		pw_thread_loop_unlock(loop);

		loop_initialized = true;
		m_playback_state = PLAYING;
	}

    void PipeWire::on_process(void *userdata)
    {
        auto stream_data = static_cast<data *>(userdata);
        struct pw_buffer *b;
        struct spa_buffer *buf;
        int16_t *dst;

        if ((b = pw_stream_dequeue_buffer(stream_data->stream)) == nullptr)
        {
            pw_log_warn("out of buffers: %m");
            return;
        }

        buf = b->buffer;
        assert(buf->datas[0].data != nullptr);

        size_t stride = sizeof(int16_t) * (*stream_data->channels);
        size_t n_frames = buf->datas[0].maxsize / stride;


        auto *dst_ptr = static_cast<int16_t *>(buf->datas[0].data);
        const size_t numFramesToCopy = std::min(n_frames, stream_data->data->size() - *stream_data->index);
        const size_t remainingFrames = n_frames - numFramesToCopy;

        // Copy the PCM data to the buffer, applying volume adjustment
        for (size_t i = 0; i < numFramesToCopy; i++)
        {
            for (int c = 0; c < (*stream_data->channels); c++)
            {
                // Apply volume adjustment to each sample
                auto sample = static_cast<int16_t>((*stream_data->data)[*stream_data->index] * (*stream_data->volume / 100.0));
                *dst_ptr++ = sample;
                (*stream_data->index)++;
            }
        }

        if (remainingFrames > 0)
        {
            std::memset(dst_ptr, 0, remainingFrames * stride);
        }

        buf->datas[0].chunk->offset = 0;
        buf->datas[0].chunk->stride = static_cast<int32_t>(stride);
        buf->datas[0].chunk->size = n_frames * stride;

        pw_stream_queue_buffer(stream_data->stream, b);

		if(stream_data->slider)
		{
			stream_data->slider->setValueDontMove((*stream_data->index / *stream_data->sampleRate) / *stream_data->channels);
		}

        if (*stream_data->index >= stream_data->data->size())
        {
			if(*stream_data->isLooping)
			{
				*stream_data->index = 0;
			}
			else
			{
				pw_thread_loop_lock(stream_data->loop);
				pw_stream_set_active(stream_data->stream, false);
				pw_thread_loop_unlock(stream_data->loop);
			}
        }
    }

	void PipeWire::pause()
	{
		if(stream)
		{
			pw_thread_loop_lock(loop);
			if(m_playback_state == PLAYING)
			{
				pw_stream_set_active(stream, false);
				m_playback_state = PAUSED;
			}
			pw_thread_loop_unlock(loop);
		}
	}

	void PipeWire::resume()
	{
		if(stream)
		{
			pw_thread_loop_lock(loop);
			if(m_playback_state == PAUSED)
			{
				pw_stream_set_active(stream, true);
				m_playback_state = PLAYING;
			}
			pw_thread_loop_unlock(loop);
		}
	}

	void PipeWire::stop() {
		if(m_playback_state == STOPPED)
		{
			return;
		}
		if (stream)
		{
			pw_thread_loop_lock(loop);
			pw_stream_flush(stream, false);
			pw_stream_destroy(stream);
            stream_data.stream = nullptr;
			stream = nullptr;
			pw_thread_loop_unlock(loop);
		}

		if (stream_data.loop)
		{
			pw_thread_loop_stop(loop);
			pw_thread_loop_destroy(loop);
            stream_data.loop = nullptr;
			loop = nullptr;
		}
		loop_initialized = false;
		audio_buffer.erase(audio_buffer.begin(), audio_buffer.end());
        audio_buffer.shrink_to_fit();
	}

	void PipeWire::setVolume(int volume) {
		m_volume = std::clamp(volume, 0, 100);
	}

    void PipeWire::goTo(unsigned int second) {
		m_index = (second * m_sampleRate) * m_channels;
    }

    void PipeWire::setLooping(bool isLooping) {
		m_is_looping = isLooping;
    }

	unsigned int PipeWire::getTotalTime() const
	{
		return m_total_time;
	}
    unsigned int PipeWire::getCurrentTime() const
	{
		return (m_index * 1000) / m_sampleRate;
	}
	void PipeWire::registerTimeSliderCallback(Widgets::TimeSlider *slider)
	{
		timeSlider = slider;
	}
}
