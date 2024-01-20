#pragma once

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>

namespace Controls{
    class TimeSlider;
    class Slider;
}

class ControlsPanel : public QWidget
{
    Q_OBJECT
    public:
        ControlsPanel(QWidget *parent = nullptr);
        ~ControlsPanel();

    private slots:
        void updateTime(const int &time);
        void updateVolumeIcon(const int &data);
        void reset();
    signals:

    protected:
        void paintEvent(QPaintEvent *pe) override;

    private:
        unsigned int m_length;
        unsigned int m_no_mute_volume;

        QString m_displayFormat;

        Controls::TimeSlider *m_timeSlider;
        Controls::Slider *m_volumeSlider;
        QLabel *m_timeLabel;
        QPushButton *m_muteBtn;
        QPushButton *m_repeatBtn;
        QPushButton *m_shuffleBtn;

        QPushButton *m_prevBtn;
        QPushButton *m_playBtn;
        QPushButton *m_nextBtn;

};
