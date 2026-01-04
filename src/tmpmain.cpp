#ifndef NDEBUG
#include "sanitize.h"
#endif

#include <QAudioSink>
#include <QGuiApplication>
#include <QMediaDevices>
#include <QTimer>
#include <QtEnvironmentVariables>

#include "FFmpegAudioDevice.h"
#include "Log.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    Log_Debug("Debug mode!");

    QAudioSink *audio; // class member
    FFmpegAudioDevice device;
    {
        device.openFile("/home/hong19/Music/Narvent & VØJ - Memory Reboot (Slowed).mp3");

        audio = new QAudioSink(device.format(), &device);
        audio->start(&device);

        FFmpegAudioDevice::connect(&device, &FFmpegAudioDevice::positionChange, [&device] {
            Log_Info(QString::number(device.position() / 1000) + "/" +
                     QString::number(device.duration() / 1000));
        });

        QTimer::singleShot(5000, [&device, &audio] {
            audio->stop();
            delete audio;
            audio = new QAudioSink(device.format(), &device);
            device.close();
            device.openFile("/home/hong19/Music/Narvent & VØJ - Memory Reboot (Slowed).mp3");
            audio->start(&device);
        });
    }

    return app.exec();
}
