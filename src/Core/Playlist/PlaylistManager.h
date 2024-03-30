#pragma once

#include <QObject>
#include <QObjectList>
#include <QStringList>
#include <QPointer>

class Cache;
class PlaylistModel;

class PlaylistManager : public QObject
{
    Q_OBJECT
    public:
        void removePlaylist(const size_t &index);
        void createPlaylist(const QString &name);
        void addPlaylist(const QString &filePath);
        void renamePlaylist(const size_t &index, const QString &newName);
        int active() const;
        void setActive(const int &index);
        //void setActive(const PlaylistModel &model);
        size_t count();

        PlaylistModel *operator[](const size_t &index);

        PlaylistManager(const PlaylistManager&) = delete;
        PlaylistManager& operator=(const PlaylistManager&) = delete;

    private:
        friend PlaylistManager& playlistManager();
        PlaylistManager();
        ~PlaylistManager();

        QList<PlaylistModel*> m_playlists;
        QPointer<Cache> m_cache;

        int m_active = 0;
};

PlaylistManager& playlistManager();
