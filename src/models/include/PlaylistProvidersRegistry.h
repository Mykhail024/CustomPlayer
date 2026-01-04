#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QPluginLoader>
#include <QQmlComponent>
#include <QtQml/qqmlregistration.h>

class IPlaylistProviderFactory;

struct ProviderFactory
{
        QML_ELEMENT
        QSharedPointer<QPluginLoader> loader;
        QPointer<IPlaylistProviderFactory> factory;
        QVariantList uriFormats;
        QString uriFormatsName;
};

class PlaylistProvidersRegistry : public QAbstractListModel
{
        Q_OBJECT
        QML_ELEMENT
    public:
        PlaylistProvidersRegistry();

        bool loadProviderPlugin(const QString pluginPath);
        bool loadProviderPlugins(const QStringList pluginPath = pluginSearchPaths());
        static QStringList pluginSearchPaths();

        QList<ProviderFactory> factories() const { return m_providerFactories; };

        Q_INVOKABLE QObject *getFactoryUI(int index, QObject *context);
        Q_INVOKABLE QObject *getFactory(int index);
        // Q_INVOKABLE QObject *getFactoryInterface(int index);

        enum PlaylistRole {
            FactoryName = Qt::UserRole + 1,
            Factory
        };

        Q_INVOKABLE QHash<int, QByteArray> roleNames() const override;

        Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    signals:
        void factoriesChanged();

    private:
        QList<ProviderFactory> m_providerFactories;
};
