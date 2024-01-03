#pragma once

QString readTextFile(const QString &filePath);

#ifdef __linux__
bool sendPlayDBusSignal(const QString &filePath);
#endif
