#pragma once

#include <QObject>
#include <QQmlComponent>

class IPlaylistProvider;

class IPlaylistProviderInterface
{
    public:
        virtual ~IPlaylistProviderInterface() = default;

        Q_INVOKABLE virtual QObject *providerUi(QObject *qmlContextParent) = 0;
        virtual bool isRequirementsProvided() const = 0;
};

#define IPlaylistProviderInterface_iid "CustomPlayer.PlaylistProviderInterface"
Q_DECLARE_INTERFACE(IPlaylistProviderInterface, IPlaylistProviderInterface_iid)
