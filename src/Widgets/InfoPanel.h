#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <QLabel>

class InfoPanel : public QWidget
{
	Q_OBJECT
	public:
		InfoPanel(QWidget *parent = nullptr);

	protected:
		void paintEvent(QPaintEvent *pa) override;

	private slots:
		void onSetImage(QImage image);
		void onSetInfo(QString title, QString artist);

	signals:
		void setImage(QImage image);
		void setInfo(QString title, QString artist);
	private:
		QLabel *infoLabel;
		QLabel *imageLabel;

		QString m_title;
		QString m_artist;
};
