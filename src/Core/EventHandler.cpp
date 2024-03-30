#include <stdexcept>

#include "Audio/AudioServer.h"

#include "Playlist/PlaylistModel.h"

#include "TagReaders/TagReader.h"

#include "Core/Globals.h"
#include "Core/Log.h"

#include "Core/Playlist/History.h"

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

    connect(this, &EventHandler::onPlay, globals()->audioServer(), &Audio::AudioServer::resume);
    connect(this, &EventHandler::onPause, globals()->audioServer(), &Audio::AudioServer::pause);
    connect(this, &EventHandler::onStop, globals()->audioServer(), &Audio::AudioServer::stop);

    connect(this, &EventHandler::onSeek, globals()->audioServer(), &Audio::AudioServer::goTo);
}

void EventHandler::EndSong()
{
    CanPlayChange(false);
    PlaybackStateChange(STOPPED);
    /*
    if(!globals()->m_loopState) {
        NextSong();
    }*/
    emit onEndSong();
}

void EventHandler::PlayPause()
{
    if(globals()->m_playbackState == PLAYING) {
        Pause();
    } else {
        Play();
    }
}

void EventHandler::NextSong()
{
    if(globals()->m_canNext) {
        if(globals()->history()->hasForward()) {
            PlayPlaylistEntry(globals()->history()->goForward(), false);
        } else {
            const PlaylistModel *model = qobject_cast<const PlaylistModel*>(globals()->history()->entry().model());
            if(globals()->m_shuffleState) {
                emit model->onSelectRandom();
            } else {
                emit model->onSelectNext();
            }
        }
        emit onNextSong();
    }
    CanPrevChange(globals()->history()->hasBack());
}

void EventHandler::PrevSong()
{
    if(globals()->m_canPrev) {
        auto history = globals()->history();
        PlayPlaylistEntry(history->goBack(), false);
        emit onPrevSong();
    }
}

bool EventHandler::PlayFile(const QString &filePath)
{
    FadeIn(true);
    bool ok = globals()->audioServer()->play(filePath);

    if(ok) {
        auto metadata = TagReaders::id3v2_read(filePath);
        metadata.ArtUrl = TagReaders::id3v2_get_image_path(filePath);
        globals()->m_currentSong = metadata;

        CanPlayChange(true);
        CanNextChange(true);
        PlaybackStateChange(PLAYING);
        emit onPlaySong(metadata);
    }
    return ok;
}

bool EventHandler::PlayPlaylistEntry(const QPersistentModelIndex &index, const bool &addToHistory)
{
    if(!index.isValid()) return false;

    const PlaylistModel *model = qobject_cast<const PlaylistModel*>(index.model());

    if(!model) return false;

    bool ok = PlayFile(model->getSongMetadata(index.row()).Path);
    if(ok) {
        model->onUpdateSelection(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        auto history = globals()->history();
        if(addToHistory) {
            if(history->size()-1 > history->currentIndex()) {
                history->clearFuture();
            }
            history->addEntry(index);
        }
        CanPrevChange(history->hasBack());
    }

    return ok;
}

bool EventHandler::Play()
{
    FadeIn(false);
    bool ok = onPlay();
    if(ok) {
        globals()->m_canPlay = true;
        PlaybackStateChange(PLAYING);
    }
    return ok;
}

bool EventHandler::Pause()
{
    bool ok = onPause();
    if(ok) {
        globals()->m_canPlay = true;
        PlaybackStateChange(PAUSED);
    }
    return ok;
}

void EventHandler::Stop()
{
    emit onStop();
    CanPlayChange(false);
    PlaybackStateChange(STOPPED);
}

void EventHandler::VolumeChange(const float& volume)
{
    StopFadeIn();
    globals()->m_volume = volume;
    emit onVolumeChange(volume);
    if(volume == 0.0f) {
        globals()->m_isMuted = true;
    } else {
        globals()->m_isMuted = false;
    }
}

void EventHandler::ColumnsChange(const COLLUMNS &columns)
{
    globals()->m_columns = columns;
    emit onColumnsChanged(columns);
}

void EventHandler::SoftwareVolumeControlChange(const bool &enable)
{
    globals()->m_softwareVolumeControl = enable;
    emit onSoftwareVolumeControlChanged(enable);
}

void EventHandler::HistoryCapacityChange(const unsigned int &size)
{
    globals()->m_historyCapacity = size;
    emit onHistoryCapacityChanged(size);
}

void EventHandler::FindClear()
{
    eventHandler().onFindClear();
}

void EventHandler::ForwardTimeChange(const unsigned int &time)
{
    globals()->m_Forward_Backward_Time = time;
    emit onForwardTimeChanged(time);
}

void EventHandler::FadeInTimeChange(const int &primary_time, const int &secondary_time)
{
    if(primary_time >= 0) {
        globals()->m_fadeInTime_Primary = primary_time;
    }
    if(secondary_time >= 0) {
        globals()->m_fadeInTime_Secondary = secondary_time;
    }
    emit onFadeInTimeChanged(globals()->m_fadeInTime_Primary, globals()->m_fadeInTime_Secondary);
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

void EventHandler::CanPlayChange(const bool &canPlay)
{
    globals()->m_canPlay = canPlay;
    emit onCanPlayChanged(canPlay);
}

void EventHandler::CanNextChange(const bool &canNext)
{
    globals()->m_canNext = canNext;
    emit onCanNextChanged(canNext);
}

void EventHandler::CanPrevChange(const bool &canPrev)
{
    globals()->m_canPrev = canPrev;
    emit onCanPrevChanged(canPrev);
}

void EventHandler::PlaybackStateChange(const PLAYBACK_STATE &state)
{
    globals()->m_playbackState = state;
    emit onPlaybackStateChanged(state);
}

void EventHandler::PositionChange(const unsigned long int &pos)
{
    globals()->m_songPosition = pos;
    emit onPositionChange(pos);
}

void EventHandler::FadeIn(const bool &isPrimary)
{
    StopFadeIn();
    if(isPrimary) {
        emit onFadeIn(globals()->m_fadeInTime_Primary);
    } else {
        emit onFadeIn(globals()->m_fadeInTime_Secondary);
    }
}

void EventHandler::StopFadeIn()
{
    emit onStopFadeIn();
}

void EventHandler::Seek(const unsigned long int &time)
{
    if(globals()->m_playbackState == STOPPED) return;

    FadeIn(false);
    emit onSeek(time);
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
    Seek((newPos <= globals()->m_currentSong.Length) ? newPos : 0);
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

void EventHandler::FindActivate()
{
    emit onFindActivate();
}

void EventHandler::VolumeMute()
{
    if(globals()->m_volume != 0.0f) {
        no_mute_volume = globals()->m_volume;
        VolumeChange(0.0f);
    }
}

void EventHandler::VolumeUnmute()
{
    if(globals()->m_volume == 0.0f) {
        VolumeChange(no_mute_volume != 0.0f ? no_mute_volume : 0.5f);
        no_mute_volume = 0.0f;
    }
}

void EventHandler::VolumeMuteUnmute()
{
    if(globals()->m_isMuted) {
        VolumeUnmute();
    } else {
        VolumeMute();
    }
}

void EventHandler::LoopStateEnableDisable()
{
    if(globals()->m_loopState) {
        LoopStateChange(false);
    } else {
        LoopStateChange(true);
    }
}

void EventHandler::ShuffleStateEnableDisable()
{
    if(globals()->m_shuffleState) {
        ShuffleStateChange(false);
    } else {
        ShuffleStateChange(true);
    }
}

EventHandler& eventHandler()
{
    static EventHandler INSTANCE;
    return INSTANCE;
}
