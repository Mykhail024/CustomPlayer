#include "NetworkPlaylistProvider.h"
#include <QDataStream>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTimer>

#include "Log.h"

const QString PLAYLISTS_DIR = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/CustomPlayer/Playlists/";

NetworkPlaylistProvider::NetworkPlaylistProvider(Mode mode, const QString &password, const QString &ipAddress, QObject *parent)
    : IEditablePlaylistProvider(parent)
    , m_mode(mode)
    , m_password(password)
    , m_ipAddress(ipAddress)
    , m_tcpServer(nullptr)
    , m_broadcaster(nullptr)
    , m_tcpSocket(nullptr)
{
    if (m_mode == Create) {
        m_tcpServer = new QTcpServer(this);
        connect(m_tcpServer, &QTcpServer::newConnection, this, &NetworkPlaylistProvider::newConnection);

        m_broadcaster = new NetworkBroadcaster(this);
    } else {
        m_tcpSocket = new QTcpSocket(this);
        connect(m_tcpSocket, &QTcpSocket::readyRead, this, &NetworkPlaylistProvider::readData);
        connect(m_tcpSocket, &QTcpSocket::disconnected, this, &NetworkPlaylistProvider::clientDisconnected);
        connect(m_tcpSocket, &QTcpSocket::errorOccurred, this, &NetworkPlaylistProvider::handleError);
    }
}

NetworkPlaylistProvider::~NetworkPlaylistProvider() { stopBroadcasting(); }

QList<PlaylistFormat> NetworkPlaylistProvider::uriFormats() const
{
    return {
        {"NPL", {"*.npl"}}
    };
}

QString NetworkPlaylistProvider::uri() const { return m_uri; }

bool NetworkPlaylistProvider::setUri(const QString &uri)
{
    m_uri = uri;
    return true;
}

bool NetworkPlaylistProvider::isReadOnly() const
{
    return m_mode == Connect; // Read-only in Connect mode
}

QString NetworkPlaylistProvider::playlistName() const { return m_playlistName; }

int NetworkPlaylistProvider::songsCount() const { return m_songs.size(); }

SongMetadata NetworkPlaylistProvider::songAt(int index) const
{
    if (index < 0 || index >= m_songs.size())
        return SongMetadata();
    return m_songs[index];
}

bool NetworkPlaylistProvider::update()
{
    // In client mode, request update from server
    if (m_mode == Connect && m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
        m_tcpSocket->write("REQUEST_UPDATE");
        return true;
    }
    return false;
}

bool NetworkPlaylistProvider::load(QString uri)
{
    if (!uri.isEmpty()) {
        m_uri = uri;
    }
    if (m_uri.isEmpty()) {
        Log_Error("URI not set for loading");
        return false;
    }
    QString filePath = PLAYLISTS_DIR + QFileInfo(m_uri).fileName();
    if (m_mode == Create) {
        return loadLocalPlaylist(filePath);
    } else {
        return loadRemotePlaylist(filePath);
    }
}

bool NetworkPlaylistProvider::setPlaylistName(const QString &playlistName)
{
    if (m_mode == Create) {
        m_playlistName = playlistName;
        emit nameChanged(playlistName);
        return true;
    }
    return false;
}

bool NetworkPlaylistProvider::setSongAt(int index, const SongMetadata &entry)
{
    if (index < 0 || index >= m_songs.size() || m_mode != Create)
        return false;

    m_songs[index] = entry;
    emit songChanged(index);
    sendPlaylist(); // Broadcast update to all clients
    return true;
}

bool NetworkPlaylistProvider::addSongs(const QList<SongMetadata> &songs)
{
    if (m_mode != Create)
        return false;

    int start = m_songs.size();
    m_songs.append(songs);
    emit songsAdded(start, songs.size());
    sendPlaylist();
    return true;
}

bool NetworkPlaylistProvider::removeSong(int index)
{
    if (index < 0 || index >= m_songs.size() || m_mode != Create)
        return false;

    m_songs.remove(index);
    emit songRemoved(index);
    sendPlaylist();
    return true;
}

bool NetworkPlaylistProvider::save(QString uri)
{
    if (!uri.isEmpty()) {
        m_uri = uri;
    }
    QString filePath = PLAYLISTS_DIR + QFileInfo(m_uri).fileName();
    if (m_mode == Create) {
        return saveLocalPlaylist(filePath);
    } else {
        return saveRemotePlaylist(filePath);
    }
}

void NetworkPlaylistProvider::startBroadcasting()
{
    if (m_broadcaster) {
        m_broadcaster->startBroadcasting(QStringList() << m_playlistName);
    }
}

void NetworkPlaylistProvider::stopBroadcasting()
{
    if (m_broadcaster) {
        m_broadcaster->stopBroadcasting();
    }
}

void NetworkPlaylistProvider::newConnection()
{
    while (m_tcpServer->hasPendingConnections()) {
        QTcpSocket *client = m_tcpServer->nextPendingConnection();
        connect(client, &QTcpSocket::readyRead, this, &NetworkPlaylistProvider::readData);
        connect(client, &QTcpSocket::disconnected, this, &NetworkPlaylistProvider::clientDisconnected);
        m_clients.append(client);

        // Send current playlist to new client
        sendPlaylist(client);
    }
}

void NetworkPlaylistProvider::readData()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket)
        return;

    QByteArray data = socket->readAll();
    processIncomingData(data);
}

void NetworkPlaylistProvider::clientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket) {
        m_clients.removeAll(socket);
        socket->deleteLater();
    }
}

void NetworkPlaylistProvider::handleError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    emit connectionError(m_tcpSocket->errorString());
}

void NetworkPlaylistProvider::sendPlaylist(QTcpSocket *specificClient)
{
    if (m_mode != Create)
        return;

    // Serialize playlist data
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << m_playlistName;
    out << m_songs;

    if (specificClient) {
        specificClient->write(block);
    } else {
        // Broadcast to all connected clients
        for (QTcpSocket *client : std::as_const(m_clients)) {
            client->write(block);
        }
    }
}

void NetworkPlaylistProvider::processIncomingData(const QByteArray &data)
{
    if (m_mode == Connect) {
        // Client received playlist update
        QDataStream in(data);
        in.setVersion(QDataStream::Qt_6_0);
        QString playlistName;
        QVector<SongMetadata> songs;
        in >> playlistName >> songs;

        if (playlistName != m_playlistName) {
            m_playlistName = playlistName;
            emit nameChanged(playlistName);
        }

        if (m_songs != songs) {
            m_songs = songs;
            emit reset();
        }
    }
}

bool NetworkPlaylistProvider::saveLocalPlaylist(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        Log_Error("Could not open file for writing: " + filePath);
        return false;
    }
    QJsonArray entriesArray;
    for (const SongMetadata &song : std::as_const(m_songs)) {
        QJsonObject songObj;
        songObj["title"] = song.title;
        songObj["artist"] = song.artist;
        songObj["album"] = song.album;
        songObj["comment"] = song.comment;
        songObj["duration"] = static_cast<double>(song.length) / 1000.0;
        songObj["year"] = static_cast<int>(song.year);
        QJsonObject audioObj;
        audioObj["bitrate"] = song.bitrate;
        audioObj["channels"] = song.channels;
        audioObj["sampleRate"] = song.sampleRate;
        songObj["audio"] = audioObj;
        QJsonObject idsObj;
        idsObj["filepath"] = QJsonArray{QJsonValue(song.uri)};
        songObj["ids"] = idsObj;
        entriesArray.append(songObj);
    }
    QJsonObject rootObj;
    rootObj["format"] = "UPL1";
    rootObj["name"] = m_playlistName;
    rootObj["entries"] = entriesArray;
    QJsonArray playlistArray;
    playlistArray.append(rootObj);
    QJsonDocument doc(playlistArray);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}
bool NetworkPlaylistProvider::loadLocalPlaylist(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        Log_Error("Could not open file: " + filePath);
        return false;
    }
    QByteArray data = file.readAll();
    file.close();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        Log_Error("JSON parse error: " + parseError.errorString());
        return false;
    }
    if (!doc.isArray()) {
        Log_Error("Root element is not an array");
        return false;
    }
    QJsonArray pArray = doc.array();
    if (pArray.isEmpty()) {
        Log_Error("Empty playlist array");
        return false;
    }
    QJsonObject pObj = pArray.first().toObject();
    m_playlistName = pObj["name"].toString();
    if (!pObj.contains("entries") || !pObj["entries"].isArray()) {
        Log_Error("Missing or invalid entries array");
        return false;
    }
    QVector<SongMetadata> newSongs;
    QJsonArray pEntries = pObj["entries"].toArray();
    for (const QJsonValue &pEntry : std::as_const(pEntries)) {
        if (!pEntry.isObject())
            continue;
        QJsonObject eObj = pEntry.toObject();
        SongMetadata song;
        song.title = eObj["title"].toString();
        song.artist = eObj["artist"].toString();
        song.album = eObj["album"].toString();
        song.comment = eObj["comment"].toString();
        song.length = eObj["duration"].toDouble() * 1000;
        if (eObj.contains("year")) {
            song.year = static_cast<unsigned int>(eObj["year"].toInt());
        }
        if (eObj.contains("audio") && eObj["audio"].isObject()) {
            QJsonObject audioObj = eObj["audio"].toObject();
            song.bitrate = audioObj["bitrate"].toInt();
            song.channels = audioObj["channels"].toInt();
            song.sampleRate = audioObj["sampleRate"].toInt();
        }
        if (eObj.contains("ids") && eObj["ids"].isObject()) {
            QJsonObject idsObj = eObj["ids"].toObject();
            if (idsObj.contains("filepath") && idsObj["filepath"].isArray()) {
                QJsonArray filepathArray = idsObj["filepath"].toArray();
                if (!filepathArray.isEmpty()) {
                    song.uri = filepathArray.first().toString();
                }
            }
        }
        newSongs.append(song);
    }
    m_songs = newSongs;
    return true;
}
bool NetworkPlaylistProvider::saveRemotePlaylist(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        Log_Error("Could not open file for writing: " + filePath);
        return false;
    }
    QJsonArray entriesArray;
    QJsonObject connectionObj;
    connectionObj["ip"] = m_uri.section(':', 0, 0);
    connectionObj["port"] = m_uri.section(':', 1, 1).toInt();
    connectionObj["password"] = m_password;
    entriesArray.append(connectionObj);
    QJsonObject rootObj;
    rootObj["format"] = "NPL1";
    rootObj["name"] = m_playlistName;
    rootObj["connection"] = entriesArray;
    QJsonArray playlistArray;
    playlistArray.append(rootObj);
    QJsonDocument doc(playlistArray);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}
bool NetworkPlaylistProvider::loadRemotePlaylist(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        Log_Error("Could not open file: " + filePath);
        return false;
    }
    QByteArray data = file.readAll();
    file.close();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        Log_Error("JSON parse error: " + parseError.errorString());
        return false;
    }
    if (!doc.isArray()) {
        Log_Error("Root element is not an array");
        return false;
    }
    QJsonArray pArray = doc.array();
    if (pArray.isEmpty()) {
        Log_Error("Empty playlist array");
        return false;
    }
    QJsonObject pObj = pArray.first().toObject();
    m_playlistName = pObj["name"].toString();
    if (!pObj.contains("connection") || !pObj["connection"].isArray()) {
        Log_Error("Missing or invalid connection array");
        return false;
    }
    QJsonArray connArray = pObj["connection"].toArray();
    if (connArray.isEmpty() || !connArray.first().isObject()) {
        Log_Error("Invalid connection data");
        return false;
    }
    QJsonObject connObj = connArray.first().toObject();
    QString ip = connObj["ip"].toString();
    quint16 port = static_cast<quint16>(connObj["port"].toInt());
    m_password = connObj["password"].toString();
    m_uri = ip + ":" + QString::number(port);

    // Now connect to the server
    m_tcpSocket->connectToHost(ip, port);
    if (!m_tcpSocket->waitForConnected(3000)) {
        Log_Error("Connection failed: " + m_tcpSocket->errorString());
        return false;
    }
    QByteArray authData = "AUTH:" + m_password.toUtf8();
    m_tcpSocket->write(authData);
    return m_tcpSocket->waitForBytesWritten();
}
