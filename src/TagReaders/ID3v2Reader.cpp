/***************************************************************************************
 * CustomPlayer
 * Copyright (c) 2023 Mykhail024
 *
 * This project uses the TagLib library distributed under the LGPL license.
 * For more information about the license, see the ./licenses/TAGLIB_LICENSE file.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * For the full text of the GNU General Public License, please refer to the LICENSE file.
 ***************************************************************************************/


#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/attachedpictureframe.h>

#include "TagReader.h"

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

		data.title = QString::fromStdString(tag->title().toCString(true));
		data.artist = QString::fromStdString(tag->artist().toCString(true));
		data.album = QString::fromStdString(tag->album().toCString(true));
		data.length = file.audioProperties()->lengthInMilliseconds();
		data.sampleRate = file.audioProperties()->sampleRate();
		data.channels = file.audioProperties()->channels();
		data.bitRate = file.audioProperties()->bitrate();
		data.year = tag->year();
		data.path = QString::fromStdString(filePath);

		return data;
	}
}
