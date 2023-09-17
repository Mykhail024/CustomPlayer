#include <QPainter>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>
#include <QFileInfo>
#include <QImage>
#include <QFileDialog>

#include <vector>

#include "ControlsPanel.h"
#include "ListControlsPanel.h"
#include "Window.h"

Window::Window(Audio::AudioServer *server) : audio_server(server), m_playback_status(STOPPED)
{
	this->setObjectName("MainWindow");
	unsigned int windowWindth = getScreenGeometry().width() * 0.8;
	unsigned int windowHeight = getScreenGeometry().height() * 0.8;
	this->setGeometry(0, 0, windowWindth, windowHeight);

	auto *v_layout = new QVBoxLayout(this);
	auto *h_layout = new QHBoxLayout();
	v_layout->setContentsMargins(0, 0, 0, 0);
	h_layout->setContentsMargins(0, 0, 0, 0);
	v_layout->setSpacing(0);
	h_layout->setSpacing(0);

	controls_panel = new ControlsPanel();

	info_panel = new InfoPanel();
	info_panel->setFixedWidth(windowWindth * 0.22);

	list_panel = new ListPanel("main.db3");

	list_control_panel = new ListControlsPanel();

	v_layout->addWidget(controls_panel, 0, Qt::AlignTop);
	v_layout->addLayout(h_layout);
	v_layout->addWidget(list_control_panel, 0, Qt::AlignBottom);
	h_layout->addWidget(info_panel, 0, Qt::AlignLeft);
	h_layout->addWidget(list_panel, 1);

	this->setLayout(v_layout);

	connect(list_panel, &ListPanel::onPlay, this, &Window::Play);

	connect(controls_panel, &ControlsPanel::volumeSliderValueChanged, this, &Window::setVolume);
	connect(controls_panel, &ControlsPanel::timeSliderValueChanged, this, &Window::goTo);
	connect(controls_panel, &ControlsPanel::repeatButtonChecked, this, &Window::setRepeatState);
	connect(controls_panel, &ControlsPanel::playButtonChecked, this, &Window::PlayPause);
	connect(controls_panel, &ControlsPanel::previousButtonClick, this, &Window::Prev);
	connect(controls_panel, &ControlsPanel::nextButtonClick, this, &Window::Next);

	connect(audio_server, &Audio::AudioServer::onTotalTimeChange, this, &Window::updateMaxTime);
	connect(audio_server, &Audio::AudioServer::onCurrentTimeChange, this, &Window::updateCurrentTime);

	connect(list_control_panel, &ListControlsPanel::openFolderButtonClick, this, &Window::openFolder);
	connect(list_control_panel, &ListControlsPanel::findLineEditTextChange, list_panel, &ListPanel::find);

	autosaveInterval = Config::getAutosaveInterval();
	enableAutoSaveConfig();

	int volume = Config::getVolume();
	controls_panel->setVolume(volume);
	audio_server->setVolume(static_cast<float>(volume) / 100.0f);
	setRepeatState(Config::getLoopStatus());
}

Window::~Window()
{
	disableAutoSaveConfig();
	Config::setVolume(audio_server->getVolume() * 100);
}

void Window::Play(std::string path)
{
	Stop();
	m_is_playing = audio_server->play(path);
	controls_panel->setEnabled(m_is_playing);
	controls_panel->setPlayButtonChecked(m_is_playing);

#ifdef __linux__
	if(!dbus_inited)
	{
		initDbus();
	}
#endif

	if(m_is_playing)
	{
		setPlaybackStatus(PLAYING);

		QImage image = TagReaders::id3v2_get_image(path);

		loadMetadata(path);

		if(m_metadata.title.isEmpty())
		{
			m_metadata.title = QFileInfo(QString::fromStdString(path)).baseName();
		}

		info_panel->setInfo(m_metadata.title, m_metadata.artist);

		//Set Image
		if(image.isNull())
		{
			image = QImage(1, 1, QImage::Format_ARGB32);
			image.fill(Qt::transparent);
		}
		else
		{
			info_panel->setImage(image);

			m_ImageUrl.clear();
			saveImage(image);
		}

#ifdef __linux__
		dbus_MediaPlayer2->emitPlaybackStatusChanged();
		dbus_MediaPlayer2->emitMetadataChanged();
#endif

	}
}

#ifdef __linux__
void Window::saveImage(const QImage &image)
{
	QString path;
	QString baseDir;

	if (QDir("/dev/shm").exists())
	{
		baseDir = "/dev/shm/CustomPlayer";
	}
	else
	{
		baseDir = Config::getConfigPath() + "/Images";
	}

	QDir dir(baseDir);
	if (dir.exists())
	{
		dir.removeRecursively();
	}

	dir.mkpath(".");

	qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
	path = dir.filePath("img" + QString::number(currentTime) + ".jpeg");

	bool saved = image.save(path, "jpeg");
	if (saved)
	{
		m_ImageUrl = QUrl::fromLocalFile(path);
	}
}
#endif

void Window::loadMetadata(const std::string &path)
{
	m_metadata = TagReaders::id3v2_read(path);
	if(m_metadata.title.isEmpty())
	{
		m_metadata.title = QFileInfo(QString::fromStdString(path)).baseName();
	}
}

void Window::resetControls()
{
	m_is_playing = false;
	controls_panel->setEnabled(false);
	info_panel->setInfo(nullptr, nullptr);
	auto image = QImage(1, 1, QImage::Format_ARGB32);
	image.fill(Qt::transparent);
	info_panel->setImage(image);
	updateMaxTime(0);
	updateCurrentTime(0);
}

void Window::Stop()
{
	audio_server->stop();
	resetControls();
	setPlaybackStatus(STOPPED);
#ifdef __linux__
	if(dbus_inited)
	{
		dbus_MediaPlayer2->emitPlaybackStatusChanged();
	}
#endif
}

void Window::Pause()
{
	m_is_playing = !audio_server->pause();
	if(!m_is_playing)
	{
		controls_panel->setPlayButtonChecked(m_is_playing);
		setPlaybackStatus(PAUSED);
#ifdef __linux__
		dbus_MediaPlayer2->emitPlaybackStatusChanged();
#endif
	}
}
void Window::Resume()
{
	m_is_playing = audio_server->resume();
	if(m_is_playing)
	{
		controls_panel->setPlayButtonChecked(m_is_playing);
		setPlaybackStatus(PLAYING);
#ifdef __linux__
		dbus_MediaPlayer2->emitPlaybackStatusChanged();
#endif
	}

}
void Window::PlayPause()
{
	if(m_is_playing)
	{
		Pause();
	}
	else
	{
		Resume();
	}
}
void Window::Next()
{
	list_panel->selectNext();
}
void Window::Prev()
{
	list_panel->selectPrev();
}

Window::PlayBackStatus Window::getPlaybackStatus()
{
	return m_playback_status;
}
void Window::setPlaybackStatus(PlayBackStatus status)
{
	m_playback_status = status;
}

void Window::setVolume(int data)
{
	controls_panel->setVolume(data);
	audio_server->setVolume(static_cast<float>(data) / 100.0f);
	m_volume = data / 100.0f;
#ifdef __linux__
	if(dbus_inited)
	{
		dbus_MediaPlayer2->emitVolumeChanged();
	}
#endif
}
void Window::goTo(int data)
{
	audio_server->goTo(data);
	if(!m_is_playing)
	{
		updateCurrentTime(data * 1000);
	}
}
void Window::updateCurrentTime(float time)
{
	controls_panel->updateTime(time / 1000);
}
void Window::updateMaxTime(float time)
{
	controls_panel->updateMaxTime(time / 1000);
}

void Window::setRepeatState(bool state)
{
	audio_server->setLooped(state);
	controls_panel->setRepeatButtonChecked(state);
}

void Window::openFolder()
{
	QString folder = QFileDialog::getExistingDirectory(nullptr, "Select Folder", QDir::homePath());
	if(!folder.isEmpty())
	{
		Stop();
		list_panel->update(folder);
	}
}

void Window::enableAutoSaveConfig()
{
	disableAutoSaveConfig();
	if(autosaveInterval == 0)
	{
		return;
	}

    ConfigAutoSaveTimer = new QTimer(this);
    connect(ConfigAutoSaveTimer, &QTimer::timeout, this, &Window::saveConfig);
    ConfigAutoSaveTimer->start(autosaveInterval * 1000);
}
void Window::disableAutoSaveConfig()
{
	if (ConfigAutoSaveTimer)
    {
        ConfigAutoSaveTimer->stop();
        ConfigAutoSaveTimer->deleteLater();
        ConfigAutoSaveTimer = nullptr;
    }
}

void Window::saveConfig()
{
	Config::setVolume(audio_server->getVolume() * 100);
	Config::setLoopStatus(audio_server->getLoopStatus());
	Config::setAutosaveInterval(autosaveInterval);
}
#ifdef __linux__
void Window::initDbus()
{
	dbus_MediaPlayer2 = new MediaPlayer2(this);
	dbus_inited = true;
}
#endif
void Window::paintEvent(QPaintEvent *pe)
{
	QStyleOption o;
	o.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
};
