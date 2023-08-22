#pragma once

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>

#include "Controls/TimeSlider.h"

class ControlsPanel : public QWidget
{
	Q_OBJECT
	public:
		ControlsPanel(QWidget *parent = nullptr);
		~ControlsPanel();
		void setEnabled(bool state);


	private slots:
		void onUpdateTimeSlider(int data) { timeSlider->setValueDontMove(data); }
		void onUpdateTimeLabel(int data);
		void onUpdateMaxTime(int data);

		void onTimeSliderValueChanged();
		void onVolumeSliderValueChanged(int data);

		void onPreviousButtonClicked();
		void onPlayButtonClicked(bool state);
		void onNextButtonClicked();

		void onMuteButtonClicked(bool state);

		void onSetPlayButtonChecked(bool state) { playBtn->setChecked(state); }

		void onSetPreviousButtonEnabled(bool state) { prevBtn->setEnabled(state); }
		void onSetPlayButtonEnabled(bool state) { playBtn->setEnabled(state); }
		void onSetNextButtonEnabled(bool state) { nextBtn->setEnabled(state); }

		void onSetVolumeSliderEnabled(bool state) { volumeSlider->setEnabled(state); }
		void onSetTimeSliderEnabled(bool state) { timeSlider->setEnabled(state); }

		void onSetVolume(int data) { volumeSlider->setValue(data); }

	signals:
		//Input signals
		void updateTime(int data);
		void updateMaxTime(int data);

		void setVolume(int data);

		void setPlayButtonChecked(bool state);

		void setPreviousButtonEnabled(bool state);
		void setPlayButtonEnabled(bool state);
		void setNextButtonEnabled(bool state);

		void setVolumeSliderEnabled(bool state);
		void setTimeSliderEnabled(bool state);

		//Output signals
		void timeSliderValueChanged(int data);
		void volumeSliderValueChanged(int data);

		void previousButtonClick();
		void playButtonChecked(bool state);
		void nextButtonClick();

		void muteButtonChecked(bool state);

	protected:
		void paintEvent(QPaintEvent *pe) override;

	private:
		int m_length;
		int no_mute_volume;

		Widgets::TimeSlider *timeSlider;
		QLabel *timeLabel;
		QPushButton *muteBtn;
		QSlider *volumeSlider;

		QPushButton *prevBtn;
		QPushButton *playBtn;
		QPushButton *nextBtn;

};
