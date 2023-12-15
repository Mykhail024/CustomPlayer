#pragma once

#include <QTimer>
#include <QObject>
#include <QVariantMap>

#include "../Config/Config.h"

namespace Audio {
	class AudioServer;
}

enum PLAYBACK_STATE
{
	PLAYING,
	PAUSED,
	STOPPED
};
struct PLAYBACK_STATUS
{
	PLAYBACK_STATE state = PLAYBACK_STATE::STOPPED;
	bool canNext = false;
	bool canPrev = false;
	bool canPlay = false;
};

struct SONG_METADATA
{
	QString Title;
	QString Artist;
	QString Album;
	QString ArtUrl;
	unsigned long int Length;
	unsigned long int ModifiedDate;
	unsigned long int Year;
	QString Path;
	uint32_t SampleRate;
	unsigned int Channels;
	unsigned int BitRate;
};

class Globals : public QObject
{
	Q_OBJECT
	public:
		Globals();
		~Globals()
		{
			deinitAudioServer();
			m_saveTimer->deleteLater();
		}
		void initAudioServer();
		void deinitAudioServer();

	public slots:
		void startVolumeSaveTimer();
		void save();

		void setColumns(const Config::COLLUMNS &columns);
		void setSoftwareVolumeControl(const bool &enabled);
		void setFadeIn_Primary(const int &time);
		void setFadeIn_Secondary(const int &time);
		void setForward_Backward_Time(const unsigned int &time);

		PLAYBACK_STATUS playbackStatus() const { return m_playbackStatus; }
		bool loopState() const { return m_loopState; }
		bool shuffleState() const { return m_shuffleState; }
		float volume() const { return m_volume; }
		Config::COLLUMNS columns() const { return m_columns; }
		bool softwareVolumeControl() const { return m_softwareVolumeControl; }
		int fadeIn_Primary() const { return m_fadeInTime_Primary; }
		int fadeIn_Secondary() const { return m_fadeInTime_Secondary; }
		unsigned int getForward_Backward_Time() const { return m_Forward_Backward_Time; }
		bool isMuted() const { return m_isMuted; }

		Audio::AudioServer* audioServer() { return m_audioServer; }

		unsigned long int songPosition() const { return m_songPosition; };

		SONG_METADATA metadata() const { return m_metadata; }

		bool findIsFocused() const { return m_findIsFocused; }

	private:
		friend class EventHandler;

		Audio::AudioServer *m_audioServer = nullptr;

		bool m_findIsFocused = false;
		unsigned int m_Forward_Backward_Time;
		PLAYBACK_STATUS m_playbackStatus;
		bool m_loopState;
		bool m_shuffleState;
		bool m_isMuted = false;
		float m_volume;
		Config::COLLUMNS m_columns;
		bool m_softwareVolumeControl;
		int m_fadeInTime_Primary;
		int m_fadeInTime_Secondary;

		unsigned long int m_songPosition;
		SONG_METADATA m_metadata;

		QTimer *m_saveTimer;
};

void initGlobals();
void deinitGlobals();
Globals* globals();
