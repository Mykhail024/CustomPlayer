#pragma once

#include <QObject>
#include <QTimer>
#include <QUdpSocket>

class NetworkDiscoverer : public QObject
{
        Q_OBJECT
    public:
        explicit NetworkDiscoverer(QObject *parent = nullptr);
        ~NetworkDiscoverer();

        Q_INVOKABLE void startDiscovering();
        Q_INVOKABLE void stopDiscovering();

    signals:
        void peerDiscovered(const QString &address, quint16 port, const QString &playlistName);

    private slots:
        void readBroadcastDatagrams();
        void sendDiscoveryRequest();

    private:
        QUdpSocket *m_listenSocket;
        QUdpSocket *m_discoverSocket;
        QTimer *m_discoveryTimer;
};
