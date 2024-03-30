#include <QApplication>

#include "Core/Tools.h"
#include "Core/EventHandler.h"
#include "Core/Globals.h"

#include "Widgets/Window.h"

#ifdef __linux__
#include "Core/DBus/MediaPlayer2.h"
#endif

int main(int argc, char** argv)
{
#ifdef __linux__
    if(argc > 1) {
        if(sendPlayDBusSignal(argv[1])) {
            return 0;
        }
    } else {
        if(sendRiseDBusSignal()) {
            return 0;
        }
    }
#endif
    QApplication app(argc, argv);
    app.setStyleSheet(readTextFile(":/Style.qss"));
    QCoreApplication::setOrganizationName(QStringLiteral("Mykhail024"));
    QCoreApplication::setApplicationName(QStringLiteral("CustomPlayer"));

    Window window;
    window.show();

    if(argc > 1) {
        eventHandler().PlayFile(argv[1]);
    }

#ifdef __linux__
    dbusService();
#endif

    int exit_code = app.exec();

    return exit_code;
}
