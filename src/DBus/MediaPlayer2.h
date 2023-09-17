#pragma once

#include <QDBusAbstractAdaptor>
#include <QApplication>

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
	Q_PROPERTY(QString DesktopEntry READ desktopEntry)
	Q_PROPERTY(QString Identity READ identity)
	Q_PROPERTY(QStringList SupportedUriSchemes READ supportedUriSchemes)
	Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes)

	Q_PROPERTY(float Volume READ volume WRITE setVolume)
	Q_PROPERTY(QString PlaybackStatus READ playbackStatus)
	Q_PROPERTY(QString LoopStatus READ loopStatus)
	Q_PROPERTY(bool CanPlay READ canPlay)
	Q_PROPERTY(bool CanControl READ canControl)
	Q_PROPERTY(bool CanPause READ canPause)
	Q_PROPERTY(bool CanGoNext READ canGoNext)
	Q_PROPERTY(bool CanGoPrevious READ canGoPrevious)
	Q_PROPERTY(QVariantMap Metadata READ metadata)

	public:
		MediaPlayer2(Window *parent);

	public slots:
		void OnPropertiesChanged(const QString& interface, const QVariantMap& changedProperties, const QStringList& invalidatedProperties);
		void sendPropertiesChanged(const QString &propertyName, const QVariant &propertyValue);

		void Raise();
		void Quit() { QApplication::quit(); }

		bool canQuit() const { return true; }
		bool canRaise() const { return true; }
		bool hasTrackList() const { return false; }
		QString desktopEntry() const { return "customplayer"; }
		QString identity() const { return "CustomPlayer"; }
		QStringList supportedUriSchemes() const
		{
			return QStringList("file");
		}
		QStringList supportedMimeTypes() const
		{
			return QStringList({"audio/mpeg", "audio/mp3"});
		}

		void Next();
		void Previous();
		void Pause();
		void PlayPause();
		void Stop();
		void Play();
		bool canPlay() const { return true; }
		bool canControl() const { return true; }
		bool canGoNext() const { return true; }
		bool canGoPrevious() const {return true; }
		bool canPause() const {return true; }

		QString playbackStatus() const;

		QString loopStatus() const { return "Track Once"; }
		float volume() const;
		void setVolume(float volume);

		QVariantMap metadata() const;

		void onPlaybackStatusChanged();
		void onMetadataChanged();
		void onVolumeChanged();

	signals:
		void emitPlaybackStatusChanged();
		void emitMetadataChanged();
		void emitVolumeChanged();
	private:
		Window *window;

		MPrisRoot *mpris_root;
		MPrisPlayer *mpris_player;
};
