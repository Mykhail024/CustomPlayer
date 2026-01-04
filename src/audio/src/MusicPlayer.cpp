#include <QAudioSink>
#include <QGuiApplication>
#include <QMediaDevices>

#include "FFmpegAudioDevice.h"
#include "Log.h"
#include "MusicPlayer.h"

MusicPlayer::MusicPlayer(QObject *parent) : QObject(parent)
{
    connect(QGuiApplication::instance(), &QGuiApplication::aboutToQuit, this, &MusicPlayer::stop);
    connect(this, &MusicPlayer::mutedChange, this, [&](bool muted) {
        if (!m_sink->isNull())
            m_sink->setVolume(muted ? 0.0 : m_volume);
    });
    setPlaybackState(StoppedState);
}

MusicPlayer::~MusicPlayer() { stop(); }

void MusicPlayer::playFile(const QString &filePath)
{
    setPlaybackState(StoppedState);

    if (!m_sink.isNull()) {
        // m_sink->stop();
        m_sink.reset();
    }

    if (!m_device.isNull()) {
        m_device->close();
    }
    m_device.reset(new FFmpegAudioDevice(filePath));
    m_device->enableRing(m_loop);

    connect(m_device.get(), &FFmpegAudioDevice::positionChange, this, [&](qint64 position) {
        if (m_live)
            emit positionChange(position);
    });
    connect(m_device.get(), &FFmpegAudioDevice::durationChange, this, [&](qint64 duration) {
        if (m_live)
            emit durationChange(duration);
    });

    connect(m_device.get(), &FFmpegAudioDevice::endOfFile, this, &MusicPlayer::handleEndOfFile, Qt::QueuedConnection);
    connect(this, &MusicPlayer::loopChange, m_device.get(), &FFmpegAudioDevice::enableRing);
    connect(m_device.get(), &FFmpegAudioDevice::errorOccurred, this, [](const QString &error) { Log_Warning(error); });

    if (!m_device->open(QIODevice::ReadOnly)) {
        Log_Warning(m_device->errorString());
        return;
    }

    QAudioFormat format = m_device->format();
    QAudioDevice device = QMediaDevices::defaultAudioOutput();
    if (!device.isFormatSupported(format)) {
        Log_Warning("Format not supported");

        format.setSampleRate(qBound(device.minimumSampleRate(), format.sampleRate(), device.maximumSampleRate()));
    }

    m_sink.reset(new QAudioSink(format));
    m_sink->setVolume(m_volume);

    connect(this, &MusicPlayer::volumeChange, m_sink.get(), &QAudioSink::setVolume);

#ifndef NDEBUG
    connect(m_device.get(), &FFmpegAudioDevice::endOfFile, this, [&] { Log_Debug("EOF"); });
#endif

    m_sink->start(m_device.get());

    setPlaybackState(PlayingState);
}

void MusicPlayer::handleEndOfFile()
{
    setPlaybackState(StoppedState);
    emit endOfFile();
}

void MusicPlayer::pause()
{
    if (!m_sink.isNull()) {
        m_sink->suspend();
        setPlaybackState(PausedState);
    }
}

void MusicPlayer::resume()
{
    if (!m_sink.isNull()) {
        m_sink->resume();
        setPlaybackState(PlayingState);
    }
}

void MusicPlayer::stop()
{
    if (!m_sink.isNull()) {
        m_sink->stop();
        m_sink->deleteLater();
        m_sink.reset();
    }
    if (m_device) {
        m_device->close();
        m_device.reset();
    }
    setPlaybackState(StoppedState);
}

void MusicPlayer::seek(qint64 milliseconds)
{
    if (m_device) {
        m_device->blockSignals(true);
        m_device->seek(milliseconds);
        m_device->blockSignals(false);
    }
}

qint64 MusicPlayer::position() const { return m_device ? m_device->position() : 0; }

qint64 MusicPlayer::duration() const { return m_device ? m_device->duration() : 0; }

MusicPlayer::PlaybackState MusicPlayer::playbackState() const { return m_playbackState; }

void MusicPlayer::setPlaybackState(MusicPlayer::PlaybackState state)
{
    m_playbackState = state;
    emit stateChanged(state);
}

bool MusicPlayer::live() const { return m_live; }

bool MusicPlayer::setLive(bool isLive)
{
    bool old = m_live;
    if (old != isLive) {
        m_live = isLive;
        emit isLiveChange(isLive);
    }
    return old;
}

bool MusicPlayer::loop() const { return m_loop; }

bool MusicPlayer::setLoop(bool loop)
{
    bool old = m_loop;
    if (old != loop) {
        m_loop = loop;
        emit loopChange(loop);
    }
    return old;
}

void MusicPlayer::setVolume(qreal volume)
{
    qreal old = m_volume;
    if (old != volume) {
        m_volume = volume;
        if (!m_muted) {
            emit volumeChange(volume);
        }
    }
}

qreal MusicPlayer::volume() const { return m_volume; }

MusicPlayer *MusicPlayer::instance()
{
    static MusicPlayer instance;
    return &instance;
}

bool MusicPlayer::muted() const { return m_muted; }

bool MusicPlayer::setMuted(bool muted)
{
    bool old = m_muted;
    if (old != muted) {
        m_muted = muted;
        emit mutedChange(muted);
    }
    return old;
}

MusicPlayer *MusicPlayer::create(QQmlEngine *engine, QJSEngine *)
{
    auto i = MusicPlayer::instance();
    engine->setObjectOwnership(i, QQmlEngine::CppOwnership);
    return i;
}
