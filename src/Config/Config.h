#pragma once

#include <map>
#include <QString>
#include <unordered_map>

namespace Config {
	struct COLLUMNS
	{
		bool Title;
		bool Artist;
		bool Album;
		bool Length;
		bool ModifiedDate;
		bool Year;
	};
	enum AUDIO_SERVERS
	{
		PIPEWIRE,
		PULSEAUDIO,
		JACK,
		ALSA,
		UNKNOWN_SERVER
	};

	void init_config();
	QString getConfigPath();

	AUDIO_SERVERS getAudioServer();
	void setAudioServer(AUDIO_SERVERS server);

	bool getLoopStatus();
	void setLoopStatus(bool value);

	int getVolume();
	void setVolume(int value);

	COLLUMNS getColumns();
	void setColumns(COLLUMNS columns);
}
