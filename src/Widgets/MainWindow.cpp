#include <vector>
#include <iostream>

#include <QScreen>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QPushButton>
#include <QList>
#include <QSlider>
#include <QLabel>
#include <QHeaderView>
#include <QDir>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>
#include <QBrush>
#include <QColor>

#include "MainWindow.hpp"
#include "../Convert.cpp"
#include "../TagReaders/TagReader.hpp"

namespace Widgets {
	static QRect getScreenGeometry()
	{
		return QGuiApplication::primaryScreen()->geometry();
	}

	MainWindow::MainWindow(AudioServers::AudioServer *audioServer, QWidget *parent)
	{
		this->audioServer = audioServer;
		this->audioServer->setLooping(true);

		unsigned int windowWindth = getScreenGeometry().width() * 0.8;
		unsigned int windowHeight = getScreenGeometry().height() * 0.8;
		this->setGeometry(0, 0, windowWindth, windowHeight);
		this->setStyleSheet(Convert::File::toQString(":/Style.qss"));

		QVBoxLayout *head_vbox = new QVBoxLayout(this);
		head_vbox->setContentsMargins(0, 0, 0, 0);

		QWidget *header = new QWidget();
		head_vbox->setAlignment(header, Qt::AlignTop);

		QWidget *interface = new QWidget();
		head_vbox->setAlignment(header, Qt::AlignBottom);

		initHeader(header);
		initInterface(interface);

		head_vbox->addWidget(header);
		head_vbox->addWidget(interface);
	}

	void MainWindow::initHeader(QWidget *header)
	{
		header->setObjectName("Header");
		header->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
		QHBoxLayout *controlLayout = new QHBoxLayout(header);

		timeSlider = new TimeSlider();
		timeSlider->setOrientation(Qt::Horizontal);
		timeSlider->setObjectName("TimeSlider");
		timeSlider->setMinimum(0);
		timeSlider->setDisabled(true);
		audioServer->registerTimeSliderCallback(timeSlider);

		timeLabel = new QLabel();
		timeLabel->setText("0:00/0:00");
		timeLabel->setObjectName("TimeLabel");
		timeLabel->setDisabled(true);

		volumeBtn = new QPushButton();
		volumeBtn->setObjectName("VolButton");

		volumeSlider = new QSlider(Qt::Horizontal);
		volumeSlider->setObjectName("VolumeSlider");
		volumeSlider->setMinimum(0);
		volumeSlider->setMaximum(100);
		volumeSlider->setValue(100);

		const int buttonSize = this->width() * .035;
		const int iconSize = buttonSize * 0.8;

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

		//Connecting
		connect(timeSlider, &QSlider::valueChanged, this, [this](){
					updateTimeLabel(timeSlider->value());
				});
		connect(timeSlider, &QSlider::sliderReleased, this, [this](){
					audioServer->goTo(timeSlider->value());
				});

		connect(volumeSlider, &QSlider::valueChanged, this, [this](){
					audioServer->setVolume(volumeSlider->value());
				});
		connect(volumeBtn, &QPushButton::clicked, this, [this](){
					if(audioServer->getVolume() == 0)
					{
						audioServer->setVolume(non_mute_volume);
					}
					else
					{
						non_mute_volume = audioServer->getVolume();
						audioServer->setVolume(0);
					}
					volumeSlider->setValue(audioServer->getVolume());
				});

		connect(playBtn, &QPushButton::clicked, this, [this](){
					if(audioServer->getPlaybackState() == AudioServers::PLAYING)
					{
						audioServer->pause();
					}
					else if (audioServer->getPlaybackState() == AudioServers::PAUSED) {
						audioServer->resume();
					}
					else if (audioServer->getPlaybackState() == AudioServers::STOPPED)
					{
						if(!files[0].isEmpty())
						{
							playSelectedSong(songList->item(0, 0));
						}
					}
				});

		//Adding
		controlLayout->addWidget(prevBtn);
		controlLayout->addWidget(playBtn);
		controlLayout->addWidget(nextBtn);

		controlLayout->addWidget(timeSlider);
		controlLayout->addWidget(timeLabel);

		controlLayout->addWidget(volumeBtn);
		controlLayout->addWidget(volumeSlider, 0, Qt::AlignLeft);

		//Final setting
		controlLayout->setSpacing(0);
		header->setLayout(controlLayout);
	}
	void MainWindow::initInterface(QWidget *interface)
	{
		interface->setObjectName("Interface");
		QVBoxLayout *mainLayout = new QVBoxLayout(interface);

		songList = new QTableWidget();
		songList->setObjectName("SongList");
		songList->setColumnCount(5);
		songList->verticalHeader()->setVisible(false);
		songList->setSelectionBehavior(QAbstractItemView::SelectRows);
		songList->setSelectionMode(QAbstractItemView::SingleSelection);
		songList->setEditTriggers(QAbstractItemView::NoEditTriggers);

		QStringList headerLabels;
		headerLabels << "Title" << "Artist" << "Album" << "Length" << "Year";
		songList->setHorizontalHeaderLabels(headerLabels);

		songList->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
		for (int i = 1; i < songList->horizontalHeader()->count(); i++) {
			songList->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
		}

		connect(songList, &QTableWidget::itemDoubleClicked, this, &MainWindow::playSelectedSong);

		mainLayout->addWidget(songList);
		interface->setLayout(mainLayout);
	}

	void MainWindow::addFolder(const QString& folderPath)
	{
		folders.push_back(folderPath);
	}

	void MainWindow::getFileListInFolders()
	{
		for(const auto& folder : folders)
		{
			QDir dir(folder);
			QStringList temp_files = dir.entryList(QStringList("*.mp3"), QDir::Files, QDir::Time);
			for(const auto& file : temp_files)
			{
				files.append(dir.filePath(file));
			}
		}
	}

	void MainWindow::updateSongList()
	{
		getFileListInFolders();
		int filesCount = files.count();
		songList->setRowCount(filesCount);
		for(int row = 0; row < filesCount; row++)
		{
			auto id3v2 = TagReaders::id3v2_read(files[row].toStdString());
			songList->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(id3v2.title)));
			songList->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(id3v2.artist)));
			songList->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(id3v2.album)));
			songList->setItem(row, 3, new QTableWidgetItem(Convert::Seconds::toMinutes(id3v2.length / 1000)));
			songList->setItem(row, 4, new QTableWidgetItem((id3v2.year != 0) ? QString::number(id3v2.year) : ""));
		}
	}

	void MainWindow::playSelectedSong(QTableWidgetItem* item)
	{
		if (item == nullptr)
			return;

		int selectedRow = item->row();
		if (selectedRow >= 0 && selectedRow < songList->rowCount())
		{
			songList->clearSelection();
			songList->selectRow(selectedRow);

			audioServer->play(files[selectedRow].toStdString());
			audioServer->goTo(0);
			setMaxTime(audioServer->getTotalTime()/1000);
			updateTime(audioServer->getCurrentTime()/1000);
			enableControls(true);
			volumeSlider->setValue(audioServer->getVolume());
		}
	}


	void MainWindow::updateTimeLabel(int value)
	{
		timeLabel->setText(Convert::Seconds::toMinutes(value) + "/" + Convert::Seconds::toMinutes(timeSlider->maximum()));
	}
	void MainWindow::updateTime(int value)
	{
		if(value < timeSlider->minimum() || value > timeSlider->maximum())
		{
			return;
		}
		timeSlider->setValue(value);
	}
	void MainWindow::setMaxTime(int value)
	{
		timeSlider->setMaximum(value);
		updateTimeLabel(0);
	}
	void MainWindow::enableControls(bool state)
	{
		timeSlider->setDisabled(!state);
		timeLabel->setDisabled(!state);
		prevBtn->setDisabled(!state);
		nextBtn->setDisabled(!state);
	}
}

