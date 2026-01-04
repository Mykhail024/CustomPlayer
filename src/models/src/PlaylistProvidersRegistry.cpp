#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>

#include "IEditablePlaylistProvider.h"
#include "IPlaylistProviderFactory.h"
#include "IPlaylistProviderInterface.h"
#include "Log.h"
#include "PlaylistProvidersRegistry.h"
#include "config.h"

PlaylistProvidersRegistry::PlaylistProvidersRegistry() : QAbstractListModel() {}

QStringList PlaylistProvidersRegistry::pluginSearchPaths()
{
    return QStringList({QStringLiteral(PLAYLIST_PROVIDER_PLUGIN_INSTALL_DIR),
                        QCoreApplication::applicationDirPath() + "/plugins/playlist_providers"});
}

bool PlaylistProvidersRegistry::loadProviderPlugin(const QString pluginPath)
{
    QSharedPointer<QPluginLoader> loader = QSharedPointer<QPluginLoader>::create(pluginPath);

    if (!loader->load()) {
        Log_Warning(QString("[PlaylistProvidersRegistry]: Failed to load plugin: %1").arg(loader->errorString()));
        return false;
    }

    QObject *pluginInstance = loader->instance();
    if (!pluginInstance) {
        Log_Warning(QString("[PlaylistProvidersRegistry]: Plugin has no instance: %1").arg(loader->errorString()));
        loader->unload();
        return false;
    }

    auto factory = qobject_cast<IPlaylistProviderFactory *>(pluginInstance);
    if (!factory) {
        Log_Warning(QString("[PlaylistProvidersRegistry]: Plugin does not implement IPlaylistProviderFactory: %1").arg(pluginPath));
        loader->unload();
        return false;
    }

    const QString pluginId = loader->metaData().value("MetaData").toObject().value("PluginId").toString();
    if (pluginId.isEmpty()) {
        Log_Warning("[PlaylistProvidersRegistry]: Plugin has no PluginId in MetaData");
        loader->unload();
        return false;
    }

    const QJsonArray uriFormats = loader->metaData().value("MetaData").toObject().value("UriFormats").toArray();
    if (uriFormats.isEmpty()) {
        Log_Warning("[PlaylistProvidersRegistry]: Plugin has no UriFormats in MetaData or json array is empty");
        loader->unload();
        return false;
    }

    for (const auto &existingProvider : std::as_const(m_providerFactories)) {
        const QString existingPluginId =
            existingProvider.loader->metaData().value("MetaData").toObject().value("PluginId").toString();
        if (existingPluginId == pluginId) {
            Log_Info(
                QString("[PlaylistProvidersRegistry]: Skipping duplicate plugin provider with PluginId: %1.\n\t Files: %2 and %3")
                    .arg(pluginId, pluginPath, existingProvider.loader->fileName()));
            loader->unload();
            return true; // Provider plugin already loaded
        }

        const QJsonArray existingUriFormats =
            existingProvider.loader->metaData().value("MetaData").toObject().value("UriFormats").toArray();
        for (const auto &format : existingUriFormats) {
            if (uriFormats.contains(format)) {
                Log_Warning(QString("[PlaylistProvidersRegistry] Duplicate of playlists formats: %1 in %2 and %3")
                                .arg(format.toString(), existingProvider.loader->fileName(), pluginPath));
            }
        }
    }

    const QString formatName = loader->metaData().value("MetaData").toObject().value("UriFormatName").toString();
    if (formatName.isEmpty()) {
        Log_Warning("[PlaylistProvidersRegistry]: Plugin has no UriFormatName in MetaData");
        loader->unload();
        return false;
    }

    ProviderFactory providerFactory{
        .loader = loader, .factory = factory, .uriFormats = uriFormats.toVariantList(), .uriFormatsName = formatName};

    m_providerFactories.append(providerFactory);

    emit factoriesChanged();

    return true;
}

bool PlaylistProvidersRegistry::loadProviderPlugins(const QStringList pluginPath)
{
    for (const auto &folder : std::as_const(pluginPath)) {
        QDir dir(folder);

        const QStringList files = dir.entryList({"*.so*", "*.dll", "*.dylib"}, QDir::Files);
        for (const QString &file : files) {
            const auto filePath = dir.filePath(file);
            if (!loadProviderPlugin(filePath)) {
                Log_Warning(QString("[PlaylistProvidersRegistry]: failed to load provider plugin: %1").arg(filePath));
            } else {
                Log_Info(QString("[PlaylistProvidersRegistry] Loaded provider from plugin: %1").arg(file));
            }
        }
    }

    std::sort(m_providerFactories.begin(), m_providerFactories.end(),
              [](const ProviderFactory &a, const ProviderFactory &b) { return a.factory->priority() > b.factory->priority(); });

    beginResetModel();
    endResetModel();
    return true;
}

QObject *PlaylistProvidersRegistry::getFactoryUI(int index, QObject *context)
{
    if (index < 0 || index >= m_providerFactories.size())
        return nullptr;

    auto fac = m_providerFactories[index].factory;

    auto *uiFac = qobject_cast<IPlaylistProviderInterface *>(fac);
    return uiFac ? uiFac->providerUi(context) : nullptr;
}

QObject *PlaylistProvidersRegistry::getFactory(int index)
{
    if (index < 0 || index >= m_providerFactories.size())
        return nullptr;

    auto f = m_providerFactories[index].factory.get();
    auto ef = qobject_cast<IEditablePlaylistProvider *>(f);
    if (ef) {
        Log_Info(QString("Editable %1").arg(ef->playlistName()));
        return ef;
    }

    return f;
}

// QObject *PlaylistProvidersRegistry::getFactoryInterface(int index)
// {
//
//     if (index < 0 || index >= m_providerFactories.size())
//         return nullptr;
//
//     auto fac = m_providerFactories[index].factory;
//
//     auto *uiFac = qobject_cast<IPlaylistProviderInterface *>(fac);
//     return uiFac;
// }

int PlaylistProvidersRegistry::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_providerFactories.size();
}

QHash<int, QByteArray> PlaylistProvidersRegistry::roleNames() const
{
    return {
        {FactoryName, "factoryName"},
        {    Factory,  "providerUi"}
    };
}

QVariant PlaylistProvidersRegistry::data(const QModelIndex &index, int role) const
{
    const int row = index.row();

    if (!index.isValid() || row < 0 || row >= m_providerFactories.size())
        return QVariant();

    const auto &p = m_providerFactories[row];

    switch (role) {
    case FactoryName:
        return p.uriFormatsName;
    case Factory:
        return QVariant::fromValue(p.factory);
    }

    return QVariant();
}
