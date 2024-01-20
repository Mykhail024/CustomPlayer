#pragma once

struct SONG_METADATA;

namespace TagReaders {
    QString id3v2_get_image_path(const QString &filePath);
    SONG_METADATA id3v2_read(const QString &filePath);
}
