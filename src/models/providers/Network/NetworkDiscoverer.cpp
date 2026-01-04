#include "NetworkDiscoverer.h"
#include <QHostAddress>

NetworkDiscoverer::NetworkDiscoverer(QObject *parent)
    : QObject(parent)
    , m_listenSocket(new QUdpSocket(this))
    , m_discoverSocket(new QUdpSocket(this))
    , m_discoveryTimer(new QTimer(this))
{
    connect(m_listenSocket, &QUdpSocket::readyRead, this, &NetworkDiscoverer::readBroadcastDatagrams);
    connect(m_discoveryTimer, &QTimer::timeout, this, &NetworkDiscoverer::sendDiscoveryRequest);
}

NetworkDiscoverer::~NetworkDiscoverer() { stopDiscovering(); }

void NetworkDiscoverer::startDiscovering()
{
    if (m_listenSocket->state() != QUdpSocket::BoundState) {
        if (m_listenSocket->bind(QHostAddress::AnyIPv4, 45454, QUdpSocket::ShareAddress)) {
            m_discoveryTimer->start(5000); // Send discovery every 5 seconds
            sendDiscoveryRequest();        // Send immediate request
        }
    }
}

void NetworkDiscoverer::stopDiscovering()
{
    m_discoveryTimer->stop();
    if (m_listenSocket->state() == QUdpSocket::BoundState) {
        m_listenSocket->close();
    }
}

void NetworkDiscoverer::sendDiscoveryRequest()
{
    QByteArray datagram = "DISCOVER_CUSTOMPLAYER";
    m_discoverSocket->writeDatagram(datagram, QHostAddress::Broadcast, 45454);
}

void NetworkDiscoverer::readBroadcastDatagrams()
{
    while (m_listenSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_listenSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_listenSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        if (datagram.startsWith("CUSTOMPLAYER_BROADCAST:")) {
            QString playlistName = QString::fromUtf8(datagram.mid(23));
            emit peerDiscovered(sender.toString(), senderPort, playlistName);
        }
    }
}
