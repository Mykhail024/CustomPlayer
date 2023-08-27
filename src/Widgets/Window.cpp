#include <QPainter>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QShortcut>
#include <qdatetime.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qlistview.h>
#include <vector>
#include <iostream>
#include <thread>
#include <QFileDialog>

#include "ControlsPanel.h"
#include "ListControlsPanel.h"
#include "Window.h"

#include "../TagReaders/TagReader.h"
#include "../Config/Config.h"

Window::Window(Audio::AudioServer *server) : audio_server(server)
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
	connect(controls_panel, &ControlsPanel::playButtonChecked, this, &Window::PlayPause);
	connect(controls_panel, &ControlsPanel::volumeSliderValueChanged, this, &Window::setVolume);
	connect(controls_panel, &ControlsPanel::timeSliderValueChanged, this, &Window::goTo);
	connect(audio_server, &Audio::AudioServer::onTotalTimeChange, this, &Window::updateMaxTime);
	connect(audio_server, &Audio::AudioServer::onCurrentTimeChange, this, &Window::updateCurrentTime);
	connect(list_control_panel, &ListControlsPanel::openFolderButtonClick, this, &Window::openFolder);
	connect(controls_panel, &ControlsPanel::repeatButtonChecked, this, &Window::setRepeatState);

	int volume = Config::getVolume();
	controls_panel->setVolume(volume);
	audio_server->setVolume(static_cast<float>(volume) / 100.0f);
	setRepeatState(Config::getLoopStatus());

	autosaveInterval = Config::getAutosaveInterval();
	enableAutoSaveConfig();
}

Window::~Window()
{
	disableAutoSaveConfig();
	Config::setVolume(audio_server->getVolume() * 100);
}

void Window::Play(std::string path)
{
	controls_panel->setTimeSliderEnabled(false);
	controls_panel->updateTime(0);
	controls_panel->updateMaxTime(0);
	m_is_playing = audio_server->play(path);
	controls_panel->setEnabled(m_is_playing);
	controls_panel->setPlayButtonChecked(m_is_playing);
	if(m_is_playing)
	{
		auto id3v2 = TagReaders::id3v2_read(path);

		QString title = QString::fromStdString(id3v2.title);
		QString artist = QString::fromStdString(id3v2.artist);

		QImage image = TagReaders::id3v2_get_image(path);

		if(title.isEmpty())
		{
			title = QFileInfo(QString::fromStdString(path)).baseName();
		}

		if(image.isNull())
		{
			image = QImage(1, 1, QImage::Format_ARGB32);
			image.fill(Qt::transparent);
		}

		info_panel->setInfo(title, artist);
		info_panel->setImage(image);
	}
	controls_panel->setTimeSliderEnabled(true);
}
void Window::Pause()
{
	m_is_playing = !audio_server->pause();
	controls_panel->setPlayButtonChecked(m_is_playing);
}
void Window::Resume()
{
	m_is_playing = audio_server->resume();
	controls_panel->setPlayButtonChecked(m_is_playing);
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

}
void Window::Prev()
{

}
void Window::setVolume(int data)
{
	audio_server->setVolume(static_cast<float>(data) / 100.0f);
}
void Window::goTo(int data)
{
	audio_server->goTo(data);
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
		list_panel->update(folder);
		info_panel->setInfo(nullptr, nullptr);
		auto image = QImage(1, 1, QImage::Format_ARGB32);
		image.fill(Qt::transparent);
		info_panel->setImage(image);
		audio_server->stop();
		controls_panel->updateMaxTime(0);
		controls_panel->updateTime(0);
		controls_panel->setTimeSliderEnabled(false);
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

void Window::paintEvent(QPaintEvent *pe)
{
	QStyleOption o;
	o.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
};
