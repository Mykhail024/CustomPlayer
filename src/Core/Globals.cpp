#include "Config.h"
#include "Playlist/History.h"

#ifdef __linux__
    #include "Audio/PipeWire.h"
#endif

#include "Globals.h"

Globals::Globals()
    : m_saveTimer(new QTimer(this))
    , m_playbackState(STOPPED)
    , m_canNext(false)
    , m_canPrev(false)
    , m_canPlay(false)
    , m_isMuted(false)
{
    Config::init_config();

    m_loopState = Config::getLoopStatus();
    m_volume = Config::getVolume();
    m_shuffleState = Config::getShuffleState();
    m_columns = Config::getColumns();
    m_softwareVolumeControl = Config::getSoftwareVolumeControl();
    m_fadeInTime_Primary = Config::getFadeIn_Primary();
    m_fadeInTime_Secondary = Config::getFadeIn_Secondary();
    m_Forward_Backward_Time = Config::getForward_Backward_Time();
    m_historyCapacity = Config::getHistoryCapacity();

    m_saveTimer->setSingleShot(true);
    connect(m_saveTimer, &QTimer::timeout, this, [&]{
                Config::setVolume(m_volume);
            });

    initAudioServer();
    m_history = new History(this);
}

void Globals::save()
{
    Config::setColumns(m_columns);
    Config::setVolume(m_volume);
    Config::setSoftwareVolumeControl(m_softwareVolumeControl);
    Config::setFadeIn_Primary(m_fadeInTime_Primary);
    Config::setFadeIn_Secondary(m_fadeInTime_Secondary);
    Config::setLoopStatus(m_loopState);
    Config::setShuffleState(m_shuffleState);
    Config::setForward_Backward_Time(m_Forward_Backward_Time);
    Config::setHistoryCapacity(m_historyCapacity);
}

void Globals::startVolumeSaveTimer()
{
	if(m_saveTimer->isActive())
	{
		m_saveTimer->stop();
	}
	m_saveTimer->start(1000);
}

unsigned int Globals::historyCapacity() const
{
    return m_historyCapacity;
}

SONG_METADATA Globals::currentSong() const
{
    return m_currentSong;
}

PLAYBACK_STATE Globals::playbackState() const
{
    return m_playbackState;
}

bool Globals::canNext() const
{
    return m_canNext;
}

bool Globals::canPrev() const
{
    return m_canPrev;
}

bool Globals::canPlay() const
{
    return m_canPlay;
}

bool Globals::loopState() const
{
    return m_loopState;
}

bool Globals::shuffleState() const
{
    return m_shuffleState;
}

float Globals::volume() const
{
    return m_volume;
}

COLLUMNS Globals::columns() const
{
    return m_columns;
}

bool Globals::softwareVolumeControl() const
{
    return m_softwareVolumeControl;
}

int Globals::fadeIn_Primary() const
{
    return m_fadeInTime_Primary;
}

int Globals::fadeIn_Secondary() const
{
    return m_fadeInTime_Secondary;
}

std::pair<int, int> Globals::fadeIn() const
{
    return std::pair<int, int> {m_fadeInTime_Primary, m_fadeInTime_Secondary};
}

unsigned int Globals::forwardTime() const
{
    return m_Forward_Backward_Time;
}

bool Globals::isMuted() const
{
    return m_isMuted;
}

unsigned long int Globals::songPosition() const
{
    return m_songPosition;
}

History* Globals::history()
{
    return m_history;
}
Audio::AudioServer* Globals::audioServer()
{
    return m_audioServer;
}

void Globals::initAudioServer()
{
#ifdef __linux__
    m_audioServer = new Audio::PipeWire();
#endif
}

void Globals::deinitAudioServer()
{
    if(m_audioServer) {
        delete m_audioServer;
        m_audioServer = nullptr;
    }
}

Globals& globals()
{
    static Globals INSTANCE;
    return INSTANCE;
}
