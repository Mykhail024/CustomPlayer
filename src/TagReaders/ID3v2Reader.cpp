#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/attachedpictureframe.h>

#include <QImage>

#include "TagReader.hpp"

namespace TagReaders
{
	QImage id3v2_get_image(const std::string& filePath)
	{
		TagLib::MPEG::File file(filePath.c_str());
		if (!file.isValid()) {
			return QImage();
		}

		TagLib::ID3v2::Tag *id3v2Tag = file.ID3v2Tag();
		if (id3v2Tag) {
			TagLib::ID3v2::FrameList frameList = id3v2Tag->frameList("APIC");
			if (!frameList.isEmpty()) {
				TagLib::ID3v2::AttachedPictureFrame *coverFrame = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(frameList.front());
				TagLib::ByteVector imageData = coverFrame->picture();
				QImage image;
				image.loadFromData(reinterpret_cast<const uchar *>(imageData.data()), static_cast<int>(imageData.size()));
				return image;
			}
		}
		delete id3v2Tag;
	return QImage();
	}

	Data id3v2_read(const std::string& filePath)
	{
		Data data;

		TagLib::MPEG::File file(filePath.c_str());
		if (!file.isValid()) {
			return data;
		}

		TagLib::Tag *tag = file.tag();
		if (!tag) {
			return data;
		}

		data.title = tag->title().toCString(true);
		data.artist = tag->artist().toCString(true);
		data.album = tag->album().toCString(true);
		data.length = file.audioProperties()->lengthInMilliseconds();
		data.sampleRate = file.audioProperties()->sampleRate();
		data.channels = file.audioProperties()->channels();
		data.bitRate = file.audioProperties()->bitrate();
		data.year = tag->year();

		return data;
	}
}
