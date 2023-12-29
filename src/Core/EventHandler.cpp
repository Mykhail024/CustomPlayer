#include <stdexcept>

#include "Audio/AudioServer.h"
#include "Globals.h"
#include "Log.h"
#include "TagReaders/TagReader.h"

#include "EventHandler.h"

#include "moc_EventHandler.cpp"

EventHandler::EventHandler()
{
	connect(this, &EventHandler::onVolumeChange, globals(), &Globals::startVolumeSaveTimer);
	connect(this, &EventHandler::onLoopStateChange, globals(), &Globals::save);
	connect(this, &EventHandler::onShuffleStateChange, globals(), &Globals::save);

	connect(this, &EventHandler::onFadeIn, globals()->audioServer(), &Audio::AudioServer::fadeIn);
	connect(this, &EventHandler::onStopFadeIn, globals()->audioServer(), &Audio::AudioServer::stopFadeIn);
	connect(this, &EventHandler::onVolumeChange, globals()->audioServer(), &Audio::AudioServer::setVolume);

	connect(this, &EventHandler::onPlaySong, globals()->audioServer(), &Audio::AudioServer::play);
	connect(this, &EventHandler::onPlay, globals()->audioServer(), &Audio::AudioServer::resume);
	connect(this, &EventHandler::onPause, globals()->audioServer(), &Audio::AudioServer::pause);
	connect(this, &EventHandler::onStop, globals()->audioServer(), &Audio::AudioServer::stop);

	connect(this, &EventHandler::onSeek, globals()->audioServer(), &Audio::AudioServer::goTo);
}
void EventHandler::EndSong()
{
	emit onEndSong();
	globals()->m_playbackStatus.state = STOPPED;
	emit onPlaybackStatusChanged();
	if(!globals()->m_loopState)
	{
		if(globals()->m_shuffleState)
		{
			NextSongRandom();
		}
		else
		{
			NextSong();
		}
	}
}
void EventHandler::PlayPause()
{
	if(globals()->m_playbackStatus.state == PLAYING)
	{
		Pause();
	}
	else
	{
		Play();
	}
}
void EventHandler::NextSong()
{
	if(globals()->m_playbackStatus.canNext)
	{
		if(globals()->m_shuffleState)
		{
			NextSongRandom();
		}
		else
		{
			emit onNextSong();
		}
	}
}
void EventHandler::PrevSong()
{
	if(globals()->m_playbackStatus.canPrev)
	{
		emit onPrevSong();
	}
}
bool EventHandler::PlaySong(const QString &path)
{
	bool ok = onPlaySong(path);
	if(ok)
	{
		auto metadata = TagReaders::id3v2_read(path);
		metadata.ArtUrl = TagReaders::id3v2_get_image_path(metadata.Path);
		globals()->m_metadata = metadata;

		globals()->m_playbackStatus.state = PLAYING;
		globals()->m_playbackStatus.canPlay = true;

		emit onPlaybackStatusChanged();
		emit onPlaySong(path);
	}
	return ok;
}
void EventHandler::NextSongRandom()
{
	emit onNextSongRandom();
}
bool EventHandler::Play()
{
	FadeIn(false);
	bool ok = onPlay();
	if(ok)
	{
		globals()->m_playbackStatus.state = PLAYING;
		globals()->m_playbackStatus.canPlay = true;
		emit onPlaybackStatusChanged();
	}

	return ok;
}
bool EventHandler::Pause()
{
	bool ok = onPause();
	if(ok)
	{
		globals()->m_playbackStatus.state = PAUSED;
		globals()->m_playbackStatus.canPlay = true;
		emit onPlaybackStatusChanged();
	}
	return ok;
}
void EventHandler::Stop()
{
	emit onStop();
	globals()->m_playbackStatus.state = STOPPED;
	globals()->m_playbackStatus.canPlay = false;
	emit onPlaybackStatusChanged();
}
void EventHandler::VolumeChange(const float& volume)
{
	StopFadeIn();
	globals()->m_volume = volume;
	emit onVolumeChange(volume);
	if(volume == 0.0f)
	{
		globals()->m_isMuted = true;
	}
	else
	{
		globals()->m_isMuted = false;
	}
}
void EventHandler::LoopStateChange(const bool &state)
{
	globals()->m_loopState = state;
	emit onLoopStateChange(state);
}
void EventHandler::ShuffleStateChange(const bool &state)
{
	globals()->m_shuffleState = state;
	emit onShuffleStateChange(state);
}
void EventHandler::PositionChange(const unsigned long int &pos)
{
	globals()->m_songPosition = pos;
	emit onPositionChange(pos);
}
void EventHandler::FadeIn(const bool &isPrimary)
{
	StopFadeIn();
	if(isPrimary)
	{
		emit onFadeIn(globals()->m_fadeInTime_Primary);
	}
	else
	{
		emit onFadeIn(globals()->m_fadeInTime_Secondary);
	}
}
void EventHandler::StopFadeIn()
{
	emit onStopFadeIn();
}
void EventHandler::Seek(const unsigned long int &time)
{
	if(globals()->playbackStatus().state == STOPPED) return;

	FadeIn(false);
	emit onSeek(time);
	globals()->m_songPosition = time;
}
void EventHandler::Rise()
{

}
void EventHandler::PlaylistFind(const QString &text)
{
	emit onPlaylistFind(text);
}
void EventHandler::NextPlaylist()
{
	emit onNextPlaylist();
}
void EventHandler::PrevPlaylist()
{
	emit onPrevPlaylist();
}
void EventHandler::Backward()
{
	long long newPos = globals()->m_songPosition - globals()->m_Forward_Backward_Time;
	Seek((newPos > 0) ? newPos : 0);
}
void EventHandler::Forward()
{
	auto newPos = globals()->m_songPosition + globals()->m_Forward_Backward_Time;
	Seek((newPos <= globals()->m_metadata.Length) ? newPos : 0);
}
void EventHandler::VolumeUp()
{
	float newVolume = globals()->m_volume + 0.05f;
	VolumeChange(newVolume <= 1.0f ? newVolume : 1.0f);
}
void EventHandler::VolumeDown()
{
	float newVolume = globals()->m_volume - 0.05f;
	VolumeChange(newVolume >= 0 ? newVolume : 0);
}

void EventHandler::LineEditActivate()
{
	globals()->m_lineEditFocused++;
	emit onLineEditActivate();
}
void EventHandler::LineEditFinish()
{
	if(globals()->m_lineEditFocused != 0)
	{
		globals()->m_lineEditFocused--;
	}
	emit onLineEditFinish();
}
void EventHandler::FindActivate()
{
	emit onFindActivate();
}
void EventHandler::VolumeMute()
{
	if(globals()->m_volume != 0.0f)
	{
		no_mute_volume = globals()->m_volume;
		VolumeChange(0.0f);
	}
}
void EventHandler::VolumeUnmute()
{
	if(globals()->m_volume == 0.0f)
	{
		VolumeChange(no_mute_volume != 0.0f ? no_mute_volume : 0.5f);
		no_mute_volume = 0.0f;
	}
}
void EventHandler::VolumeMuteUnmute()
{
	if(globals()->m_isMuted)
	{
		VolumeUnmute();
	}
	else
	{
		VolumeMute();
	}
}
void EventHandler::LoopStateEnableDisable()
{
	if(globals()->m_loopState)
	{
		LoopStateChange(false);
	}
	else
	{
		LoopStateChange(true);
	}
}
void EventHandler::ShuffleStateEnableDisable()
{
	if(globals()->m_shuffleState)
	{
		ShuffleStateChange(false);
	}
	else
	{
		ShuffleStateChange(true);
	}
}

EventHandler *_eventHandler = nullptr;

void initEventHandler()
{
	if(!globals())
	{
		throw std::runtime_error("Error: Globals must be initialized before the EventHandler!");
	}
	_eventHandler = new EventHandler();
}
void deinitEventHandler()
{
	if(_eventHandler)
	{
		delete _eventHandler;
		_eventHandler = nullptr;
	}
}
EventHandler* eventHandler()
{
	return _eventHandler;
}
