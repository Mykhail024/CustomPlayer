#pragma once

#include <QObject>

#include "IPlaylistProvider.h"

class IPlaylistProviderFactory : public QObject
{
        Q_OBJECT
    public:
        virtual ~IPlaylistProviderFactory() override = default;
        virtual IPlaylistProvider *create(QObject *parent = nullptr) const = 0;
        virtual int priority() const { return 0; };
};

#define IPlaylistProviderFactory_iid "CustomPlayer.PlaylistProviderFactory"
Q_DECLARE_INTERFACE(IPlaylistProviderFactory, IPlaylistProviderFactory_iid)
