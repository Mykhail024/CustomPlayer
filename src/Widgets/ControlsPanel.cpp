#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QWidget>

#include "Controls/TimeSlider.h"
#include "Controls/Slider.h"

#include "Core/EventHandler.h"
#include "Core/Globals.h"

#include "Core/Playlist/PlaylistModel.h"

#include "ControlsPanel.h"

#include "moc_ControlsPanel.cpp"

ControlsPanel::ControlsPanel(QWidget *parent) : QWidget(parent)
{
    this->setObjectName("ControlsPanel");

    auto *layout = new QHBoxLayout(this);

    m_timeSlider = new Controls::TimeSlider();
    m_timeSlider->setOrientation(Qt::Horizontal);
    m_timeSlider->setObjectName("TimeSlider");
    m_timeSlider->setMinimum(0);
    m_timeSlider->setDisabled(true);

    m_timeLabel = new QLabel();
    m_timeLabel->setText("0:00/0:00");
    m_timeLabel->setObjectName("TimeLabel");
    m_timeLabel->setDisabled(true);

    const int buttonSize = this->width() * 0.06;
    const int iconSize = buttonSize * 0.8;

    m_muteBtn = new QPushButton();
    m_muteBtn->setObjectName("MuteButton");
    m_muteBtn->setCheckable(true);
    m_muteBtn->setFixedSize(buttonSize * 0.7, buttonSize * 0.7);
    m_muteBtn->setIconSize(QSize(iconSize * 0.7, iconSize * 0.7));

    m_volumeSlider = new Controls::Slider(Qt::Horizontal);
    m_volumeSlider->setObjectName("VolumeSlider");
    m_volumeSlider->setMinimum(0);
    m_volumeSlider->setMaximum(100);
    float volume = globals()->volume() * 100;
    m_volumeSlider->setValue(volume);
    updateVolumeIcon(volume);

    m_repeatBtn = new QPushButton();
    m_repeatBtn->setObjectName("RepeatButton");
    m_repeatBtn->setCheckable(true);
    m_repeatBtn->setChecked(globals()->loopState());
    m_repeatBtn->setFixedSize(buttonSize * 0.7, buttonSize * 0.7);
    m_repeatBtn->setIconSize(QSize(iconSize * 0.7, iconSize * 0.7));

    m_shuffleBtn = new QPushButton();
    m_shuffleBtn->setObjectName("ShuffleButton");
    m_shuffleBtn->setCheckable(true);
    m_shuffleBtn->setChecked(globals()->shuffleState());
    m_shuffleBtn->setFixedSize(buttonSize * 0.5, buttonSize * 0.5);
    m_shuffleBtn->setIconSize(QSize(iconSize * 0.5, iconSize * 0.5));

    m_prevBtn = new QPushButton();
    m_prevBtn->setObjectName("PrevButton");
    m_prevBtn->setFixedSize(buttonSize, buttonSize);
    m_prevBtn->setIconSize(QSize(iconSize, iconSize));
    m_prevBtn->setDisabled(true);

    m_playBtn = new QPushButton();
    m_playBtn->setObjectName("PlayButton");
    m_playBtn->setFixedSize(buttonSize, buttonSize);
    m_playBtn->setIconSize(QSize(iconSize, iconSize));
    m_playBtn->setCheckable(true);
    m_playBtn->setEnabled(false);

    m_nextBtn = new QPushButton();
    m_nextBtn->setObjectName("NextButton");
    m_nextBtn->setFixedSize(buttonSize, buttonSize);
    m_nextBtn->setIconSize(QSize(iconSize, iconSize));
    m_nextBtn->setDisabled(true);

    layout->addWidget(m_prevBtn);
    layout->addWidget(m_playBtn);
    layout->addWidget(m_nextBtn);
    layout->addWidget(m_timeSlider);
    layout->addWidget(m_timeLabel);
    layout->addWidget(m_repeatBtn);
    layout->addWidget(m_shuffleBtn);
    layout->addWidget(m_muteBtn);
    layout->addWidget(m_volumeSlider, 0, Qt::AlignLeft);

    layout->setSpacing(0);

    connect(eventHandler(), &EventHandler::onPlaySong, this, [&](const SONG_METADATA &metadata){
                m_length = metadata.Length;
                m_timeSlider->setMaximum(m_length / 1e3);
                m_timeSlider->setEnabled(true);
                if(m_length < 3.6e+6) {
                    m_displayFormat = "mm:ss";
                } else {
                    m_displayFormat = "hh:mm:ss";
                }
            });
    connect(eventHandler(), &EventHandler::onCanPlayChanged, m_playBtn, &QPushButton::setEnabled);
    connect(eventHandler(), &EventHandler::onCanNextChanged, m_nextBtn, &QPushButton::setEnabled);
    connect(eventHandler(), &EventHandler::onCanPrevChanged, m_prevBtn, &QPushButton::setEnabled);
    connect(eventHandler(), &EventHandler::onPlaybackStateChanged, this, [&](const PLAYBACK_STATE &state){
                m_playBtn->setChecked(state == PLAYBACK_STATE::PLAYING ? true : false);
            });
    connect(eventHandler(), &EventHandler::onStop, this, &ControlsPanel::reset);
    connect(eventHandler(), &EventHandler::onEndSong, this, &ControlsPanel::reset);
    connect(eventHandler(), &EventHandler::onPositionChange, this, &ControlsPanel::updateTime);
    connect(eventHandler(), &EventHandler::onSeek, this, &ControlsPanel::updateTime);
    connect(m_timeSlider, &Controls::TimeSlider::sliderReleased, [&]{
                eventHandler()->Seek(m_timeSlider->value() * 1e3);
            });
    connect(m_volumeSlider, &Controls::Slider::valueChanged, this, [&](int value){
                eventHandler()->VolumeChange((float)value / 100.0f);
                updateVolumeIcon(value);
            });
    connect(m_muteBtn, &QPushButton::clicked, eventHandler(), &EventHandler::VolumeMuteUnmute);
    connect(eventHandler(), &EventHandler::onVolumeChange, this, [&](const float &volume){
                m_volumeSlider->setValue(volume * 100);
            });
    connect(m_repeatBtn, &QPushButton::clicked, eventHandler(), &EventHandler::LoopStateChange);
    connect(eventHandler(), &EventHandler::onLoopStateChange, m_repeatBtn, &QPushButton::setChecked);
    connect(m_shuffleBtn, &QPushButton::clicked, eventHandler(), &EventHandler::ShuffleStateChange);
    connect(eventHandler(), &EventHandler::onShuffleStateChange, m_shuffleBtn, &QPushButton::setChecked);

    connect(m_playBtn, &QPushButton::clicked, eventHandler(), &EventHandler::PlayPause);
    connect(m_nextBtn, &QPushButton::clicked, eventHandler(), &EventHandler::NextSong);
    connect(m_prevBtn, &QPushButton::clicked, eventHandler(), &EventHandler::PrevSong);
}

ControlsPanel::~ControlsPanel() {}

void ControlsPanel::reset()
{
    m_timeSlider->setEnabled(false);
    m_timeLabel->setText("0:00/0:00");
}

void ControlsPanel::updateVolumeIcon(const int &data)
{
    QString iconPath;
    if (data == 0) {
        iconPath = ":/Icons/volume-mute.svg";
    }
    else if (data <= 33) {
        iconPath = ":/Icons/volume-low.svg";
    }
    else if (data <= 66) {
        iconPath = ":/Icons/volume-medium.svg";
    } else {
        iconPath = ":/Icons/volume-high.svg";
    }

    m_muteBtn->setStyleSheet(QString("#MuteButton { icon: url(%1); }"
                "#MuteButton:hover { icon: url(%2)}")
            .arg(iconPath)
            .arg(iconPath.replace(".svg", "_hover.svg")));
}

void ControlsPanel::updateTime(const int &time)
{
    QString currentTime = QTime::fromMSecsSinceStartOfDay(time).toString(m_displayFormat);
    QString totalLength = QTime::fromMSecsSinceStartOfDay(m_length).toString(m_displayFormat);

    m_timeLabel->setText(currentTime + "/" + totalLength);
    m_timeSlider->setValueDontMove(qRound((float)time / 1e3));
    m_timeSlider->setEnabled(true);
}

void ControlsPanel::paintEvent(QPaintEvent *pe)
{
    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}
