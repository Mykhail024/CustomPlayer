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
		unsigned int no_mute_volume;

		QString m_displayFormat;

		Controls::TimeSlider *timeSlider;
		Controls::Slider *volumeSlider;
		QLabel *timeLabel;
		QPushButton *muteBtn;
		QPushButton *repeatBtn;
		QPushButton *shuffleBtn;

		QPushButton *prevBtn;
		QPushButton *playBtn;
		QPushButton *nextBtn;

};
