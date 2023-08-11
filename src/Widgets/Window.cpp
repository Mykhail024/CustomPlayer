#include <QPainter>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <map>
#include <qlistview.h>
#include <vector>

#include "InfoPanel.h"
#include "ListPanel.h"
#include "Window.h"
#include "ControlsPanel.h"
#include "../TagReaders/TagReader.h"

Window::Window()
{
	this->setObjectName("MainWindow");
	unsigned int windowWindth = getScreenGeometry().width() * 0.8;
	unsigned int windowHeight = getScreenGeometry().height() * 0.8;
	this->setGeometry(0, 0, windowWindth, windowHeight);

	auto *v_layout = new QVBoxLayout(this);
	auto *h_layout = new QHBoxLayout();
	v_layout->setContentsMargins(0, 0, 0, 0);
	h_layout->setContentsMargins(0, 0, 0, 0);
	v_layout->setSpacing(0);
	h_layout->setSpacing(0);

	auto *controls_panel = new ControlsPanel();

	auto *info_panel = new InfoPanel();
	info_panel->setFixedWidth(windowWindth * 0.28);

	auto *list_panel = new ListPanel("");
	list_panel->setupColumns();

	v_layout->addWidget(controls_panel, 0, Qt::AlignTop);
	h_layout->addWidget(info_panel, 0, Qt::AlignLeft);
	h_layout->addWidget(list_panel, 1);

	v_layout->addLayout(h_layout);
	this->setLayout(v_layout);

	auto data = TagReaders::id3v2_read("/home/hong19/Music/Kirasa II - Jigokuraku Opening Full 『WORK』 millennium parade × Sheena Ringo 【ENG Sub】.mp3");
	info_panel->setInfo(QString::fromStdString(data.title), QString::fromStdString(data.artist));
	info_panel->setImage(TagReaders::id3v2_get_image("/home/hong19/Music/Kirasa II - Jigokuraku Opening Full 『WORK』 millennium parade × Sheena Ringo 【ENG Sub】.mp3"));
}

Window::~Window()
{

}

void Window::keyPressEvent(QKeyEvent* event)
{
	QWidget::keyPressEvent(event);
}
void Window::paintEvent(QPaintEvent *pe)
{
	QStyleOption o;
	o.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
};
