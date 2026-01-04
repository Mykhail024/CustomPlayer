#include <QCryptographicHash>
#include <QDir>

#include "ICache.h"

QString ICache::generateCacheKey(const QString &filePath, const QDateTime &modTime)
{
    QString composite = filePath + "_" + modTime.toString(Qt::ISODate);
    return QCryptographicHash::hash(composite.toUtf8(), QCryptographicHash::Sha1).toHex();
}
