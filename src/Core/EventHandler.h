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
		void emitPlaylistFind(const QString &text);
		void emitNextPlaylist();
		void emitPrevPlaylist();

		void emitAddPlaylists();

		void emitFindActivate();
		void emitFindFinish();

		void emitEndSong();
		void emitNextSong();
		void emitPrevSong();
		bool emitPlaySong(const QString&);
		void emitNextSongRandom();
		bool emitPlay();
		bool emitPause();
		void emitStop();
		void emitPlayPause();
		void emitBackward();
		void emitForward();

		void emitVolumeUp();
		void emitVolumeDown();
		void emitVolumeMute();
		void emitVolumeUnmute();
		void emitVolumeMuteUnmute();
		void emitVolumeChange(const float&);

		void emitLoopStateChange(const bool&);
		void emitLoopStateEnableDisable();
		void emitShuffleStateChange(const bool&);
		void emitShuffleStateEnableDisable();
		void emitFadeIn(const bool &isPrimary);
		void emitStopFadeIn();
		void emitSeek(const unsigned long int &time);
		void emitRise();
		void emitPositionChange(const unsigned long int &pos);

	signals:
		void onAddPlaylists();
		void onFindActivate();
		void onFindFinish();
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
