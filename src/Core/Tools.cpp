#include <QFile>
#include <QString>
#include <QDBusConnection>
#include <QDBusInterface>

#include "Tools.h"

QString readTextFile(const QString &filePath)
{
    QFile file(filePath);
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        return QString::fromUtf8(file.readAll());
    }
    else {
        qWarning() << "Error reading: " << filePath.toStdString();
        return "";
    }
}
#ifdef __linux__
bool sendPlayDBusSignal(const QString &filePath)
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
