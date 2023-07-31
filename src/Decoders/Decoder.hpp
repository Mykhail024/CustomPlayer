#pragma once

#include <vector>
#include <optional>

#include <QString>
#include <QImage>

namespace Decoders {
	std::optional<std::vector<unsigned char>> decode_mp3(const std::string &filePath);
}
