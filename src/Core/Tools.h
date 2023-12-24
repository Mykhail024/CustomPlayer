#pragma once

QString secondsToMinutes(const int &seconds);
QString readTextFile(const QString &filePath);

#ifdef __linux__
bool sendPlayDBusSignal(const QString &filePath);
#endif
