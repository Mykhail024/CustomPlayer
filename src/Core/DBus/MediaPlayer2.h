#pragma once

#include <QCoreApplication>

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

#include "Core/Globals.h"

class MPrisRoot;
class MPrisPlayer;

class Window;

class MediaPlayer2 : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")

    Q_PROPERTY(bool CanRaise READ canRaise)
    Q_PROPERTY(bool CanQuit READ canQuit)
    Q_PROPERTY(bool HasTrackList READ hasTrackList)
    Q_PROPERTY(QString Identity READ identity)
    Q_PROPERTY(QString DesktopEntry READ desktopEntry)
    Q_PROPERTY(QStringList SupportedUriSchemes READ supportedUriSchemes)
    Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes)

    Q_PROPERTY(float Volume READ volume WRITE setVolume)
    Q_PROPERTY(QString PlaybackStatus READ playbackStatus)
    Q_PROPERTY(QString LoopStatus READ loopStatus WRITE setLoopStatus)
    Q_PROPERTY(bool Shuffle READ shuffle WRITE setShuffle)
    Q_PROPERTY(bool CanPlay READ canPlay)
    Q_PROPERTY(bool CanControl READ canControl)
    Q_PROPERTY(bool CanPause READ canPause)
    Q_PROPERTY(bool CanGoNext READ canGoNext)
    Q_PROPERTY(bool CanGoPrevious READ canGoPrevious)
    Q_PROPERTY(unsigned long int Position READ position)
    Q_PROPERTY(QVariantMap Metadata READ metadata)
    Q_PROPERTY(bool CanSeek READ canSeek)

    public slots:
        void sendPropertiesChanged(const QString &propertyName, const QVariant &propertyValue);

        void Raise();
        void Quit() { QCoreApplication::quit(); }
        void Seek(const long int &time);
        void SetPosition(QDBusObjectPath path, const unsigned long int &time);
        /**
         * @return The media player position in microseconds with a precision of seconds.
         */
        unsigned long int position();

        bool canQuit() const { return true; }
        bool canRaise() const { return true; }
        bool hasTrackList() const { return false; }
        QString identity() const { return QStringLiteral("Custom Player"); }
        QString desktopEntry() const { return QStringLiteral("customplayer"); }
        QStringList supportedUriSchemes() const { return {QStringLiteral("file")}; }
        QStringList supportedMimeTypes() const { return {QStringLiteral("audio/mpeg"), QStringLiteral("audio/mp3")}; }

        void Next();
        void Previous();
        void Pause();
        void PlayPause();
        void Stop();
        void Play();
        bool canPlay() const;
        bool canControl() const { return true; }
        bool canGoNext() const;
        bool canGoPrevious() const;
        bool canSeek() const { return true; };
        bool canPause() const {return true; }

        QString loopStatus() const;
        void setLoopStatus(const QString &status);

        bool shuffle();
        void setShuffle(const bool &state);

        QString playbackStatus() const;

        float volume() const;
        void setVolume(float volume);

        void OpenFile(const QString&);

        QVariantMap metadata() const;

        void onPlaybackStateChanged(const PLAYBACK_STATE &state);
        void onMetadataChanged();
        void onVolumeChanged();
        void onPositionChanged(const unsigned long int &time);
        void onLoopChanged();
        void onShuffleChanged();
        void onCanPlayChanged(const bool &canPlay);
        void onCanGoNextChanged(const bool &canNext);
        void onCanGoPreviousChanged(const bool &canPrev);

    private:
        friend MediaPlayer2& dbusService();
        MediaPlayer2();
        ~MediaPlayer2();
        MPrisRoot *mpris_root;
        MPrisPlayer *mpris_player;
};

MediaPlayer2& dbusService();
