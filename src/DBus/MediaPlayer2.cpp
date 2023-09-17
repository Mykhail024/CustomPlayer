#include <QDBusConnection>
#include <QStringLiteral>
#include <QDBusMessage>

#include "../../build/mprisroot.h"
#include "../../build/mprisplayer.h"

#include "MediaPlayer2.h"
#include "../Widgets/Window.h"

MediaPlayer2::MediaPlayer2(Window *parent)
    : QDBusAbstractAdaptor(parent)
    , window(parent)
    , mpris_root(new MPrisRoot(this))
    , mpris_player(new MPrisPlayer(this))
{
    QDBusConnection connection = QDBusConnection::sessionBus();

    connection.registerService("org.mpris.MediaPlayer2.CustomPlayer");
    connection.registerObject("/org/mpris/MediaPlayer2", this);

    QDBusConnection::sessionBus().connect(
        "org.mpris.MediaPlayer2.CustomPlayer",
        "/org/mpris/MediaPlayer2",
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged",
        this,
        SLOT(OnPropertiesChanged(QString, QVariantMap, QStringList))
    );

	connect(this, &MediaPlayer2::emitPlaybackStatusChanged, &MediaPlayer2::onPlaybackStatusChanged);
	connect(this, &MediaPlayer2::emitMetadataChanged, &MediaPlayer2::onMetadataChanged);
	connect(this, &MediaPlayer2::emitVolumeChanged, &MediaPlayer2::onVolumeChanged);
}

void MediaPlayer2::OnPropertiesChanged(const QString& interface, const QVariantMap& changedProperties, const QStringList& invalidatedProperties)
{
}

void MediaPlayer2::sendPropertiesChanged(const QString &propertyName, const QVariant &propertyValue)
{
	QVariantMap changedProperties;
	changedProperties.insert(propertyName, propertyValue);
	QStringList invalidatedProperties;
	QDBusMessage signal = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
			"org.freedesktop.DBus.Properties", "PropertiesChanged");
	signal.setArguments({
			QVariant::fromValue(QString("org.mpris.MediaPlayer2.CustomPlayer")),
			QVariant::fromValue(changedProperties),
			QVariant::fromValue(invalidatedProperties)
			});
	QDBusConnection::sessionBus().send(signal);
}

void MediaPlayer2::Raise()
{
	window->activateWindow();
}

void MediaPlayer2::Next()
{
	window->Next();
}
void MediaPlayer2::Previous()
{
	window->Prev();
}
void MediaPlayer2::Pause()
{
	window->Pause();
}
void MediaPlayer2::PlayPause()
{
	window->PlayPause();
}
void MediaPlayer2::Stop()
{
	window->Stop();
}
void MediaPlayer2::Play()
{
	window->Resume();
}
QString MediaPlayer2::playbackStatus() const
{
	switch (window->getPlaybackStatus()) {
		case Window::PlayBackStatus::PLAYING:
			return "Playing";
			break;
		case Window::PlayBackStatus::PAUSED:
			return "Paused";
			break;
		case Window::PlayBackStatus::STOPPED:
			return "Stopped";
			break;
	}
}

void MediaPlayer2::onPlaybackStatusChanged()
{
	sendPropertiesChanged("PlaybackStatus", QVariant::fromValue(playbackStatus()));
}

void MediaPlayer2::onMetadataChanged()
{
	sendPropertiesChanged("Metadata", QVariant::fromValue(metadata()));
}

void MediaPlayer2::onVolumeChanged()
{
	sendPropertiesChanged("Volume", QVariant::fromValue(static_cast<double>(volume())));
}

float MediaPlayer2::volume() const
{
	return window->volume();
}

void MediaPlayer2::setVolume(float volume)
{
	window->setVolume(volume * 100);
}

QVariantMap MediaPlayer2::metadata() const
{
	QVariantMap metadataMap;

	auto metadata = window->metadata();

	metadataMap.insert("mpris:trackid", "/org/mpris/MediaPlayer2/TrackList/NoTrack");
	metadataMap.insert("xesam:title", metadata.title);
	metadataMap.insert("xesam:artist", QStringList{metadata.artist});
	metadataMap.insert("xesam:album", metadata.album);
    metadataMap.insert("mpris:artUrl", QVariant::fromValue(window->imageUrl().toDisplayString()));
	metadataMap.insert("xesam:url", metadata.path);

	return metadataMap;
}

