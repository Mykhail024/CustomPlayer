#pragma once

#include <QImage>

#include <string>
#include <cstdint>

namespace TagReaders {
	struct Data
	{
		QString title;
		QString artist;
		QString album;
		unsigned int length;
		uint32_t sampleRate;
		unsigned int channels;
		unsigned int bitRate;
		unsigned int year;
		QString path;
	};

	QImage id3v2_get_image(const std::string& filePath);
	Data id3v2_read(const std::string& filePath);
}
