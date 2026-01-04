#pragma once

#include <QDebug>

#ifdef NDEBUG
#define LOCATION ""
#else
#define LOCATION QString("\n\t\033[1;37m(%1:%2)\033[0m").arg(__FILE__).arg(__LINE__)
#endif

#define Log_Info(message) qDebug().noquote() << "[\033[1;37mInfo\033[0m]" << message << LOCATION
#define Log_Warning(message)                                                                       \
    qWarning().noquote() << "[\033[1;33mWarning\033[0m]" << message << LOCATION
#define Log_Error(message)                                                                         \
    qCritical().noquote() << "[\033[1;31mError\033[0m]" << message << LOCATION;                    \
    exit(-1)

#ifdef NDEBUG
#define Log_Debug(message)
#else
#define Log_Debug(message) qDebug().noquote() << "[\033[1;32mDebug\033[0m]" << message << LOCATION
#endif
