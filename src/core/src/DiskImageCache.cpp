#include <QCryptographicHash>
#include <QDir>
#include <QMutexLocker>
#include <QStandardPaths>

#include "DiskImageCache.h"
#include "ID3V2Handler.h"
#include "Log.h"

QString DiskImageCache::retrieve(const QString &filePath)
{
    QFileInfo info(filePath);

    if (!info.exists()) {
        Log_Warning(QString("No exists file %1").arg(filePath));
        return QString();
    }

    QDateTime modTime = info.lastModified();
    QString hashKey = generateCacheKey(filePath, modTime);
    QString hashedFilePath = cacheDir() + "/" + hashKey + ".jpg";

    if (QFile::exists(hashedFilePath)) {
        return hashedFilePath;
    }

    QImage image = id3v2_extract_image(filePath);
    if (image.isNull()) {
        return QString();
    }

    QMutexLocker locker(&m_mutex);

    QDir _cacheDir(cacheDir());
    if (!_cacheDir.exists()) {
        _cacheDir.mkpath(".");
    }

    if (!image.save(hashedFilePath, "JPG", 30)) {
        Log_Warning(QString("Failed to save image %1 to cache").arg(hashedFilePath));
        return QString();
    }

    return hashedFilePath;
}

QString DiskImageCache::cacheDir() const { return QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/images"; }
