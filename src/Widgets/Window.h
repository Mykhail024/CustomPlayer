#pragma once

#include <QWidget>
#include <QGuiApplication>
#include <QScreen>
#include <QThread>
#include <QTimer>
#include <QUrl>

#include <string>

#include "ControlsPanel.h"
#include "InfoPanel.h"
#include "ListPanel.h"
#include "ListControlsPanel.h"
#include "../Audio/AudioServer.h"
#include "../TagReaders/TagReader.h"

#ifdef __linux__
#include <QDBusConnection>
#include <QDBusError>
#include "../DBus/MediaPlayer2.h"
#endif

static QRect getScreenGeometry()
{
	return QGuiApplication::primaryScreen()->geometry();
}

class Window : public QWidget
{
	Q_OBJECT
    public:
		enum PlayBackStatus
		{
			PLAYING,
			PAUSED,
			STOPPED
		};

        Window(Audio::AudioServer *server);
        ~Window();

	public slots:
		void Stop();
		void Pause();
		void Resume();
		void PlayPause();
		void Next();
		void Prev();
		void setRepeatState(bool state);
		void setVolume(int data);
		void goTo(int data);
		PlayBackStatus getPlaybackStatus();
		void setPlaybackStatus(PlayBackStatus status);
		float volume() const { return m_volume; }
		TagReaders::Data metadata() const { return m_metadata; }
		QUrl imageUrl() const { return m_ImageUrl; }
		int trackNumber() const { return list_panel->trackNumber(); }

	private slots:
		void Play(std::string path);
		void enableAutoSaveConfig();
		void disableAutoSaveConfig();
		void saveConfig();
		void openFolder();
		void resetControls();
		void loadMetadata(const std::string &path);
#ifdef __linux__
		void saveImage(const QImage &image);
#endif
		void updateCurrentTime(float time);
		void updateMaxTime(float time);

#ifdef __linux__
	signals:
		void emitDbusImageChanged(const QUrl &url);
#endif
	protected:
		void paintEvent(QPaintEvent *pa) override;

	private:
#ifdef __linux__
		void initDbus();
		MediaPlayer2 *dbus_MediaPlayer2;
		bool dbus_inited = false;
#endif
		float m_maxTime;
		bool m_is_playing;
		unsigned int autosaveInterval;
		float m_volume;
		PlayBackStatus m_playback_status;

		ControlsPanel *controls_panel;
		InfoPanel *info_panel;
		ListPanel *list_panel;
		ListControlsPanel *list_control_panel;

		Audio::AudioServer *audio_server;

		QTimer *ConfigAutoSaveTimer = nullptr;

		TagReaders::Data m_metadata;
		QUrl m_ImageUrl;
};

