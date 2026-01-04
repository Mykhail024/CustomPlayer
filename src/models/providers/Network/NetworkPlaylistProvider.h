#pragma once

#include "IEditablePlaylistProvider.h"
#include "NetworkBroadcaster.h"
#include <QTcpServer>
#include <QUrl>
#include <QTcpSocket>
#include <QVector>

class NetworkPlaylistProvider : public IEditablePlaylistProvider
{
    Q_OBJECT
    Q_INTERFACES(IEditablePlaylistProvider)
public:
    enum Mode { Create, Connect };

    explicit NetworkPlaylistProvider(Mode mode, const QString &password, const QString &ipAddress = QString(), QObject *parent = nullptr);
    ~NetworkPlaylistProvider();

    // IPlaylistProvider interface
    QList<PlaylistFormat> uriFormats() const override;
    QString uri() const override;
    bool setUri(const QString &uri) override;
    bool isReadOnly() const override;
    QString playlistName() const override;
    int songsCount() const override;
    SongMetadata songAt(int index) const override;
    bool update() override;
    bool load(QString uri = QString()) override;

    // IEditablePlaylistProvider interface
    bool setPlaylistName(const QString &playlistName) override;
    bool setSongAt(int index, const SongMetadata &entry) override;
    bool addSongs(const QList<SongMetadata> &songs) override;
    bool removeSong(int index) override;
    bool save(QString uri = QString()) override;
    void sendPlaylist(QTcpSocket *specificClient);

    void startBroadcasting();
    void stopBroadcasting();

signals:
    void connectionError(const QString &error);

private slots:
    void newConnection();
    void readData();
    void clientDisconnected();
    void handleError(QAbstractSocket::SocketError socketError);

private:
    void sendPlaylist();
    void processIncomingData(const QByteArray &data);

    bool saveLocalPlaylist(const QString &filePath);
    bool loadLocalPlaylist(const QString &filePath);
    bool saveRemotePlaylist(const QString &filePath);
    bool loadRemotePlaylist(const QString &filePath);

    Mode m_mode;
    QString m_password;
    QString m_ipAddress;
    QString m_playlistName;
    QVector<SongMetadata> m_songs;
    QString m_uri;

    // For server (Create mode)
    QTcpServer *m_tcpServer;
    QList<QTcpSocket*> m_clients;
    NetworkBroadcaster *m_broadcaster;

    // For client (Connect mode)
    QTcpSocket *m_tcpSocket;
};
