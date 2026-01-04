#pragma once

#include <QAudio>
#include <QAudioDecoder>
#include <QAudioSink>
#include <QMediaPlayer>
#include <QObject>
#include <qtypes.h>

class FFmpegAudioDevice;

class MusicPlayer : public QObject
{
        Q_OBJECT
        Q_PROPERTY(qint64 position READ position WRITE seek NOTIFY positionChange)
        Q_PROPERTY(qint64 duration READ duration NOTIFY durationChange)
        Q_PROPERTY(bool live READ live WRITE setLive NOTIFY isLiveChange)
        Q_PROPERTY(bool loop READ loop WRITE setLoop NOTIFY loopChange)
        Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChange)
        Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChange)
        Q_PROPERTY(PlaybackState playbackState READ playbackState NOTIFY stateChanged)
    public:
        static MusicPlayer *instance();
        static MusicPlayer *create(QQmlEngine *, QJSEngine *);
        ~MusicPlayer();

        enum PlaybackState {
            StoppedState,
            PlayingState,
            PausedState
        };
        Q_ENUM(PlaybackState)

        Q_INVOKABLE qint64 position() const;
        Q_INVOKABLE qint64 duration() const;
        Q_INVOKABLE bool live() const;
        Q_INVOKABLE MusicPlayer::PlaybackState playbackState() const;
        Q_INVOKABLE bool loop() const;
        Q_INVOKABLE qreal volume() const;
        Q_INVOKABLE bool muted() const;

    public slots:
        Q_INVOKABLE void playFile(const QString &filePath);
        Q_INVOKABLE void resume();
        Q_INVOKABLE void pause();
        Q_INVOKABLE void stop();
        Q_INVOKABLE void seek(qint64 miliseconds);
        Q_INVOKABLE bool setLive(bool isLive);
        Q_INVOKABLE bool setLoop(bool loop);
        Q_INVOKABLE void setVolume(qreal volume);
        Q_INVOKABLE bool setMuted(bool muted);

    signals:
        void positionChange(qint64 ms);
        void durationChange(qint64 ms);
        void isLiveChange(bool isLive);
        void loopChange(bool loop);
        void stateChanged(MusicPlayer::PlaybackState state);
        void endOfFile();
        void errorOccurred(const QString &message);
        void volumeChange(float volume);
        void mutedChange(bool muted);

    private slots:
        void handleEndOfFile();

    private:
        explicit MusicPlayer(QObject *parent = nullptr);
        void setPlaybackState(PlaybackState state);

        QScopedPointer<FFmpegAudioDevice> m_device;
        QScopedPointer<QAudioSink> m_sink;

        bool m_live = true;
        bool m_loop = false;
        bool m_muted = false;
        qreal m_volume = 0.5;
        PlaybackState m_playbackState = StoppedState;
};
