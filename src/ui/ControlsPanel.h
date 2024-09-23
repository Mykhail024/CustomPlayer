#pragma once

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSlider>

#include "macros.h"

class ControlsPanel : public QWidget
{
    Q_OBJECT
    DEFAULT_WIDGET_PAINT_EVENT
    public:
        ControlsPanel(QWidget *parent);
        
    public slots:
        void onPlayPauseClicked(){}
        void onNextClicked(){}
        void onPrevClicked(){}

    private:
        QHBoxLayout *m_layout;

        QPushButton *m_prev;
        QPushButton *m_play_pause;
        QPushButton *m_next;

        QSlider *m_timeline;

        QPushButton *m_mute;
        QPushButton *m_loop;
        QPushButton *m_shuffle;
        QSlider *m_volume;
};
