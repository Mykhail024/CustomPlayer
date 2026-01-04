#include <taglib/attachedpictureframe.h>
#include <taglib/fileref.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>

#include "ID3V2Handler.h"
#include "SongMetadata.h"

SongMetadata id3v2_read(const QString &filePath)
{
    SongMetadata data;

    TagLib::MPEG::File file(filePath.toStdString().c_str());
    if (!file.isValid()) {
        return data;
    }

    TagLib::Tag *tag = file.tag();
    if (!tag) {
        return data;
    }

    data.title = QString::fromStdString(tag->title().toCString(true));
    data.artist.push_back(QString::fromStdString(tag->artist().toCString(true)));
    data.album = QString::fromStdString(tag->album().toCString(true));
    data.length = file.audioProperties()->lengthInMilliseconds();
    data.sampleRate = file.audioProperties()->sampleRate();
    data.channels = file.audioProperties()->channels();
    data.bitrate = file.audioProperties()->bitrate();
    data.year = tag->year();
    data.uri = filePath;

    return data;
}

QImage id3v2_extract_image(const QString &filePath)
{
    TagLib::MPEG::File file(filePath.toStdString().c_str());
    if (!file.isValid()) {
        return QImage();
    }

    QImage image;

    TagLib::ID3v2::Tag *id3v2Tag = file.ID3v2Tag();
    if (id3v2Tag) {
        TagLib::ID3v2::FrameList frameList = id3v2Tag->frameList("APIC");
        if (!frameList.isEmpty()) {
            TagLib::ID3v2::AttachedPictureFrame *coverFrame = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(frameList.front());
            TagLib::ByteVector imageData = coverFrame->picture();
            image.loadFromData(reinterpret_cast<const uchar *>(imageData.data()), static_cast<int>(imageData.size()));
            return image;
        }
    }

    return QImage();
}
