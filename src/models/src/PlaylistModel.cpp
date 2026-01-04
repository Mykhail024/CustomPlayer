#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>

#include "DiskImageCache.h"
#include "ID3V2Handler.h"
#include "IEditablePlaylistProvider.h"
#include "IPlaylistProvider.h"
#include "Log.h"
#include "PlaylistModel.h"

PlaylistModel::PlaylistModel(QSharedPointer<IPlaylistProvider> provider, QObject *parent)
    : QAbstractListModel(parent)
    , m_provider(provider)
{
}

bool PlaylistModel::setProvider(QSharedPointer<IPlaylistProvider> provider) { return false; }

QString PlaylistModel::playlistName() const { return m_provider->playlistName(); }

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_provider)
        return 0;
    return m_provider->songsCount();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();

    if (!index.isValid() || row < 0 || row >= m_provider->songsCount())
        return QVariant();

    const auto &song = m_provider->songAt(row);

    switch (role) {
    case TitleRole:
        return !song.title.isEmpty() ? song.title : QFileInfo(song.uri).completeBaseName();
    case ArtistRole:
        return song.artist;
    case AlbumRole:
        return song.album;
    case GenreRole:
        return song.genre;
    case CommentRole:
        return song.comment;
    case LengthRole:
        return song.length;
    case ChannelsRole:
        return song.channels;
    case BitRateRole:
        return song.bitrate;
    case SampleRateRole:
        return song.sampleRate;
    case YearRole:
        return song.year;
    case UriRole:
        return song.uri;
    case AlbumArtRole:
        return DiskImageCache::instance()->retrieve(song.uri);
    default:
        return QVariant();
    }
}

bool PlaylistModel::addSong(const SongMetadata &song) { return addSongs({song}); }
bool PlaylistModel::addSongs(const QList<SongMetadata> &songs)
{
    int oldCount = m_provider->songsCount();
    int newCount = oldCount + songs.size();

    auto editable = qobject_cast<IEditablePlaylistProvider *>(m_provider.get());
    if (!editable)
        return false;

    if (!editable->addSongs(songs))
        return false;

    beginInsertRows(QModelIndex(), oldCount, newCount - 1);
    endInsertRows();

    editable->save();

    return true;
}

static QString normalizeFilePath(const QUrl &url)
{
    if (url.isValid() && url.scheme() == "file") {
        return url.toLocalFile(); // Decoded local file path
    }
    // If not a file URL, return as-is
    return url.toString();
}

bool PlaylistModel::addSong(const QUrl &url)
{
    QString normalizedPath = normalizeFilePath(url);
    if (!QFile::exists(normalizedPath)) {
        Log_Error(QString("File %1 does not exist").arg(normalizedPath));
    }

    return addSong(id3v2_read(normalizedPath));
}

bool PlaylistModel::addSongs(const QList<QUrl> &urls)
{
    QList<SongMetadata> songsToAdd;
    for (const auto &url : std::as_const(urls)) {
        QString normalizedPath = normalizeFilePath(url);
        auto data = id3v2_read(normalizedPath);
        if (!data.uri.isEmpty()) {
            songsToAdd.append(std::move(data));
        }
    }

    if (!songsToAdd.isEmpty()) {
        return addSongs(songsToAdd);
    }

    return false;
}

bool PlaylistModel::removeSongs(int index, int count) { return false; }

bool PlaylistModel::setPlaylistName(const QString &playlistName)
{
    auto editable = qobject_cast<IEditablePlaylistProvider *>(m_provider.get());

    if (!editable) {
        return false;
    }

    return editable->setPlaylistName(playlistName);
}
