#pragma once

#include <string>
#include <vector>

#include "../Widgets/TimeSlider.hpp"

namespace AudioServers
{
	static std::vector<int16_t> convertToPcm16(std::vector<unsigned char> data)
	{
		std::vector<int16_t> pcmData;

		// Convert the MP3 data to PCM format.
		for (size_t i = 0; i < data.size(); i += 2) {
			unsigned char byte1 = data[i];
			unsigned char byte2 = data[i + 1];
			auto sample = static_cast<int16_t>((byte2 << 8) | byte1);
			pcmData.push_back(sample);
		}

		return pcmData;
	}
	enum PLAYBACK_STATE
	{
		PLAYING = 0,
		PAUSED = 1,
		STOPPED = 2
	};
	class AudioServer
	{
		public:
			virtual void play(const std::string &filePath) = 0;
			virtual void pause() = 0;
			virtual void stop() = 0;
			virtual void resume() = 0;
			virtual void setVolume(int volume) = 0;
			virtual void goTo(unsigned int second) = 0;
			virtual void setLooping(bool isLooping) = 0;
			virtual void registerTimeSliderCallback(Widgets::TimeSlider *slider) = 0;

			virtual bool isPlaying() const = 0;
			virtual int getVolume() const = 0;
			virtual unsigned int getTotalTime() const = 0;
			virtual PLAYBACK_STATE getPlaybackState() const = 0;
			virtual unsigned int getCurrentTime() const = 0;
	};
}
