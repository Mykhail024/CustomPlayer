#pragma once

#include <cstdint>
#include <QImage>
#include <string>

namespace TagReaders {
	struct Data
	{
		std::string title;
		std::string artist;
		std::string album;
		unsigned int length;
		uint32_t sampleRate;
		unsigned int channels;
		unsigned int bitRate;
		unsigned int year;
	};

	QImage id3v2_get_image(const std::string& filePath);
	Data id3v2_read(const std::string& filePath);
}
