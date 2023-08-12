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
		void onUpdate();
		void onSetupColumns();
		void onSetupRows();
	signals:
		void update();
		void setupColumns();
		void setupRows();

	private:
		Config::COLLUMNS columns;
		DB::SQLiteHandler db;

		std::vector<DB::SONG> songs;
};
