#pragma once

#include <QTableWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QWidget>
#include <QSortFilterProxyModel>
#include <vector>

#include "../DataBase/SQLiteHandler.h"
#include "../Config/Config.h"

class ListPanel : public QTableView {
    Q_OBJECT

	public:
		ListPanel(QString dataBase, QWidget *parent = nullptr);

	public slots:
		int trackNumber() const { return m_trackNumber; };

	private slots:
		//Input
		void onUpdate(QString path);
		void onSetupColumns();
		void onSetupRows();
		int onGetSelected();
		void onSetSelected(int rowIndex);
		void onSelectNext();
		void onSelectPrev();
		void onFind(const QString &text);

		//Output
		void Play(const QItemSelection &selected, const QItemSelection &deselected);
	signals:
		//Input
		void update(QString path);
		void setupColumns();
		void setupRows();

		void find(const QString &text);

		int getSelected();
		void setSelected(int rowIndex);
		void selectNext();
		void selectPrev();

		//Output
		void onPlay(std::string path);

	private:
		QStandardItemModel *model;
		QSortFilterProxyModel *proxyModel;

		bool while_sort;
		QModelIndex selected_index = QModelIndex();

		Config::COLLUMNS columns;
		DB::SQLiteHandler db;

		std::vector<DB::SONG> songs;

		int m_trackNumber;

		QString db_path;
		QString music_path;
};
