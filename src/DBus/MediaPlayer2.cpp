#include <QDBusConnection>
#include <QStringLiteral>
#include <QDBusMessage>
#include <QDebug>
#include <qlatin1stringview.h>

#include "../../build/mprisroot.h"
#include "../../build/mprisplayer.h"

#include "../Core/EventHandler.h"
#include "../Core/Globals.h"

#include "MediaPlayer2.h"

#include "moc_MediaPlayer2.cpp"

MediaPlayer2::MediaPlayer2() : QDBusAbstractAdaptor(eventHandler())
{
	const QString PLAYER_NAME(QStringLiteral("CustomPlayer"));
	const QString MPRIS2_NANE(QStringLiteral("org.mpris.MediaPlayer2.") + PLAYER_NAME);

	bool success = QDBusConnection::sessionBus().registerService(MPRIS2_NANE);

	if(!success)
	{
		success = QDBusConnection::sessionBus().registerService(MPRIS2_NANE + QLatin1String(".instance") + QString::number(getpid()));
	}

	if (success) {
        mpris_root = new MPrisRoot(this);
        mpris_player = new MPrisPlayer(this);

        QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/mpris/MediaPlayer2"), this, QDBusConnection::ExportAdaptors);

		connect(eventHandler(), &EventHandler::onVolumeChange, this, &MediaPlayer2::onVolumeChanged);
		connect(eventHandler(), &EventHandler::onPlaySong, this, &MediaPlayer2::onMetadataChanged);
		connect(eventHandler(), &EventHandler::onPlaybackStatusChanged, this, &MediaPlayer2::onPlaybackStatusChanged);
		connect(eventHandler(), &EventHandler::onSeek, this, &MediaPlayer2::onPositionChanged);
		connect(eventHandler(), &EventHandler::onLoopStateChange, this, &MediaPlayer2::onLoopChanged);
		connect(eventHandler(), &EventHandler::onShuffleStateChange, this, &MediaPlayer2::onShuffleChanged);
	}
}
MediaPlayer2::~MediaPlayer2()
{
	delete mpris_root;
	delete mpris_player;
}

void MediaPlayer2::sendPropertiesChanged(const QString &propertyName, const QVariant &propertyValue)
{
	QVariantMap changedProperties;
	changedProperties.insert(propertyName, propertyValue);

	QDBusMessage signal = QDBusMessage::createSignal(QStringLiteral("/org/mpris/MediaPlayer2"),
			QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("PropertiesChanged"));

	signal.setArguments({
			QVariant::fromValue(QStringLiteral("org.mpris.MediaPlayer2.CustomPlayer")),
			QVariant::fromValue(changedProperties),
			QVariant::fromValue(QStringList())
			});

	QDBusConnection::sessionBus().send(signal);
}

void MediaPlayer2::Raise()
{
	eventHandler()->emitRise();
}

void MediaPlayer2::Next()
{
	eventHandler()->emitNextSong();
}
void MediaPlayer2::Previous()
{
	eventHandler()->emitPrevSong();
}
void MediaPlayer2::Pause()
{
	eventHandler()->emitPause();
}
void MediaPlayer2::PlayPause()
{
	eventHandler()->emitPlayPause();
}
void MediaPlayer2::Stop()
{
	eventHandler()->emitStop();
}
void MediaPlayer2::Play()
{
	eventHandler()->emitPlay();
}
void MediaPlayer2::OpenFile(const QString &filePath)
{
	eventHandler()->emitPlaySong(filePath);
}

void MediaPlayer2::onPositionChanged(const unsigned long int &time)
{
	sendPropertiesChanged(QStringLiteral("Position"), time * 1e3);
}
void MediaPlayer2::Seek(const long int &time)
{
	eventHandler()->emitSeek(globals()->songPosition() + (time / 1e3));
}
void MediaPlayer2::SetPosition(QDBusObjectPath path, const unsigned long int &time)
{
	eventHandler()->emitSeek(time / 1e3);
}
unsigned long int MediaPlayer2::position()
{
	return static_cast<unsigned long int>((globals()->songPosition()) / 1e3) * 1e6;
}

QString MediaPlayer2::loopStatus() const
{
	if(globals()->loopState())
	{
		return QStringLiteral("Track");
	}
	return QStringLiteral("None");
}
void MediaPlayer2::setLoopStatus(const QString &status)
{
	if(status != QStringLiteral("None"))
	{
		eventHandler()->emitLoopStateChange(true);
	}
	else
	{
		eventHandler()->emitLoopStateChange(false);
	}
	onLoopChanged();
}
bool MediaPlayer2::shuffle()
{
	return globals()->shuffleState();
}
void MediaPlayer2::setShuffle(const bool &state)
{
	eventHandler()->emitShuffleStateChange(state);
	onShuffleChanged();
}
void MediaPlayer2::onLoopChanged()
{
	sendPropertiesChanged(QStringLiteral("LoopStatus"), loopStatus());
}
void MediaPlayer2::onShuffleChanged()
{
	sendPropertiesChanged(QStringLiteral("Shuffle"), shuffle());
}
QString MediaPlayer2::playbackStatus() const
{
	switch (globals()->playbackStatus().state) {
		case PLAYING:
			return QStringLiteral("Playing");
			break;
		case PAUSED:
			return QStringLiteral("Paused");
			break;
		case STOPPED:
			return QStringLiteral("Stopped");
			break;
	}
}

void MediaPlayer2::onPlaybackStatusChanged()
{
	sendPropertiesChanged(QStringLiteral("PlaybackStatus"), QVariant::fromValue(playbackStatus()));
	sendPropertiesChanged(QStringLiteral("CanPlay"), QVariant::fromValue(canPlay()));
	sendPropertiesChanged(QStringLiteral("CanGoNext"), QVariant::fromValue(canGoNext()));
	sendPropertiesChanged(QStringLiteral("CanGoPrevious"), QVariant::fromValue(canGoPrevious()));
}

void MediaPlayer2::onMetadataChanged()
{
	sendPropertiesChanged(QStringLiteral("Metadata"), QVariant::fromValue(metadata()));
}

void MediaPlayer2::onVolumeChanged()
{
	sendPropertiesChanged(QStringLiteral("Volume"), QVariant::fromValue(static_cast<double>(volume())));
}

float MediaPlayer2::volume() const
{
	return globals()->volume();
}
bool MediaPlayer2::canPlay() const
{
	return globals()->playbackStatus().canPlay;
}
bool MediaPlayer2::canGoNext() const
{
	return globals()->playbackStatus().canNext;
}
bool MediaPlayer2::canGoPrevious() const
{
	return globals()->playbackStatus().canPrev;
}
void MediaPlayer2::setVolume(float volume)
{
	eventHandler()->emitVolumeChange(volume);
}

QVariantMap MediaPlayer2::metadata() const
{
	QVariantMap metadataMap;

	auto metadata = globals()->metadata();
	metadataMap.insert(QStringLiteral("mpris:trackid"), QDBusObjectPath(QStringLiteral("/org/mpris/MediaPlayer2/CurrentTrack")));
	metadataMap.insert(QStringLiteral("xesam:title"), metadata.Title);
	metadataMap.insert(QStringLiteral("xesam:artist"), QStringList{metadata.Artist});
	metadataMap.insert(QStringLiteral("xesam:album"), metadata.Album);
	metadataMap.insert(QStringLiteral("xesam:url"), metadata.Path);
	metadataMap.insert(QStringLiteral("mpris:length"), metadata.Length * 1e3);
	metadataMap.insert(QStringLiteral("mpris:artUrl"), metadata.ArtUrl);

	return metadataMap;
}

MediaPlayer2 *dbus_service;

void initDBusService()
{
	dbus_service = new MediaPlayer2();
}
void deinitDBusService()
{
	if(dbus_service)
	{
		delete dbus_service;
	}
}
MediaPlayer2* dbusService()
{
	return dbus_service;
}
