#pragma once

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QList>

#include "IEditablePlaylistProvider.h"

struct PlaylistFormat;

class UPLPlaylistProvider : public IEditablePlaylistProvider
{
        Q_OBJECT
        Q_INTERFACES(IEditablePlaylistProvider)
    public:
        UPLPlaylistProvider(QObject *parent = nullptr);

        QString playlistName() const override;

        QList<PlaylistFormat> uriFormats() const override;

        QString uri() const override;

        bool setUri(const QString &uri) override;

        int songsCount() const override;

        SongMetadata songAt(int index) const override;

        bool update() override;

        bool setPlaylistName(const QString &playlistName) override;

        bool setSongAt(int index, const SongMetadata &entry) override;

        bool addSongs(const QList<SongMetadata> &songs) override;

        bool removeSong(int index) override;

        bool load(QString filePath = QString()) override;

        bool save(QString filePath = QString()) override;

    private:
        QString m_playlistName;
        QString m_filePath;

        QList<SongMetadata> m_songs;
};
