#include "NetworkBroadcaster.h"
#include <QHostAddress>

NetworkBroadcaster::NetworkBroadcaster(QObject *parent)
: QObject(parent)
, m_broadcastSocket(new QUdpSocket(this))
, m_broadcastTimer(new QTimer(this))
{
    connect(m_broadcastTimer, &QTimer::timeout, this, &NetworkBroadcaster::broadcastDatagram);
}

NetworkBroadcaster::~NetworkBroadcaster()
{
    stopBroadcasting();
}

void NetworkBroadcaster::startBroadcasting(const QStringList &playlistNames)
{
    m_playlistNames = playlistNames;
    m_broadcastTimer->start(2000); // Broadcast every 2 seconds
}

void NetworkBroadcaster::stopBroadcasting()
{
    m_broadcastTimer->stop();
}

void NetworkBroadcaster::addPlaylist(const QString &playlistName)
{
    if (!m_playlistNames.contains(playlistName)) {
        m_playlistNames.append(playlistName);
    }
}

void NetworkBroadcaster::removePlaylist(const QString &playlistName)
{
    m_playlistNames.removeAll(playlistName);
}

void NetworkBroadcaster::broadcastDatagram()
{
    if (m_playlistNames.isEmpty())
        return;

    foreach (const QString &playlistName, m_playlistNames) {
        QByteArray datagram = "CUSTOMPLAYER_BROADCAST:";
        datagram.append(playlistName.toUtf8());
        m_broadcastSocket->writeDatagram(datagram, QHostAddress::Broadcast, 45454);
    }
}
