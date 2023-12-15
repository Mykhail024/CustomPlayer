#include <QDebug>

#ifdef __linux__
	#include "../Audio/PipeWire.h"
#endif

#include "Globals.h"

Globals::Globals() : m_saveTimer(new QTimer(this))
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

	m_saveTimer->setSingleShot(true);
	connect(m_saveTimer, &QTimer::timeout, this, [=]{
				Config::setVolume(m_volume);
			});

	initAudioServer();
}
void Globals::setColumns(const Config::COLLUMNS &columns)
{
	m_columns = columns;
	Config::setColumns(columns);
}
void Globals::setForward_Backward_Time(const unsigned int &time)
{
	m_Forward_Backward_Time = time;
	Config::setForward_Backward_Time(time);
}
void Globals::startVolumeSaveTimer()
{
	if(m_saveTimer->isActive())
	{
		m_saveTimer->stop();
	}
	m_saveTimer->start(1000);
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
}
void Globals::setSoftwareVolumeControl(const bool &enabled)
{
	m_softwareVolumeControl = enabled;
	Config::setSoftwareVolumeControl(enabled);
}

void Globals::setFadeIn_Primary(const int &time)
{
	m_fadeInTime_Primary = time;
	Config::setFadeIn_Primary(time);
}
void Globals::setFadeIn_Secondary(const int &time)
{
	m_fadeInTime_Secondary = time;
	Config::setFadeIn_Secondary(time);
}

Globals *_globals = nullptr;

void initGlobals()
{
	_globals = new Globals();
}
void deinitGlobals()
{
	if(_globals)
	{
		delete _globals;
		_globals = nullptr;
	}
}
Globals* globals()
{
	return _globals;
}

void Globals::initAudioServer()
{
#ifdef __linux__
	m_audioServer = new Audio::PipeWire();
#endif
}


void Globals::deinitAudioServer()
{
	if(m_audioServer)
	{
		delete m_audioServer;
		m_audioServer = nullptr;
	}
}
