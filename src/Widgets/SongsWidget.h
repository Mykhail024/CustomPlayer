#pragma once

#include <QTabWidget>

class SongsWidget : public QTabWidget
{
	Q_OBJECT
	public:
		SongsWidget(QWidget *parent = nullptr);
	public slots:
		void selectNextTab();
		void selectPrevTab();
		void addPlaylists();
		void setupTabs();

	private slots:
		void onTabClose(const int &index);
};
