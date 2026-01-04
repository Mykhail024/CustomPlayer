#pragma once

#include <QImage>
#include <QString>
#include <QStringList>

struct SongMetadata
{
        QString title;
        QString album;
        QString genre;
        QString comment;
        QString artist;
        int length;
        int channels;
        int bitrate;
        int sampleRate;
        unsigned int year;
        QString uri;
};

inline QDataStream &operator<<(QDataStream &out, const SongMetadata &metadata)
{
    out << metadata.title << metadata.album << metadata.genre << metadata.comment << metadata.artist << metadata.length
        << metadata.channels << metadata.bitrate << metadata.sampleRate << metadata.year << metadata.uri;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, SongMetadata &metadata)
{
    in >> metadata.title >> metadata.album >> metadata.genre >> metadata.comment >> metadata.artist >> metadata.length >>
        metadata.channels >> metadata.bitrate >> metadata.sampleRate >> metadata.year >> metadata.uri;
    return in;
}

inline bool operator==(const SongMetadata &lhs, const SongMetadata &rhs)
{
    return lhs.title == rhs.title &&
    lhs.album == rhs.album &&
    lhs.genre == rhs.genre &&
    lhs.comment == rhs.comment &&
    lhs.artist == rhs.artist &&
    lhs.length == rhs.length &&
    lhs.channels == rhs.channels &&
    lhs.bitrate == rhs.bitrate &&
    lhs.sampleRate == rhs.sampleRate &&
    lhs.year == rhs.year &&
    lhs.uri == rhs.uri;
}

inline bool operator!=(const SongMetadata &lhs, const SongMetadata &rhs)
{
    return !(lhs == rhs);
}
