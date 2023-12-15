
#include <stdexcept>
#include <QDebug>

#include "../Audio/AudioServer.h"
#include "Globals.h"
#include "../TagReaders/TagReader.h"
#include "PlaylistModel.h"

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
void EventHandler::emitEndSong()
{
	emit onEndSong();
	globals()->m_playbackStatus.state = STOPPED;
	emit onPlaybackStatusChanged();
	if(!globals()->m_loopState)
	{
		if(globals()->m_shuffleState)
		{
			emitNextSongRandom();
		}
		else
		{
			emitNextSong();
		}
	}
}
void EventHandler::emitPlayPause()
{
	if(globals()->m_playbackStatus.state == PLAYING)
	{
		emitPause();
	}
	else
	{
		emitPlay();
	}
}
void EventHandler::emitNextSong()
{
	if(globals()->m_playbackStatus.canNext)
	{
		if(globals()->m_shuffleState)
		{
			emitNextSongRandom();
		}
		else
		{
			emit onNextSong();
		}
	}
}
void EventHandler::emitPrevSong()
{
	if(globals()->m_playbackStatus.canPrev)
	{
		emit onPrevSong();
	}
}
bool EventHandler::emitPlaySong(const QString &path)
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
void EventHandler::emitNextSongRandom()
{
	emit onNextSongRandom();
}
bool EventHandler::emitPlay()
{
	emitFadeIn(false);
	bool ok = onPlay();
	if(ok)
	{
		globals()->m_playbackStatus.state = PLAYING;
		globals()->m_playbackStatus.canPlay = true;
		emit onPlaybackStatusChanged();
	}

	return ok;
}
bool EventHandler::emitPause()
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
void EventHandler::emitStop()
{
	emit onStop();
	globals()->m_playbackStatus.state = STOPPED;
	globals()->m_playbackStatus.canPlay = false;
	emit onPlaybackStatusChanged();
}
void EventHandler::emitVolumeChange(const float& volume)
{
	emitStopFadeIn();
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
void EventHandler::emitLoopStateChange(const bool &state)
{
	globals()->m_loopState = state;
	emit onLoopStateChange(state);
}
void EventHandler::emitShuffleStateChange(const bool &state)
{
	globals()->m_shuffleState = state;
	emit onShuffleStateChange(state);
}
void EventHandler::emitPositionChange(const unsigned long int &pos)
{
	globals()->m_songPosition = pos;
	emit onPositionChange(pos);
}
void EventHandler::emitFadeIn(const bool &isPrimary)
{
	emitStopFadeIn();
	if(isPrimary)
	{
		emit onFadeIn(globals()->m_fadeInTime_Primary);
	}
	else
	{
		emit onFadeIn(globals()->m_fadeInTime_Secondary);
	}
}
void EventHandler::emitStopFadeIn()
{
	emit onStopFadeIn();
}
void EventHandler::emitSeek(const unsigned long int &time)
{
	if(globals()->playbackStatus().state == STOPPED) return;

	emitFadeIn(false);
	emit onSeek(time);
	globals()->m_songPosition = time;
}
void EventHandler::emitRise()
{

}
void EventHandler::emitPlaylistFind(const QString &text)
{
	emit onPlaylistFind(text);
}
void EventHandler::emitNextPlaylist()
{
	emit onNextPlaylist();
}
void EventHandler::emitPrevPlaylist()
{
	emit onPrevPlaylist();
}
void EventHandler::emitBackward()
{
	long long newPos = globals()->m_songPosition - globals()->m_Forward_Backward_Time;
	emitSeek((newPos > 0) ? newPos : 0);
}
void EventHandler::emitForward()
{
	auto newPos = globals()->m_songPosition + globals()->m_Forward_Backward_Time;
	emitSeek((newPos <= globals()->m_metadata.Length) ? newPos : 0);
}
void EventHandler::emitVolumeUp()
{
	float newVolume = globals()->m_volume + 0.05f;
	emitVolumeChange(newVolume <= 1.0f ? newVolume : 1.0f);
}
void EventHandler::emitVolumeDown()
{
	float newVolume = globals()->m_volume - 0.05f;
	emitVolumeChange(newVolume >= 0 ? newVolume : 0);
}
void EventHandler::emitFindActivate()
{
	globals()->m_findIsFocused = true;
	emit onFindActivate();
}
void EventHandler::emitFindFinish()
{
	globals()->m_findIsFocused = false;
	emit onFindFinish();
}
void EventHandler::emitAddPlaylists()
{
	emit onAddPlaylists();
}
void EventHandler::emitVolumeMute()
{
	if(globals()->m_volume != 0.0f)
	{
		no_mute_volume = globals()->m_volume;
		emitVolumeChange(0.0f);
	}
}
void EventHandler::emitVolumeUnmute()
{
	if(globals()->m_volume == 0.0f)
	{
		emitVolumeChange(no_mute_volume != 0.0f ? no_mute_volume : 0.5f);
		no_mute_volume = 0.0f;
	}
}
void EventHandler::emitVolumeMuteUnmute()
{
	if(globals()->m_isMuted)
	{
		emitVolumeUnmute();
	}
	else
	{
		emitVolumeMute();
	}
}
void EventHandler::emitLoopStateEnableDisable()
{
	if(globals()->m_loopState)
	{
		emitLoopStateChange(false);
	}
	else
	{
		emitLoopStateChange(true);
	}
}
void EventHandler::emitShuffleStateEnableDisable()
{
	if(globals()->m_shuffleState)
	{
		emitShuffleStateChange(false);
	}
	else
	{
		emitShuffleStateChange(true);
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
