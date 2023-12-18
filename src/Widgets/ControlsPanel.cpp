#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <qwidget.h>

#include "../Convert.cpp"

#include "Controls/TimeSlider.h"
#include "Controls/Slider.h"
#include "../Core/EventHandler.h"
#include "../Core/Globals.h"

#include "ControlsPanel.h"

#include "moc_ControlsPanel.cpp"

ControlsPanel::ControlsPanel(QWidget *parent) : QWidget(parent)
{
	this->setObjectName("ControlsPanel");

	auto *layout = new QHBoxLayout(this);

	timeSlider = new Controls::TimeSlider();
	timeSlider->setOrientation(Qt::Horizontal);
	timeSlider->setObjectName("TimeSlider");
	timeSlider->setMinimum(0);
	timeSlider->setDisabled(true);

	timeLabel = new QLabel();
	timeLabel->setText("0:00/0:00");
	timeLabel->setObjectName("TimeLabel");
	timeLabel->setDisabled(true);

	const int buttonSize = this->width() * 0.06;
	const int iconSize = buttonSize * 0.8;

	muteBtn = new QPushButton();
	muteBtn->setObjectName("MuteButton");
	muteBtn->setCheckable(true);
	muteBtn->setFixedSize(buttonSize * 0.7, buttonSize * 0.7);
	muteBtn->setIconSize(QSize(iconSize * 0.7, iconSize * 0.7));

	volumeSlider = new Controls::Slider(Qt::Horizontal);
	volumeSlider->setObjectName("VolumeSlider");
	volumeSlider->setMinimum(0);
	volumeSlider->setMaximum(100);
	float volume = globals()->volume() * 100;
	volumeSlider->setValue(volume);
	updateVolumeIcon(volume);

	repeatBtn = new QPushButton();
	repeatBtn->setObjectName("RepeatButton");
	repeatBtn->setCheckable(true);
	repeatBtn->setChecked(globals()->loopState());
	repeatBtn->setFixedSize(buttonSize * 0.7, buttonSize * 0.7);
	repeatBtn->setIconSize(QSize(iconSize * 0.7, iconSize * 0.7));

	shuffleBtn = new QPushButton();
	shuffleBtn->setObjectName("ShuffleButton");
	shuffleBtn->setCheckable(true);
	shuffleBtn->setChecked(globals()->shuffleState());
	shuffleBtn->setFixedSize(buttonSize * 0.5, buttonSize * 0.5);
	shuffleBtn->setIconSize(QSize(iconSize * 0.5, iconSize * 0.5));

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
	playBtn->setEnabled(false);

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
	layout->addWidget(repeatBtn);
	layout->addWidget(shuffleBtn);
	layout->addWidget(muteBtn);
	layout->addWidget(volumeSlider, 0, Qt::AlignLeft);

	layout->setSpacing(0);

	connect(eventHandler(), &EventHandler::onPlaySong, this, [&]{
				if(globals()->playbackStatus().state != PLAYBACK_STATE::STOPPED)
				{
					m_length = globals()->metadata().Length / 1e3;
					timeSlider->setMaximum(m_length);
					timeSlider->setEnabled(true);
				}
			});
	connect(eventHandler(), &EventHandler::onPlaybackStatusChanged, this, [&]{
				auto status = globals()->playbackStatus();
				prevBtn->setEnabled(status.canPrev);
				playBtn->setEnabled(status.canPlay);
				nextBtn->setEnabled(status.canNext);
				playBtn->setChecked((status.state == PLAYBACK_STATE::PLAYING ? true : false));
			});
	connect(eventHandler(), &EventHandler::onStop, this, &ControlsPanel::reset);
	connect(eventHandler(), &EventHandler::onEndSong, this, &ControlsPanel::reset);
	connect(eventHandler(), &EventHandler::onPositionChange, this, &ControlsPanel::updateTime);
	connect(eventHandler(), &EventHandler::onSeek, this, &ControlsPanel::updateTime);
	connect(timeSlider, &Controls::TimeSlider::sliderReleased, [&]{
				eventHandler()->Seek(timeSlider->value() * 1e3);
			});
	connect(volumeSlider, &Controls::Slider::valueChanged, this, [&](int value){
				eventHandler()->VolumeChange((float)value / 100.0f);
				updateVolumeIcon(value);
			});
	connect(muteBtn, &QPushButton::clicked, eventHandler(), &EventHandler::VolumeMuteUnmute);
	connect(eventHandler(), &EventHandler::onVolumeChange, this, [&](const float &volume){
				volumeSlider->setValue(volume * 100);
			});
	connect(repeatBtn, &QPushButton::clicked, eventHandler(), &EventHandler::LoopStateChange);
	connect(eventHandler(), &EventHandler::onLoopStateChange, repeatBtn, &QPushButton::setChecked);
	connect(shuffleBtn, &QPushButton::clicked, eventHandler(), &EventHandler::ShuffleStateChange);
	connect(eventHandler(), &EventHandler::onShuffleStateChange, shuffleBtn, &QPushButton::setChecked);

	connect(playBtn, &QPushButton::clicked, eventHandler(), &EventHandler::PlayPause);
	connect(nextBtn, &QPushButton::clicked, eventHandler(), &EventHandler::NextSong);
	connect(prevBtn, &QPushButton::clicked, eventHandler(), &EventHandler::PrevSong);
}
ControlsPanel::~ControlsPanel()
{

}

void ControlsPanel::reset()
{
	timeSlider->setEnabled(false);
	playBtn->setEnabled(false);
	timeLabel->setText("0:00/0:00");
}

void ControlsPanel::updateVolumeIcon(const int &data)
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
}

void ControlsPanel::updateTime(const int &time)
{
	timeLabel->setText(Convert::Seconds::toMinutes(time / 1e3) + "/" + Convert::Seconds::toMinutes(m_length));
	timeSlider->setValueDontMove(time / 1e3);
}

void ControlsPanel::paintEvent(QPaintEvent *pe)
{
	QStyleOption o;
	o.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
};
