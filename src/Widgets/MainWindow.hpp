#pragma once

#include <vector>

#include <QWidget>
#include <QPushButton>
#include <QDateTime>
#include <QLabel>
#include <QSlider>
#include <QTableWidget>

#include "../AudioServers/AudioServer.hpp"
#include "TimeSlider.hpp"

namespace Widgets
{
	class MainWindow : public QWidget
	{
		Q_OBJECT
		public:
			MainWindow(AudioServers::AudioServer *audioServer, QWidget *parent = nullptr);
		public slots:
			void updateTimeLabel(int value);
			void updateTime(int value);
			void setMaxTime(int value);
			void enableControls(bool state);
			void addFolder(const QString& folderPath);
			void updateSongList();
			void playSelectedSong(QTableWidgetItem* item);
		private:
			void getFileListInFolders();
			void initHeader(QWidget *header);
			void initInterface(QWidget *interface);

			AudioServers::AudioServer *audioServer;

			int non_mute_volume;

			QStringList files;
			QStringList folders;
			//Controls
			QPushButton *playBtn;
			QPushButton *prevBtn;
			QPushButton *nextBtn;
			QPushButton *volumeBtn;
			TimeSlider *timeSlider;
			QSlider *volumeSlider;
			QLabel *timeLabel;

			QTableWidget *songList;
	};
}
