#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QList>

#include "IEditablePlaylistProvider.h"
#include "Log.h"
#include "PlaylistFormat.h"
#include "UPLPlaylistProvider.h"

UPLPlaylistProvider::UPLPlaylistProvider(QObject *parent) : IEditablePlaylistProvider(parent)
{

    //setPlaylistName("remote");
    //setUri("/home/hong19/.config/CustomPlayer/Playlists/remote.upl");
    //save();
}

QString UPLPlaylistProvider::playlistName() const { return m_playlistName; }

QList<PlaylistFormat> UPLPlaylistProvider::uriFormats() const
{
    return {
        {"UPL", {"*.upl"}}
    };
}

QString UPLPlaylistProvider::uri() const { return m_filePath; };

bool UPLPlaylistProvider::setUri(const QString &uri)
{
    m_filePath = uri;
    return true;
}

int UPLPlaylistProvider::songsCount() const { return m_songs.size(); }

SongMetadata UPLPlaylistProvider::songAt(int index) const
{
    if (index < 0 || index >= m_songs.count())
        return {};
    return m_songs.at(index);
}

bool UPLPlaylistProvider::update() { return load(m_filePath); }

bool UPLPlaylistProvider::setPlaylistName(const QString &playlistName)
{
    m_playlistName = playlistName;
    return true;
}

bool UPLPlaylistProvider::setSongAt(int index, const SongMetadata &entry)
{
    if (index < 0 || index >= m_songs.size())
        return false;

    m_songs[index] = entry;

    emit songChanged(index);
    return true;
};

bool UPLPlaylistProvider::addSongs(const QList<SongMetadata> &songs)
{
    m_songs.append(songs);
    emit songsAdded(m_songs.size() - songs.size(), songs.size());
    return true;
}

bool UPLPlaylistProvider::removeSong(int index)
{
    if (index < 0 || index >= m_songs.size())
        return false;

    m_songs.removeAt(index);
    emit songRemoved(index);
    return true;
};

bool UPLPlaylistProvider::load(QString filePath)
{
    if (filePath.isEmpty()) {
        filePath = m_filePath;
    }

    if (filePath.isEmpty()) {
        Log_Warning("Failed to load playlist, filepath is empty");
        return false;
    }

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Log_Warning(QString("Cannot open UPL file: %1. Err: %2").arg(filePath, file.errorString()));
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;

    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        Log_Warning(QString("Json parse error: %1").arg(parseError.errorString()));
        return false;
    }

    if (!doc.isArray()) {
        Log_Warning("UPL root element is not an array");
        return false;
    }

    QJsonArray pArray = doc.array();

    if (pArray.isEmpty()) {
        return false;
    }

    QJsonObject pObj = pArray.first().toObject();

    const QString format = pObj.value("format").toString();
    if (format != "UPL1") {
        Log_Warning(QString("Unsupported format: %1").arg(format));
        return false;
    }

    const QString pName = pObj.value("name").toString();

    if (!pObj.contains("entries") || !pObj.value("entries").isArray()) {
        Log_Warning("Missing or invalid 'entries' array in UPL");
        return false;
    }

    QList<SongMetadata> songs;

    const QJsonArray pEntries = pObj.value("entries").toArray();
    for (const QJsonValue &pEntry : std::as_const(pEntries)) {
        if (!pEntry.isObject())
            continue;

        QJsonObject eObj = pEntry.toObject();
        SongMetadata song;

        song.title = eObj.value("title").toString();
        song.artist = eObj.value("artist").toString();
        song.album = eObj.value("album").toString();
        song.comment = eObj.value("comment").toString();
        song.length = eObj.value("duration").toDouble() * 1000;

        if (eObj.contains("audio") && eObj.value("audio").isObject()) {
            QJsonObject audioObj = eObj.value("audio").toObject();
            song.channels = audioObj.value("channels").toInt(0);
            song.bitrate = audioObj.value("bitrate").toInt(0);
            song.sampleRate = audioObj.value("sampleRate").toInt(0);
        }

        if (eObj.contains("year"))
            song.year = static_cast<unsigned int>(eObj.value("year").toInt(0));

        if (!eObj.contains("ids") || !eObj.value("ids").isObject()) {
            Log_Warning(QString("No ids field for: %1 - %2").arg(song.title, song.artist));
            continue;
        }

        QJsonObject idsObj = eObj.value("ids").toObject();

        if (!idsObj.contains("filepath") || !idsObj.value("filepath").isArray()) {
            Log_Warning(QString("Invalid filepath field for: %1 - %2").arg(song.title, song.artist));
            continue;
        }

        QJsonArray filepathArray = idsObj.value("filepath").toArray();
        if (filepathArray.isEmpty() || !filepathArray.first().isString()) {
            Log_Warning(QString("Empty or invalid filepath array for: %1 - %2").arg(song.title, song.artist));
            continue;
        }

        song.uri = filepathArray.first().toString();

        songs.append(song);
    }

    m_playlistName = pName;
    m_filePath = filePath;
    m_songs = songs;

    emit reset();
    return true;
}

bool UPLPlaylistProvider::save(QString filePath)
{
    if (filePath.isEmpty()) {
        filePath = m_filePath;
    }

    if (filePath.isEmpty()) {
        Log_Warning("Failed to save playlist, filepath is empty");
        return false;
    }

    QJsonArray pArray;

    QJsonObject pObj;
    pObj["format"] = "UPL1";
    pObj["name"] = m_playlistName;

    QJsonArray entries;
    for (const SongMetadata &song : std::as_const(m_songs)) {
        QJsonObject eObj;
        eObj["title"] = song.title;
        eObj["artist"] = song.artist;
        eObj["album"] = song.album;
        eObj["comment"] = song.comment;
        eObj["duration"] = static_cast<double>(song.length) / 1000.0;

        QJsonObject audioObj;
        audioObj["channels"] = song.channels;
        audioObj["bitrate"] = song.bitrate;
        audioObj["sampleRate"] = song.sampleRate;
        eObj["audio"] = audioObj;

        if (song.year > 0)
            eObj["year"] = static_cast<int>(song.year);

        QJsonObject idsObj;
        idsObj["filepath"] = QJsonArray{QJsonValue(song.uri)};
        eObj["ids"] = idsObj;

        entries.append(eObj);
    }

    pObj["entries"] = entries;
    pArray.append(pObj);

    QJsonDocument doc(pArray);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Log_Warning("Cannot open UPL file for writing");
        return false;
    }

    qint64 written = file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    if (written <= 0) {
        Log_Warning("Failed to write UPL file");
        return false;
    }

    return true;
}
