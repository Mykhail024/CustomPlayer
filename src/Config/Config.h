#include <map>
#include <QString>
#include <unordered_map>

namespace Config {

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

	std::vector<std::pair<QString, bool>> getColumns();
	void setColumns(std::vector<std::pair<QString, bool>> columns);
}
