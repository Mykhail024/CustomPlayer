#pragma once

#include <QDateTime>
#include <QString>

class ICache
{
    public:
        virtual ~ICache() = default;

        virtual QString retrieve(const QString &filePath) = 0;

    protected:
        ICache() = default;

        QString generateCacheKey(const QString &path, const QDateTime &time);
};
