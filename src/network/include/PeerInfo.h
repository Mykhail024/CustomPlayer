#pragma once

#include <QHostAddress>
#include <QString>

struct PeerInfo
{
        QString id;
        QHostAddress address;
        quint16 port;
};
