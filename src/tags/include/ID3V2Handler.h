#pragma once

#include <QImage>
#include <QString>
#include <QStringList>

struct SongMetadata;

SongMetadata id3v2_read(const QString &filePath);

QImage id3v2_extract_image(const QString &filePath);
