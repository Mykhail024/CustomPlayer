#pragma once

#include <QMutex>
#include <QObject>

#include "IDiskCache.h"
#include "ISingleton.h"

class DiskImageCache : public QObject, public IDiskCache, public ISingleton<DiskImageCache>
{
        Q_OBJECT
    public:
        QString retrieve(const QString &filePath) override;
        QString cacheDir() const override;

    private:
        QMutex m_mutex;
};
