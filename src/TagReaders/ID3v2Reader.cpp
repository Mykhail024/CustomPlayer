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

#include <QDir>
#include <QImage>

#include "../Core/Globals.h"
#include "TagReader.h"

namespace TagReaders
{
	QString saveImage(const QImage &image)
	{
		QString path;
		QString baseDir;

		if (QDir("/dev/shm").exists())
		{
			baseDir = "/dev/shm/CustomPlayer";
		}
		else
		{
			baseDir = Config::getConfigPath() + "/Images";
		}

		QDir dir(baseDir);
		if (dir.exists())
		{
			dir.removeRecursively();
		}

		dir.mkpath(".");

		qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
		path = dir.filePath("img" + QString::number(currentTime) + ".jpeg");

		bool saved = image.save(path, "jpeg");
		if (saved)
		{
			return path;
		}
		return "";
	}

	QString id3v2_get_image_path(const QString &filePath)
	{
		TagLib::MPEG::File file(filePath.toStdString().c_str());
		if (!file.isValid()) {
			return "";
		}

		QImage image;

		TagLib::ID3v2::Tag *id3v2Tag = file.ID3v2Tag();
		if (id3v2Tag) {
			TagLib::ID3v2::FrameList frameList = id3v2Tag->frameList("APIC");
			if (!frameList.isEmpty()) {
				TagLib::ID3v2::AttachedPictureFrame *coverFrame = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(frameList.front());
				TagLib::ByteVector imageData = coverFrame->picture();
				image.loadFromData(reinterpret_cast<const uchar *>(imageData.data()), static_cast<int>(imageData.size()));
				return saveImage(image);
			}
		}
		return "";
	}

	SONG_METADATA id3v2_read(const QString &filePath)
	{
		SONG_METADATA data;

		TagLib::MPEG::File file(filePath.toStdString().c_str());
		if (!file.isValid()) {
			return data;
		}

		TagLib::Tag *tag = file.tag();
		if (!tag) {
			return data;
		}

		data.Title = QString::fromStdString(tag->title().toCString(true));
		data.Artist = QString::fromStdString(tag->artist().toCString(true));
		data.Album = QString::fromStdString(tag->album().toCString(true));
		data.Length = file.audioProperties()->lengthInMilliseconds();
		data.SampleRate = file.audioProperties()->sampleRate();
		data.Channels = file.audioProperties()->channels();
		data.BitRate = file.audioProperties()->bitrate();
		data.Year = tag->year();
		data.Path = filePath;
		data.ModifiedDate = QFileInfo(filePath).lastModified().toSecsSinceEpoch();

		return data;
	}
}
