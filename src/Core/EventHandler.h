#pragma once

#include <QObject>

class Globals;
class PlaylistModel;

class EventHandler : public QObject
{
	Q_OBJECT
	public:
		EventHandler();

	public slots:
		void PlaylistFind(const QString &text);
		void NextPlaylist();
		void PrevPlaylist();

		void LineEditActivate();
		void LineEditFinish();

		void FindActivate();

		void EndSong();
		void NextSong();
		void PrevSong();
		bool PlaySong(const QString&);
		void NextSongRandom();
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
		void FadeIn(const bool &isPrimary);
		void StopFadeIn();
		void Seek(const unsigned long int &time);
		void Rise();
		void PositionChange(const unsigned long int &pos);

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
		void onPositionChange(const unsigned long int &pos);
		void onRaise();
		void onSeek(const unsigned long int &time);
		void onStopFadeIn();
		void onFadeIn(const unsigned int &milliseconds);
		void onShuffleStateChange(const bool&);
		void onLoopStateChange(const bool&);
		void onVolumeChange(const float&);
		void onPlaybackStatusChanged();
		void onNextSongRandom();
		void onNextSong();
		void onPrevSong();
		bool onPlaySong(QString);
		void onEndSong();
		bool onPlay();
		bool onPause();
		void onStop();

	private:
		Globals *m_globals;
		float no_mute_volume;
};

void initEventHandler();
void deinitEventHandler();
EventHandler* eventHandler();
