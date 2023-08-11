#pragma once

#include <QTableWidget>
#include <QWidget>

class ListPanel : public QTableWidget {
    Q_OBJECT

	public:
		ListPanel(QString dataBase, QWidget *parent = nullptr);

	private slots:
		void onUpdate();
		void onSetupColumns();
	signals:
		void update();
		void setupColumns();

	private:
		std::vector<std::pair<QString, bool>> columns;
		QString dataBase;
};
