#pragma once

#include <QObject>

#include "Globals.h"

struct COLLUMNS;
struct SONG_METADATA;

class EventHandler : public QObject
{
    Q_OBJECT
    public:

    public slots:
        void PlaylistFind(const QString &text);
        void FindClear();
        void NextPlaylist();
        void PrevPlaylist();

        void FindActivate();

        void EndSong();
        void NextSong();
        void PrevSong();
        bool PlayFile(const QString &filePath);
        bool PlayPlaylistEntry(const QPersistentModelIndex &index, const bool &addToHistory);
        bool Play();
        bool Pause();
        void Stop();
        void PlayPause();
        void Backward();
        void Forward();

        void VolumeUp();
        void VolumeDown();
        void VolumeMute();
        void VolumeUnmute();
        void VolumeMuteUnmute();
        void VolumeChange(const float&);

        void LoopStateChange(const bool&);
        void LoopStateEnableDisable();

        void ShuffleStateChange(const bool&);
        void ShuffleStateEnableDisable();

        void CanPlayChange(const bool &canPlay);
        void CanNextChange(const bool &canNext);
        void CanPrevChange(const bool &canPrev);
        void PlaybackStateChange(const PLAYBACK_STATE &state);

        void FadeIn(const bool &isPrimary);
        void StopFadeIn();
        void Seek(const unsigned long int &time);
        void Rise();
        void PositionChange(const unsigned long int &pos);
        /** Set visible columns in playlist*/
        void ColumnsChange(const COLLUMNS &columns);
        void SoftwareVolumeControlChange(const bool &enable);
        void HistoryCapacityChange(const unsigned int &size);
        /**Set Forward/Backward time*/
        void ForwardTimeChange(const unsigned int &time);
        /**Set -1 to unchange*/
        void FadeInTimeChange(const int &primary_time, const int &secondary_time);

    signals:
        void onAddPlaylist();
        void onRemovePlaylist();
        void onRenamePlaylist(const int &index, const QString &name);
        void onFindActivate();
        void onLineEditActivate();
        void onLineEditFinish();
        void onNextPlaylist();
        void onPrevPlaylist();
        void onPlaylistFind(const QString &text);
        void onFindClear();
        void onPositionChange(const unsigned long int &pos);
        void onRaise();
        void onSeek(const unsigned long int &time);
        void onStopFadeIn();
        void onFadeIn(const unsigned int &milliseconds);
        void onShuffleStateChange(const bool&);
        void onLoopStateChange(const bool&);
        void onVolumeChange(const float&);
        void onPlaybackStateChanged(const PLAYBACK_STATE &state);
        void onNextSong();
        void onPrevSong();
        bool onPlaySong(const SONG_METADATA &metadata);
        void onEndSong();
        bool onPlay();
        bool onPause();
        void onStop();
        void onCanPlayChanged(const bool &canPlay);
        void onCanNextChanged(const bool &canNext);
        void onCanPrevChanged(const bool &canPrev);

        void onColumnsChanged(const COLLUMNS &columns);
        void onSoftwareVolumeControlChanged(const bool &enable);
        void onHistoryCapacityChanged(const unsigned int &size);
        void onForwardTimeChanged(const unsigned int &time);
        void onFadeInTimeChanged(const int &primary_time, const int &secondary_time);

    private:
        friend EventHandler& eventHandler();
        EventHandler();
        Globals *m_globals;
        float no_mute_volume;
};

EventHandler& eventHandler();
