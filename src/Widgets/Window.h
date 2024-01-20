#pragma once

#include <QWidget>
#include <QGuiApplication>
#include <QScreen>
#include <QThread>
#include <QTimer>
#include <QUrl>

#include <string>

#include "ControlsPanel.h"

class PlaylistControl;
class SongsWidget;
class GlobalEventFilter;

class Window : public QWidget
{
    public:
        Window();
        ~Window();

    protected:
        void paintEvent(QPaintEvent *pa) override;

    private:
        ControlsPanel *m_controlsPanel;
        PlaylistControl *m_playlistControl;
        SongsWidget *m_songsWidget;
};

