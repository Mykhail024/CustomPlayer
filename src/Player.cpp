#include <QApplication>

#include "Widgets/Window.h"
#include "Config/Config.h"
#include "Convert.cpp"
#include "Audio/PipeWire.h"

int main(int argc, char** argv)
{
	Config::init_config();

	QApplication app(argc, argv);
	app.setStyleSheet(Convert::File::toQString(":/Style.qss"));

	Audio::PipeWire pipewire;

	Window window(&pipewire);
	window.show();

	return app.exec();
}
