#include <QObject>

#include "IPlaylistProviderFactory.h"
#include "UPLPlaylistProvider.h"

class UPLPlaylistProviderFactory : public IPlaylistProviderFactory
{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID IPlaylistProviderFactory_iid FILE "UPLPlaylistProvider.json")
        Q_INTERFACES(IPlaylistProviderFactory)

    public:
        IPlaylistProvider *create(QObject *parent = nullptr) const override { return new UPLPlaylistProvider(parent); }
        int priority() const override { return 1000; }
};

#include "UPLPlaylistProviderFactory.moc"
