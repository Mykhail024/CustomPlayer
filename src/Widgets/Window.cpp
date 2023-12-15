#include <QPainter>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>
#include <QFileInfo>
#include <QImage>
#include <QFileDialog>

#include <vector>

#include "ControlsPanel.h"
#include "PlaylistControl.h"
#include "SongsWidget.h"

#include "Window.h"

#include "moc_Window.cpp"

Window::Window()
{
	QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
	this->resize(screenGeometry.size() * 0.8);

	this->setObjectName("MainWindow");

	auto *v_layout = new QVBoxLayout(this);
	auto *h_layout = new QHBoxLayout();
	v_layout->setContentsMargins(0, 0, 0, 0);
	h_layout->setContentsMargins(0, 0, 0, 0);
	v_layout->setSpacing(0);
	h_layout->setSpacing(0);

	m_songsWidget = new SongsWidget(this);
	m_controlsPanel = new ControlsPanel();
	m_playlistControl = new PlaylistControl(this);

	h_layout->addWidget(m_songsWidget);

	v_layout->addWidget(m_controlsPanel, 0, Qt::AlignTop);
	v_layout->addLayout(h_layout);
	v_layout->addWidget(m_playlistControl);

	this->setLayout(v_layout);
}

Window::~Window()
{
}

void Window::paintEvent(QPaintEvent *pe)
{
	QStyleOption o;
	o.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
};
