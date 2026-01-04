#pragma once

#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include <QStringList>

class NetworkBroadcaster : public QObject
{
    Q_OBJECT
public:
    explicit NetworkBroadcaster(QObject *parent = nullptr);
    ~NetworkBroadcaster();

    Q_INVOKABLE void startBroadcasting(const QStringList &playlistNames);
    Q_INVOKABLE void stopBroadcasting();
    Q_INVOKABLE void addPlaylist(const QString &playlistName);
    Q_INVOKABLE void removePlaylist(const QString &playlistName);

private slots:
    void broadcastDatagram();

private:
    QUdpSocket *m_broadcastSocket;
    QTimer *m_broadcastTimer;
    QStringList m_playlistNames;
};
