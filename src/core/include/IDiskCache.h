#pragma once

#include <QMutex>
#include <QObject>

#include "ICache.h"

class IDiskCache : public ICache
{
    public:
        virtual ~IDiskCache() = default;

        virtual QString cacheDir() const = 0;

    protected:
        IDiskCache() = default;
};
