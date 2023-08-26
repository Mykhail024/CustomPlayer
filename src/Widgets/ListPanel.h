#pragma once

#include <QTableWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QWidget>
#include <vector>

#include "../DataBase/SQLiteHandler.h"
#include "../Config/Config.h"

class ListPanel : public QTableView {
    Q_OBJECT

	public:
		ListPanel(QString dataBase, QWidget *parent = nullptr);

	private slots:
		//Input
		void onUpdate(QString path);
		void onSetupColumns();
		void onSetupRows();

		//Output
		void Play(const QItemSelection &selected, const QItemSelection &deselected);
	signals:
		//Input
		void update(QString path);
		void setupColumns();
		void setupRows();

		//Output
		void onPlay(std::string path);

	private:
		QStandardItemModel *model;

		Config::COLLUMNS columns;
		DB::SQLiteHandler db;

		std::vector<DB::SONG> songs;

		QString db_path;
		QString music_path;
};
