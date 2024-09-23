#include <QPushButton>

#include "ControlsPanel.h"

ControlsPanel::ControlsPanel(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QHBoxLayout(this))
    , m_prev(new QPushButton(this))
    , m_play_pause(new QPushButton(this))
    , m_next(new QPushButton(this))
    , m_timeline(new QSlider(this))
    , m_loop(new QPushButton(this))
    , m_shuffle(new QPushButton(this))
    , m_mute(new QPushButton(this))
    , m_volume(new QSlider(this))
{
    this->setObjectName("ControlsPanel");

#ifdef NDEBUG
    this->setEnabled(false);
#endif // !NDEBUG

#ifdef Q_OS_LINUX
    m_prev->setIcon(QIcon::fromTheme("media-skip-backward"));
    m_play_pause->setIcon(QIcon::fromTheme("media-playback-stop"));
    m_next->setIcon(QIcon::fromTheme("media-skip-forward"));
#endif

    m_prev->setProperty("type", "media-backward");
    m_play_pause->setProperty("type", "media-forward");
    m_next->setProperty("type", "media-play-pause");
    m_timeline->setProperty("type", "media-timeline");

    m_timeline->setOrientation(Qt::Horizontal);

    m_volume->setOrientation(Qt::Horizontal);
    m_volume->setRange(0, 100);

    m_layout->addWidget(m_prev);
    m_layout->addWidget(m_play_pause);
    m_layout->addWidget(m_next);
    m_layout->addWidget(m_timeline, 1);
    m_layout->addWidget(m_loop);
    m_layout->addWidget(m_shuffle);
    m_layout->addWidget(m_mute);
    m_layout->addWidget(m_volume);

    connect(m_prev, &QPushButton::clicked, this, &ControlsPanel::onPrevClicked);
    connect(m_play_pause, &QPushButton::clicked, this, &ControlsPanel::onPlayPauseClicked);
    connect(m_next, &QPushButton::clicked, this, &ControlsPanel::onNextClicked);
}
