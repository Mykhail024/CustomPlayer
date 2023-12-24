#include <QApplication>

#include "Core/Tools.h"
#include "Core/EventHandler.h"
#include "Core/Globals.h"
#include "Core/PlaylistManager.h"

#include "Widgets/GlobalEventFilter.h"
#include "Widgets/Window.h"

#ifdef __linux__
#include "Core/DBus/MediaPlayer2.h"
#endif

int main(int argc, char** argv)
{
#ifdef __linux__
	qputenv("QT_QPA_PLATFORMTHEME", "flatpak");
	if(argc > 1)
	{
		if(sendPlayDBusSignal(argv[1]))
		{
			return 0;
		}
	}
#endif
	QApplication app(argc, argv);
	GlobalEventFilter filter;
	app.installEventFilter(&filter);
	app.setStyleSheet(readTextFile(":/Style.qss"));
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
