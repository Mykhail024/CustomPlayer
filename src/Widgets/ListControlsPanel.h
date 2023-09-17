#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>

class ListControlsPanel : public QWidget
{
	Q_OBJECT
	public:
		ListControlsPanel(QWidget *parrent = nullptr);

	private slots:
		void onOpenFolderButtonClick() { emit openFolderButtonClick(); };
		void onFindLineEditTextChange(const QString &text) { emit findLineEditTextChange(text);}

	signals:
		void openFolderButtonClick();
		void findLineEditTextChange(const QString &text);

	protected:
		void paintEvent(QPaintEvent *pe) override;

	private:
		QPushButton *openFolderBtn;
		QLineEdit *findLineEdit;
};
