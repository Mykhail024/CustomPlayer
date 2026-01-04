#pragma once

#include <QObject>
#include <QString>
#include <QtQml/qqmlregistration.h>

class Utils : public QObject
{
        QML_ELEMENT
        Q_OBJECT
        QML_SINGLETON
    public:
        Q_INVOKABLE static QString msecToDuration(const int &durationInMsec);
};
