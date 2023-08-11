#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <qwidget.h>

#include "../Convert.cpp"

#include "ControlsPanel.h"

ControlsPanel::ControlsPanel(QWidget *parent) : QWidget(parent)
{
	this->setObjectName("ControlsPanel");

	auto *layout = new QHBoxLayout(this);

	timeSlider = new Widgets::TimeSlider();
	timeSlider->setOrientation(Qt::Horizontal);
	timeSlider->setObjectName("TimeSlider");
	timeSlider->setMinimum(0);
	timeSlider->setDisabled(true);

	timeLabel = new QLabel();
	timeLabel->setText("0:00/0:00");
	timeLabel->setObjectName("TimeLabel");
	timeLabel->setDisabled(true);

	const int buttonSize = this->width() * .060;
	const int iconSize = buttonSize * 0.8;

	muteBtn = new QPushButton();
	muteBtn->setObjectName("MuteButton");
	muteBtn->setCheckable(true);
	muteBtn->setFixedSize(buttonSize * 0.7, buttonSize * 0.7);
	muteBtn->setIconSize(QSize(iconSize * 0.7, iconSize * 0.7));

	volumeSlider = new QSlider(Qt::Horizontal);
	volumeSlider->setObjectName("VolumeSlider");
	volumeSlider->setMinimum(0);
	volumeSlider->setMaximum(100);

	prevBtn = new QPushButton();
	prevBtn->setObjectName("PrevButton");
	prevBtn->setFixedSize(buttonSize, buttonSize);
	prevBtn->setIconSize(QSize(iconSize, iconSize));
	prevBtn->setDisabled(true);

	playBtn = new QPushButton();
	playBtn->setObjectName("PlayButton");
	playBtn->setFixedSize(buttonSize, buttonSize);
	playBtn->setIconSize(QSize(iconSize, iconSize));
	playBtn->setCheckable(true);

	nextBtn = new QPushButton();
	nextBtn->setObjectName("NextButton");
	nextBtn->setFixedSize(buttonSize, buttonSize);
	nextBtn->setIconSize(QSize(iconSize, iconSize));
	nextBtn->setDisabled(true);

	layout->addWidget(prevBtn);
	layout->addWidget(playBtn);
	layout->addWidget(nextBtn);
	layout->addWidget(timeSlider);
	layout->addWidget(timeLabel);
	layout->addWidget(muteBtn);
	layout->addWidget(volumeSlider, 0, Qt::AlignLeft);

	layout->setSpacing(0);
	this->setLayout(layout);

	//Input Signals
	connect(this, &ControlsPanel::updateTime, &ControlsPanel::onUpdateTimeSlider);
	connect(this, &ControlsPanel::updateTime, &ControlsPanel::onUpdateTimeLabel);

	connect(this, &ControlsPanel::updateMaxTime, &ControlsPanel::onUpdateMaxTime);

	connect(this, &ControlsPanel::setPlayButtonChecked, &ControlsPanel::onSetPlayButtonChecked);

	connect(this, &ControlsPanel::setPreviousButtonEnabled, &ControlsPanel::onSetPreviousButtonEnabled);
	connect(this, &ControlsPanel::setPlayButtonEnabled, &ControlsPanel::onSetPlayButtonEnabled);
	connect(this, &ControlsPanel::setNextButtonEnabled, &ControlsPanel::onSetNextButtonEnabled);

	connect(this, &ControlsPanel::setVolumeSliderEnabled, &ControlsPanel::onSetVolumeSliderEnabled);
	connect(this, &ControlsPanel::setTimeSliderEnabled, &ControlsPanel::onSetTimeSliderEnabled);

	connect(this, &ControlsPanel::setVolume, &ControlsPanel::onSetVolume);

	//Output signals
	connect(timeSlider, &QSlider::valueChanged, this, &ControlsPanel::onTimeSliderValueChanged);
	connect(volumeSlider, &QSlider::valueChanged, this, &ControlsPanel::onVolumeSliderValueChanged);

	connect(nextBtn, &QPushButton::clicked, this, &ControlsPanel::onNextButtonClicked);
	connect(playBtn, &QPushButton::clicked, this, &ControlsPanel::onPlayButtonClicked);
	connect(prevBtn, &QPushButton::clicked, this, &ControlsPanel::onPreviousButtonClicked);

	connect(muteBtn, &QPushButton::clicked, this, &ControlsPanel::onMuteButtonClicked);

	volumeSlider->setValue(100);
}
ControlsPanel::~ControlsPanel()
{

}

void ControlsPanel::setEnabled(bool state)
{
	timeSlider->setEnabled(state);
	timeLabel->setEnabled(state);
}

void ControlsPanel::onUpdateTimeLabel(int data)
{
	timeLabel->setText(Convert::Seconds::toMinutes(data / 1000) + "/" + Convert::Seconds::toMinutes(m_length / 1000));
}

void ControlsPanel::onUpdateMaxTime(int data)
{
	m_length = data;
	timeSlider->setMaximum(m_length);
}

void ControlsPanel::onTimeSliderValueChanged(int data) { emit timeSliderValueChanged(data); }
void ControlsPanel::onVolumeSliderValueChanged(int data)
{
	QString iconPath;
	if (data == 0)
	{
		iconPath = ":/Icons/volume-mute.svg";
	}
	else if (data <= 33)
	{
		iconPath = ":/Icons/volume-low.svg";
	}
	else if (data <= 66)
	{
		iconPath = ":/Icons/volume-medium.svg";
	}
	else
	{
		iconPath = ":/Icons/volume-high.svg";
	}

	muteBtn->setStyleSheet(QString("#MuteButton { icon: url(%1); }"
								   "#MuteButton:hover { icon: url(%2)}")
								   .arg(iconPath)
								   .arg(iconPath.replace(".svg", "_hover.svg")));
	emit volumeSliderValueChanged(data);
}

void ControlsPanel::onPreviousButtonClicked() { emit previousButtonClick(); }
void ControlsPanel::onPlayButtonClicked(bool state) { emit playButtonChecked(state); }
void ControlsPanel::onNextButtonClicked() { emit nextButtonClick(); }

void ControlsPanel::onMuteButtonClicked(bool state)
{
	if(state == true)
	{
		no_mute_volume = volumeSlider->value();
		volumeSlider->setValue(0);
	}
	else
	{
		volumeSlider->setValue(no_mute_volume);
	}
	emit muteButtonChecked(state);
}

void ControlsPanel::paintEvent(QPaintEvent *pe)
{
	QStyleOption o;
	o.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
};
