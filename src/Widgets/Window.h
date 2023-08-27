#pragma once

#include <QWidget>
#include <QGuiApplication>
#include <QScreen>
#include <qthread.h>
#include <qtimer.h>
#include <string>

#include "ControlsPanel.h"
#include "InfoPanel.h"
#include "ListPanel.h"
#include "ListControlsPanel.h"
#include "../Audio/AudioServer.h"

static QRect getScreenGeometry()
{
	return QGuiApplication::primaryScreen()->geometry();
}

class Window : public QWidget
{
	Q_OBJECT
    public:
        Window(Audio::AudioServer *server);
        ~Window();

	private slots:
		void Play(std::string path);
		void Pause();
		void Resume();
		void PlayPause();
		void Next();
		void Prev();
		void setVolume(int data);
		void goTo(int data);
		void enableAutoSaveConfig();
		void disableAutoSaveConfig();
		void saveConfig();
		void openFolder();

		void updateCurrentTime(float time);
		void updateMaxTime(float time);

	protected:
		void paintEvent(QPaintEvent *pa) override;

	private:
		float m_maxTime;
		bool m_is_playing;
		unsigned int autosaveInterval;

		ControlsPanel *controls_panel;
		InfoPanel *info_panel;
		ListPanel *list_panel;
		ListControlsPanel *list_control_panel;

		Audio::AudioServer *audio_server;

		QTimer *ConfigAutoSaveTimer = nullptr;
};

