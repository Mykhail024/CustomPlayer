#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QPluginLoader>
#include <QRegularExpression>
#include <QStandardPaths>
#include <qqmllist.h>

#include "IPlaylistProvider.h"
#include "IPlaylistProviderFactory.h"
#include "Log.h"
#include "PlaylistModel.h"
#include "PlaylistProvidersRegistry.h"
#include "PlaylistsModel.h"
#include "config.h"

PlaylistsModel::PlaylistsModel(const QString &playlistsDir, QObject *parent)
    : QAbstractListModel(parent)
    , m_providersRegistry(new PlaylistProvidersRegistry())
    , m_playlistsDir(playlistsDir)
{
    m_providersRegistry->loadProviderPlugins();
    update();
}

void PlaylistsModel::update()
{
    m_playlists.clear();
    loadPlaylists(m_playlistsDir);
}

bool PlaylistsModel::loadPlaylists(const QString playlistsPath)
{
    QDir dir(playlistsPath);
    if (!dir.exists())
        return false;

    const QStringList files = dir.entryList(QDir::Files);
    QList<QSharedPointer<PlaylistModel>> newPlaylists;

    const auto factories = m_providersRegistry->factories();
    for (const auto &factory : factories) {
        if (!factory.factory)
            continue;

        for (const QString &fileName : files) {
            const QString fullPath = dir.absoluteFilePath(fileName);

            for (const QVariant &formatVariant : factory.uriFormats) {
                const QString pattern = formatVariant.toString();
                QRegularExpression re =
                    QRegularExpression::fromWildcard(pattern, Qt::CaseSensitive, QRegularExpression::DefaultWildcardConversion);

                if (!re.isValid())
                    continue;

                if (!re.match(fileName).hasMatch())
                    continue;

                QSharedPointer<IPlaylistProvider> provider(factory.factory->create());
                if (!provider)
                    continue;

                if (!provider->setUri(fullPath))
                    continue;

                if (!provider->load())
                    continue;

                QSharedPointer<PlaylistModel> model = QSharedPointer<PlaylistModel>::create(provider);
                newPlaylists.append(model);
                break;
            }
        }
    }

    if (newPlaylists.isEmpty())
        return false;

    beginResetModel();
    m_playlists += newPlaylists;
    endResetModel();

    return true;
}

void PlaylistsModel::addPlaylist(const IPlaylistProviderFactory &fac)
{
    QSharedPointer<IPlaylistProvider> provider(fac.create(this));
    QSharedPointer<PlaylistModel> model = QSharedPointer<PlaylistModel>::create(provider);
    m_playlists.push_back(model);
}

int PlaylistsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_playlists.size();
}

QVariant PlaylistsModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if (!index.isValid() || row < 0 || row >= m_playlists.size())
        return QVariant();

    const auto p = m_playlists[row];

    switch (role) {
    case NameRole:
        return p->playlistName();
    case UriRole:
        return p->provider()->uri();
    case SongsModelRole:
        return QVariant::fromValue(p);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PlaylistsModel::roleNames() const
{
    return {
        {      NameRole,       "name"},
        {       UriRole,       "path"},
        {SongsModelRole, "songsModel"}
    };
}

int PlaylistsModel::setCurrentIndex(int index)
{
    int old = m_currentPlaylistIndex;
    if (index != old && index >= 0 && index < m_playlists.size()) {
        emit currentIndexAboutToChange(m_currentPlaylistIndex);
        m_currentPlaylistIndex = index;
        emit currentIndexChanged(index);
        emit currentPlaylistChanged(m_playlists[index].get());
    }
    return old;
}

PlaylistModel *PlaylistsModel::currentPlaylist() const
{
    return m_currentPlaylistIndex >= 0 ? m_playlists[m_currentPlaylistIndex].get() : nullptr;
}

PlaylistProvidersRegistry *PlaylistsModel::providersRegistry() const { return m_providersRegistry.get(); }
