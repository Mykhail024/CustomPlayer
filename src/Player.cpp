#include <QApplication>
#include <QCoreApplication>
#include <iostream>

#include "Widgets/Window.h"
#include "Config/Config.h"
#include "Convert.cpp"

int main(int argc, char** argv)
{
	Config::init_config();

	QApplication app(argc, argv);
	app.setStyleSheet(Convert::File::toQString(":/Style.qss"));

    Window window;
    window.show();

	return app.exec();
}

