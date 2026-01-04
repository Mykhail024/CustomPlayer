#pragma once

#include <QAbstractListModel>
#include <QPluginLoader>
#include <QQmlListProperty>
#include <QStandardPaths>
#include <QVector>
#include <QtQml/qqmlregistration.h>

#include "IPlaylistProviderFactory.h"

// class IPlaylistProviderFactory;
class PlaylistProvidersRegistry;
class PlaylistModel;

class PlaylistsModel : public QAbstractListModel
{
        QML_ELEMENT
        Q_OBJECT
        Q_PROPERTY(QString playlistsDir READ playlistsDir CONSTANT)
        Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
        Q_PROPERTY(PlaylistModel *currentPlaylist READ currentPlaylist NOTIFY currentPlaylistChanged)
        Q_PROPERTY(PlaylistProvidersRegistry *providersRegistry READ providersRegistry CONSTANT)
    public:
        explicit PlaylistsModel(
            const QString &playlistsDir = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) +
                                          "/CustomPlayer/Playlists",
            QObject *parent = nullptr);
        Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        Q_INVOKABLE QHash<int, QByteArray> roleNames() const override;
        Q_INVOKABLE QString playlistsDir() const { return m_playlistsDir; }

        static QStringList pluginSearchPaths();

        Q_INVOKABLE PlaylistModel *currentPlaylist() const;

        Q_INVOKABLE int setCurrentIndex(int index);
        Q_INVOKABLE int currentIndex() const { return m_currentPlaylistIndex; }

        Q_INVOKABLE void addPlaylist(const IPlaylistProviderFactory &fac);

        Q_INVOKABLE void update();

        enum PlaylistRole {
            NameRole = Qt::UserRole + 1,
            UriRole,
            SongsModelRole
        };

        Q_INVOKABLE PlaylistProvidersRegistry *providersRegistry() const;

    signals:
        void currentIndexChanged(int index);
        void currentIndexAboutToChange(int old_index);
        void currentPlaylistChanged(PlaylistModel *new_model);

    private:
        bool loadPlaylists(const QString playlistsPath);

        QSharedPointer<PlaylistProvidersRegistry> m_providersRegistry;
        QList<QSharedPointer<PlaylistModel>> m_playlists;
        QString m_playlistsDir;
        int m_currentPlaylistIndex = -1;
};
