#pragma once

#include <QObject>
#include <QString>

#include "PlaylistFormat.h"
#include "SongMetadata.h"

class IPlaylistProvider : public QObject
{
        Q_OBJECT
    public:
        explicit IPlaylistProvider(QObject *parent = nullptr) : QObject(parent) {}
        virtual ~IPlaylistProvider() override = default;

        virtual QList<PlaylistFormat> uriFormats() const = 0;
        virtual QString uri() const = 0;
        virtual bool setUri(const QString &uri) = 0;

        virtual bool isReadOnly() const { return true; }

        virtual QString playlistName() const = 0;

        virtual int songsCount() const = 0;
        virtual SongMetadata songAt(int index) const = 0;

        virtual bool update() = 0;

        virtual bool load(QString uri = QString()) = 0;

    signals:
        void nameChanged(const QString &name);

        void reset();
};

Q_DECLARE_INTERFACE(IPlaylistProvider, "CustomPlayer.EPlaylistProvider")
