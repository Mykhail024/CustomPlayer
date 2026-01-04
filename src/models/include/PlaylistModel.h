#pragma once

#include <QAbstractListModel>
#include <QUrl>
#include <QtQml/qqmlregistration.h>
#include <vector>

#include "SongMetadata.h"

class IPlaylistProvider;

class PlaylistModel : public QAbstractListModel
{
        Q_OBJECT
    public:
        explicit PlaylistModel() = delete;
        explicit PlaylistModel(QSharedPointer<IPlaylistProvider> provider, QObject *parent = nullptr);

        Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        Q_INVOKABLE QString playlistName() const;
        QSharedPointer<IPlaylistProvider> provider() const { return m_provider; }

        // struct Song : public SongMetadata
        // {
        //         Song() = default;
        //         Song(const SongMetadata &metadata) : SongMetadata(metadata) /* , favorite(false) */ {}
        //         QUrl albumArt;
        //         // bool favorite;
        // };

        enum SongRoles {
            TitleRole = Qt::UserRole + 1,
            ArtistRole,
            AlbumRole,
            GenreRole,
            CommentRole,
            LengthRole,
            ChannelsRole,
            BitRateRole,
            SampleRateRole,
            YearRole,
            UriRole,
            AlbumArtRole
        };

        QHash<int, QByteArray> roleNames() const override
        {
            return {
                {     TitleRole,      "title"},
                {    ArtistRole,     "artist"},
                {     AlbumRole,      "album"},
                {     GenreRole,      "genre"},
                {   CommentRole,    "comment"},
                {    LengthRole,     "length"},
                {  ChannelsRole,   "channels"},
                {   BitRateRole,    "bitrate"},
                {SampleRateRole, "samplerate"},
                {      YearRole,       "year"},
                {       UriRole,        "uri"},
                {  AlbumArtRole,   "albumArt"},
            };
        }

    public slots:
        bool setProvider(QSharedPointer<IPlaylistProvider> provider);
        Q_INVOKABLE bool removeSongs(int index, int count = 1);
        Q_INVOKABLE bool addSong(const SongMetadata &song);
        Q_INVOKABLE bool addSongs(const QList<SongMetadata> &songs);
        Q_INVOKABLE bool addSong(const QUrl &url);
        Q_INVOKABLE bool addSongs(const QList<QUrl> &urls);
        Q_INVOKABLE bool setPlaylistName(const QString &playlistName);

    private:
        QSharedPointer<IPlaylistProvider> m_provider;
};
