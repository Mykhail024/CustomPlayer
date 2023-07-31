#include "Decoder.hpp"
#include <iostream>
#include <mpg123.h>
#include <vector>

namespace Decoders {
	std::optional<std::vector<unsigned char>> decode_mp3(const std::string &filePath)
	{
		mpg123_init();
		mpg123_handle *mp3Handle = mpg123_new(nullptr, nullptr);

		if (mpg123_open(mp3Handle, filePath.c_str()) != MPG123_OK)
		{
			mpg123_delete(mp3Handle);
			std::cerr << "Error open file: " << filePath << std::endl;
			return std::nullopt;
		}

		int channels, encoding;
		long rate;
		mpg123_getformat(mp3Handle, &rate, &channels, &encoding);

		if (encoding != MPG123_ENC_SIGNED_16) {
			std::cerr << "Unsupported encoding: " << encoding << std::endl;
			mpg123_close(mp3Handle);
			mpg123_delete(mp3Handle);
			return std::nullopt;
		}

		const size_t buffer_size = 4096;
		std::vector<unsigned char> buffer(buffer_size);
		std::vector<unsigned char> mp3Data;

		while (true) {
			size_t done;
			int err = mpg123_read(mp3Handle, buffer.data(), buffer_size, &done);

			if (err == MPG123_NEW_FORMAT)
			{
				mpg123_getformat(mp3Handle, &rate, &channels, &encoding);
			}
			else if (err == MPG123_DONE)
			{
				// End of file, break the loop
				break;
			}
			else if (err != MPG123_OK)
			{
				std::cerr << "Error reading MP3 data: " << mpg123_strerror(mp3Handle) << " - " << err << std::endl;
				break;
			}

			if (done == 0) {
				// No more data to read, break the loop
				break;
			}

			// Append the read data to the mp3Data vector
			mp3Data.insert(mp3Data.end(), std::make_move_iterator(buffer.begin()), std::make_move_iterator(buffer.begin() + done));
		}

		mpg123_close(mp3Handle);
		mpg123_delete(mp3Handle);
		mpg123_exit();

		return mp3Data;
	}
} // namespace Decoders
