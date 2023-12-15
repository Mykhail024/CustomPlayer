#pragma once

#include <QWidget>
#include <QGuiApplication>
#include <QScreen>
#include <QThread>
#include <QTimer>
#include <QUrl>

#include <string>

#include "ControlsPanel.h"

#ifdef __linux__
#include <QDBusConnection>
#include <QDBusError>
#endif

class PlaylistControl;
class SongsWidget;
class GlobalEventFilter;

class Window : public QWidget
{
	Q_OBJECT
    public:
        Window();
        ~Window();

	public slots:

	private slots:

	protected:
		void paintEvent(QPaintEvent *pa) override;

	private:
		ControlsPanel *m_controlsPanel;
		PlaylistControl *m_playlistControl;
		SongsWidget *m_songsWidget;

};

