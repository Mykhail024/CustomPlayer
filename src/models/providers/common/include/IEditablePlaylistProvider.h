#pragma once

#include <QObject>
#include <QString>

#include "IPlaylistProvider.h"
#include "SongMetadata.h"

class IEditablePlaylistProvider : public IPlaylistProvider
{
        Q_OBJECT
        Q_INTERFACES(IPlaylistProvider)
    public:
        explicit IEditablePlaylistProvider(QObject *parent = nullptr) : IPlaylistProvider(parent) {}
        virtual ~IEditablePlaylistProvider() override = default;

        virtual bool isReadOnly() const override { return false; }

        virtual bool setPlaylistName(const QString &playlistName) = 0;

        virtual bool setSongAt(int index, const SongMetadata &entry) = 0;
        virtual bool addSongs(const QList<SongMetadata> &songs) = 0;
        virtual bool removeSong(int index) = 0;

        virtual bool save(QString uri = QString()) = 0;

    signals:
        void songsAdded(int index, int count);
        void songRemoved(int index);
        void songChanged(int index);
};

Q_DECLARE_INTERFACE(IEditablePlaylistProvider, "CustomPlayer.EditablePlaylistProvider")
