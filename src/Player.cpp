#include <QApplication>

#include "Convert.cpp"
#include "Widgets/Window.h"
#include "Core/EventHandler.h"
#include "Core/Globals.h"
#include "Core/PlaylistManager.h"
#include "Widgets/GlobalEventFilter.h"

#ifdef __linux__
#include "DBus/MediaPlayer2.h"
#include <QDBusConnection>
#include <QDBusInterface>

bool sendDbusSignalIfExists(const QString &filePath)
{
	QDBusConnection bus = QDBusConnection::sessionBus();
	QDBusInterface mediaPlayer2("org.mpris.MediaPlayer2.CustomPlayer", "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player", bus);
	if (mediaPlayer2.isValid()) {
		mediaPlayer2.call("OpenFile", filePath);
		return true;
	}
	return false;
}

#endif

int main(int argc, char** argv)
{
#ifdef __linux__
	if(argc > 1)
	{
		if(sendDbusSignalIfExists(argv[1]))
		{
			return 0;
		}
	}
#endif
	QApplication app(argc, argv);
	GlobalEventFilter filter;
	app.installEventFilter(&filter);
	app.setStyleSheet(Convert::File::toQString(":/Style.qss"));
    QCoreApplication::setOrganizationName(QStringLiteral("Mykhail024"));
    QCoreApplication::setApplicationName(QStringLiteral("CustomPlayer"));

	initGlobals();
	initEventHandler();
	initPlaylistManager();
#ifdef __linux__
	initDBusService();
#endif
	Window window;
	window.show();

	if(argc > 1)
	{
		eventHandler()->PlaySong(argv[1]);
	}
	int exit_code = app.exec();

#ifdef __linux__
	deinitDBusService();
#endif
	deinitPlaylistManager();
	deinitEventHandler();
	deinitGlobals();

	return exit_code;
}
