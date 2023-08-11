#pragma once

#include <QWidget>
#include <QGuiApplication>
#include <QScreen>

static QRect getScreenGeometry()
{
	return QGuiApplication::primaryScreen()->geometry();
}

class Window : public QWidget
{
	Q_OBJECT
    public:
        Window();
        ~Window();

    protected:
        void keyPressEvent(QKeyEvent* event) override;
		void paintEvent(QPaintEvent *pa) override;
};

