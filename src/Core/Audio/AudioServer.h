#pragma once

#include <string>
#include <QObject>

namespace Audio {
    class AudioServer : public QObject
    {
        Q_OBJECT
        public slots:
            virtual bool play(const QString &filePath) = 0;
            virtual bool pause() = 0;
            virtual bool resume() = 0;
            virtual void goTo(const unsigned int &time) = 0;
            virtual void setVolume(const float &volume) = 0;
            virtual void fadeIn(unsigned int milliseconds) = 0;
            virtual void stop() = 0;
            virtual void stopFadeIn() = 0;
    };
}
