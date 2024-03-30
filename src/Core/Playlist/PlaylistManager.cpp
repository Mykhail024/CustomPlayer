#include <QFile>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <cctype>

#include "Core/Cache.h"
#include "Core/EventHandler.h"
#include "Core/Config.h"
#include "Core/Log.h"

#include "Core/Playlist/PlaylistModel.h"
#include "Core/Playlist/PlaylistManager.h"

#include "moc_PlaylistManager.cpp"

PlaylistManager::PlaylistManager() : QObject(&eventHandler())
{
    m_cache = new Cache(this);
    QDir pDir(Config::getPlaylistsPath());
    auto files = pDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    for(const auto &file : files) {
        if(QFileInfo(file).suffix().isEmpty()) {
            addPlaylist(pDir.absoluteFilePath(file));
        }
    }
}

int PlaylistManager::active() const
{
    return m_active;
}

void PlaylistManager::setActive(const int &index)
{
    m_active = index;
}

PlaylistManager::~PlaylistManager()
{
    for(auto p : m_playlists) {
        delete p;
    }
}

void PlaylistManager::createPlaylist(const QString &name)
{
    QString path = Config::getPlaylistsPath() + "/" + name;

    int counter = 0;
    while(QFile::exists(path + (counter == 0 ? "" : QString::number(counter)))) {
        counter++;
    }
    if(counter != 0) path += QString::number(counter);

    QFile file(path);

    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        Log_Warning(QString("Error creating playlist %1 (%2), %3").arg(name).arg(path).arg(file.errorString()));
        return;
    }
    QTextStream stream(&file);
    stream << QString("!#%1\n").arg(name);

    file.close();
    Log_Info(QString("Playlist %1 (%2) created").arg(name).arg(path));

    addPlaylist(path);
}

void PlaylistManager::addPlaylist(const QString &filePath)
{
    PlaylistModel *playlist = new PlaylistModel(filePath, m_cache, this);
    m_playlists.push_back(playlist);

    emit eventHandler().onAddPlaylist();
}

void PlaylistManager::renamePlaylist(const size_t &index, const QString &newName)
{
    m_playlists[index]->rename(newName);
    emit eventHandler().onRenamePlaylist(index, newName);
    m_playlists[index]->save();
}

void PlaylistManager::removePlaylist(const size_t &index)
{
    auto p = m_playlists[index];
    m_playlists.erase(m_playlists.begin() + index);

    QFile file(p->filePath());
    if(file.exists()) {
        file.remove();
        Log_Info(QString("Playlist %1 (%2) removed").arg(p->name()).arg(p->filePath()));
    }

    emit eventHandler().onRemovePlaylist();
    delete p;
}

size_t PlaylistManager::count()
{
    return m_playlists.size();
}

PlaylistModel* PlaylistManager::operator[](const size_t &index)
{
    return m_playlists[index];
}

PlaylistManager& playlistManager()
{
    static PlaylistManager INSTANCE;
    return INSTANCE;
}

