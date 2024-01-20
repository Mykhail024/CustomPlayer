#pragma once

#include <QDebug>

#define Log_Info(message) qDebug().noquote() << "[\033[1;37mInfo\033[0m]" << message
#define Log_Warning(message) qWarning().noquote() << "[\033[1;33mWarning\033[0m]" << message
#define Log_Error(message) qCritical().noquote() << "[\033[1;31mError\033[0m]" << message; exit(-1)

#ifdef NDEBUG
    #define Log_Debug(message)
#else
    #define Log_Debug(message) qDebug().noquote() << "[\033[1;32mDebug\033[0m]" << message
#endif

