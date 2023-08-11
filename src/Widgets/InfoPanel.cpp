
#include <QStyleOption>
#include <QPainter>
#include <QPixmap>
#include <QVBoxLayout>
#include <qnamespace.h>
#include <qwidget.h>

#include "InfoPanel.h"

InfoPanel::InfoPanel(QWidget *parent) : QWidget(parent)
{
	this->setObjectName("InfoPanel");

	auto *layout = new QVBoxLayout(this);

	imageLabel = new QLabel();
	imageLabel->setContentsMargins(0, 0, 0, 0);

	infoLabel = new QLabel();
	infoLabel->setAlignment(Qt::AlignCenter);
	infoLabel->setWordWrap(true);

	layout->addWidget(imageLabel, 0, Qt::AlignTop);
	layout->addWidget(infoLabel, 1, Qt::AlignTop);
	this->setLayout(layout);

	connect(this, &InfoPanel::setImage, &InfoPanel::onSetImage);
	connect(this, &InfoPanel::setInfo, &InfoPanel::onSetInfo);
}

void InfoPanel::onSetImage(QImage image)
{
	QPixmap pixmap = QPixmap::fromImage(image.scaledToWidth(imageLabel->width()/2, Qt::SmoothTransformation));
	imageLabel->resize(pixmap.size());
	imageLabel->setPixmap(pixmap);
}

void InfoPanel::onSetInfo(QString title, QString artist)
{
	m_title = title;
	m_artist = artist;
	infoLabel->setText(QString("%1\n%2").arg(title).arg(artist));
}

void InfoPanel::paintEvent(QPaintEvent *pe)
{
	QStyleOption o;
	o.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
};
