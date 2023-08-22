#pragma once

#include <QTableWidget>
#include <QWidget>
#include <vector>

#include "../DataBase/SQLiteHandler.h"
#include "../Config/Config.h"

class ListPanel : public QTableWidget {
    Q_OBJECT

	public:
		ListPanel(QString dataBase, QWidget *parent = nullptr);

	private slots:
		//Input
		void onUpdate(QString path);
		void onSetupColumns();
		void onSetupRows();

		//Output
		void Play();
	signals:
		//Input
		void update(QString path);
		void setupColumns();
		void setupRows();

		//Output
		void onPlay(std::string path);

	private:
		Config::COLLUMNS columns;
		DB::SQLiteHandler db;

		std::vector<DB::SONG> songs;

		QString db_path;
		QString music_path;
};
