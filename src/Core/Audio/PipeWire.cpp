#include <QTimer>
#include <QThread>
#include <chrono>
#include <math.h>
#include <pipewire/stream.h>
#include <pipewire/thread-loop.h>
#include <qlogging.h>
#include <thread>

#include "Core/EventHandler.h"
#include "Core/Globals.h"
#include "Core/Log.h"

#include "PipeWire.h"

#include "moc_PipeWire.cpp"

namespace Audio {
	PipeWire::PipeWire()
	{
		pw_init(nullptr, nullptr);

		loop = pw_thread_loop_new("Custom-Player-PipeWire", nullptr);
		if(!loop)
		{
			Log_Error("PipeWireOut: unable to create thread loop");
		}

		struct pw_context *context = pw_context_new(
				pw_thread_loop_get_loop(loop),
				pw_properties_new(PW_KEY_CONFIG_NAME, "client-rt.conf", nullptr),
				0);
		if(!context)
		{
			Log_Error("PipeWireOut: unable to create context");
		}

		core = pw_context_connect(context, nullptr, 0);
		if(!core)
		{
			Log_Error("PipeWireOut: unable to connect context");
		}

		pw_loop_add_signal(pw_thread_loop_get_loop(loop), SIGINT,
				do_quit, &data);
		pw_loop_add_signal(pw_thread_loop_get_loop(loop), SIGTERM,
				do_quit, &data);

        pw_thread_loop_start(loop);
	}
	PipeWire::~PipeWire()
	{
		stopFadeIn();
		do_quit(&data, 0);
        pw_thread_loop_lock(loop);
		clear();
		pw_thread_loop_unlock(loop);
		if(loop)
		{
			pw_thread_loop_destroy(loop);
		}
		pw_deinit();
	}

	void PipeWire::stop()
	{
		stopFadeIn();
		pw_thread_loop_lock(loop);
		clear();
		pw_thread_loop_unlock(loop);
	}

	void PipeWire::on_process(void *userdata)
	{
		struct data *data = static_cast<struct data*>(userdata);

		struct pw_buffer *b;

		if(!(b = pw_stream_dequeue_buffer(data->pipewire->stream)))
		{
			Log_Debug("PipeWireOut: out of buffers");
			return;
		}

		float *buf = static_cast<float*>(b->buffer->datas[0].data);
		if(!buf)
		{
			return;
		}

		uint32_t stride = sizeof(float) * data->fileinfo.channels;

		uint32_t n_frames = b->buffer->datas[0].maxsize / stride;
		if(b->requested)
		{
			n_frames = SPA_MIN(n_frames, b->requested);
		}

		sf_count_t current = 0;
		while(current < n_frames)
		{
			if(data->pipewire->time_goto != -1)
			{
				sf_seek(data->file, data->pipewire->time_goto, SEEK_SET);
				data->pipewire->time_goto = -1;
			}

			sf_count_t ret = sf_readf_float(data->file,
					&buf[current * data->fileinfo.channels],
					n_frames - current);

			if(ret < 0)
			{
				Log_Warning("file reading error");
				goto error_after_dequeue;
			}
			current += ret;

			if(current != n_frames)
			{
				if (sf_seek(data->file, 0, SEEK_CUR) == data->fileinfo.frames)
				{
					if(globals()->loopState())
					{
						if (sf_seek(data->file, 0, SEEK_SET) < 0)
						{
							Log_Warning("file seek error");
							goto error_after_dequeue;
						}
						emit eventHandler()->FadeIn(false);
					}
					else
					{
						pw_thread_loop_signal(data->pipewire->loop, 0);
						emit eventHandler()->EndSong();
						return;
					}
				}
			}

		}

		eventHandler()->PositionChange(sf_seek(data->file, 0, SEEK_CUR) * 1000 / data->fileinfo.samplerate);

		for (uint32_t i = 0; i < n_frames * data->fileinfo.channels; i++)
		{
			buf[i] *= data->pipewire->m_volume;
		}

		b->buffer->datas[0].chunk->offset = 0;
		b->buffer->datas[0].chunk->stride = stride;
		b->buffer->datas[0].chunk->size = n_frames * stride;
		pw_stream_queue_buffer(data->pipewire->stream, b);
		return;

error_after_dequeue:
		pw_thread_loop_stop(data->pipewire->loop);
		b->buffer->datas[0].chunk->offset = 0;
		b->buffer->datas[0].chunk->stride = 0;
		b->buffer->datas[0].chunk->size = 0;
		pw_stream_queue_buffer(data->pipewire->stream, b);
	}
	void PipeWire::do_quit(void *userdata, int signal_number)
	{
		struct data *data = static_cast<struct data*>(userdata);
		pw_thread_loop_stop(data->pipewire->loop);
	}

	bool PipeWire::play(const QString &filePath)
	{
        pw_thread_loop_lock(loop);

		clear();

		memset(&data.fileinfo, 0, sizeof(data.fileinfo));
		data.file = sf_open(filePath.toStdString().c_str(), SFM_READ, &data.fileinfo);
		if(!data.file)
		{
			Log_Warning("Error open file: " + filePath);
			pw_thread_loop_unlock(loop);
			return false;
		}

		char rate_str[64];
		snprintf(rate_str, sizeof(rate_str), "1/%u",
				data.fileinfo.samplerate);

		stream = pw_stream_new(
				core,
				"CustomPlayerStream",
				pw_properties_new(
					PW_KEY_MEDIA_TYPE, "Audio",
					PW_KEY_MEDIA_CATEGORY, "Playback",
					PW_KEY_MEDIA_ROLE, "Music",
					PW_KEY_APP_ID, "customplayer",
					PW_KEY_APP_ICON_NAME, "customplayer",
					PW_KEY_APP_NAME, "Custom Player",
					nullptr));

		pw_stream_add_listener(stream, &event_listener, &stream_events, &data);

		uint8_t buffer[1024];
		struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

		struct spa_audio_info_raw spa_audio_info =
			SPA_AUDIO_INFO_RAW_INIT(
					.format = SPA_AUDIO_FORMAT_F32,
					.rate = (uint32_t)data.fileinfo.samplerate,
					.channels = (uint32_t)data.fileinfo.channels
					);

		params[0] = spa_format_audio_raw_build(&b,SPA_PARAM_EnumFormat, &spa_audio_info);

		pw_stream_connect(stream,
				PW_DIRECTION_OUTPUT,
				PW_ID_ANY,
				stream_flags,
				params, 1);

        pw_thread_loop_unlock(loop);

		eventHandler()->VolumeChange(globals()->volume());
		emit eventHandler()->FadeIn(true);

        started = true;
		return true;
	}
	bool PipeWire::pause()
	{
		if(!stream)
		{
			return false;
		}
		pw_thread_loop_lock(loop);
		pw_stream_set_active(stream, false);
		pw_thread_loop_unlock(loop);

		return true;
	}
	bool PipeWire::resume()
	{
		if(!stream)
		{
			return false;
		}
		pw_thread_loop_lock(loop);
		pw_stream_set_active(stream, true);
		pw_thread_loop_unlock(loop);

		return true;
	}
	void PipeWire::setVolume(const float &volume)
	{
		if(globals()->softwareVolumeControl())
		{
			setPipewireVolume(1.0f);
			setSoftwareVolume(volume);
		}
		else
		{
			if(m_volume != 1.0f)
			{
				m_volume = 1.0f;
			}
			setPipewireVolume(volume);
		}
	}

	void PipeWire::setPipewireVolume(float volume)
	{
		if(!stream)
		{
			return;
		}

		pw_thread_loop_lock(loop);
		float volumes[data.fileinfo.channels];
		if(data.fileinfo.channels == 2)
		{
			volumes[0] = volume;
			volumes[1] = volume;
		}
		else
		{
			for(int i = 0; i < data.fileinfo.channels; i++)
			{
				volumes[i] = volume;
			}
		}
		pw_stream_set_control(stream, SPA_PROP_channelVolumes, data.fileinfo.channels, volumes, nullptr);
		pw_thread_loop_unlock(loop);
	}

	void PipeWire::setSoftwareVolume(float volume)
	{
		m_volume = volume;
	}

	void PipeWire::goTo(const unsigned int &time)
	{
		time_goto = (time / 1000 * data.fileinfo.samplerate);
	}

	void PipeWire::fadeIn(unsigned int milliseconds)
	{
		fadeThread = new QThread;

		QObject::connect(fadeThread, &QThread::finished, [=]() {
				fadeThread->deleteLater();
				fadeThread = nullptr;
				});

		QObject::connect(fadeThread, &QThread::started, [=]() {
					float currentVolume = 0.0f;
					float targetVolume = m_volume;
					float step = targetVolume / milliseconds;
					while (currentVolume < targetVolume && !fadeThread->isInterruptionRequested()) {
						setSoftwareVolume(currentVolume);
						currentVolume += step;
						QThread::msleep(1);
					}
					setSoftwareVolume(targetVolume);

					fadeThread->quit();
				});

		fadeThread->start();
	}

	void PipeWire::stopFadeIn()
	{
		if (fadeThread && fadeThread->isRunning()) {
			fadeThread->requestInterruption();
			fadeThread->wait();
		}
	}

	void PipeWire::clear()
	{
        if(started)
		{
			if(stream)
			{
				pw_stream_disconnect(stream);
				pw_stream_destroy(stream);
				stream = nullptr;
			}
			if(data.file)
			{
				sf_close(data.file);
				data.file = nullptr;

			}
			spa_hook_remove(&event_listener);
        }
	}
}
