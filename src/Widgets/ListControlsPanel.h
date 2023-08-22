#pragma once

#include <QWidget>
#include <QPushButton>

class ListControlsPanel : public QWidget
{
	Q_OBJECT
	public:
		ListControlsPanel(QWidget *parrent = nullptr);

	private slots:
		void onOpenFolderButtonClick() { emit openFolderButtonClick(); };

	signals:
		void openFolderButtonClick();

	protected:
		void paintEvent(QPaintEvent *pe) override;

	private:
		QPushButton *openFolderBtn;
};
