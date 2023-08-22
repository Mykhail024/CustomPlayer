#pragma once

#include <string>
#include <QObject>

namespace Audio {
	class AudioServer : public QObject
	{
		Q_OBJECT
		public:
			virtual bool play(std::string filePath) = 0;
			virtual bool pause() = 0;
			virtual bool resume() = 0;
			virtual void goTo(int time) = 0;
			virtual void setVolume(float volume) = 0;
			virtual float getVolume() const = 0;
			virtual void setLooped(bool is_loop) = 0;
			virtual bool getLoopStatus() = 0;
			virtual void fadeIn(unsigned int milliseconds) = 0;
			virtual void stop() = 0;
		signals:
			void onEndOFile();
			void onCurrentTimeChange(float time);
			void onTotalTimeChange(float time);
	};
}
