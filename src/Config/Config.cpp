#include <cstdlib>
#include <map>
#include <stdexcept>
#include <string>
#include <iostream>

#include <QFileInfo>
#include <QtMinMax>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>
#include <unordered_map>

#include "Config.h"

namespace Config {

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CustomPlayer", "Config");

	void init_config()
	{
		//Global
		if(settings.value("audio_server", "err") == "err")
		{
			settings.setValue("sound_server", "pipewire");
		}
		if(settings.value("lang", "err") == "err")
		{
			settings.setValue("lang", "en");
		}

		//Session
		if(settings.value("Session/isLoop", -1) == -1)
		{
			settings.setValue("Session/isLoop", false);
		}
		if(settings.value("Session/Volume", -1) == -1)
		{
			settings.setValue("Session/Volume", 80);
		}
		if(settings.value("Session/SortBy", -1) == -1)
		{
			settings.setValue("Session/SortBy", "Title");
		}

		//Columns
		if(settings.value("Columns/Title", -1) == -1)
		{
			settings.setValue("Columns/Title", true);
		}
		if(settings.value("Columns/Artist", -1) == -1)
		{
			settings.setValue("Columns/Artist", true);
		}
		if(settings.value("Columns/Album", -1) == -1)
		{
			settings.setValue("Columns/Album", true);
		}
		if(settings.value("Columns/Length", -1) == -1)
		{
			settings.setValue("Columns/Length", true);
		}
		if(settings.value("Columns/Year", -1) == -1)
		{
			settings.setValue("Columns/Year", false);
		}
		if(settings.value("Columns/Modified-Date", -1) == -1)
		{
			settings.setValue("Columns/Modified-Date", false);
		}
	}

	QString getConfigPath()
	{
		return QFileInfo(settings.fileName()).path();
	}

	AUDIO_SERVERS getAudioServer()
	{
		QString value = settings.value("audio_server", "unknown").toString();
		if(value == "pipewire")
		{
			return PIPEWIRE;
		}
		else if (value == "pulseaudio")
		{
			return PULSEAUDIO;
		}
		else if (value == "jack")
		{
			return JACK;
		}
		else if (value == "alsa") {
			return ALSA;
		}
		return UNKNOWN_SERVER;
	}

	void setAudioServer(AUDIO_SERVERS server)
	{
		QString value;
		if(server == PIPEWIRE) value = "pipewire";
		else if (server == PULSEAUDIO) value = "pulseaudio";
		else if (server == JACK) value = "jack";
		else if (server == ALSA) value = "alsa";
		else
		{
			throw std::invalid_argument("Invalid audio server value");
		}

		settings.setValue("audio_server", value);
	}

	bool getLoopStatus()
	{
		return settings.value("Session/isLoop", false).toBool();
	}

	void setLoopStatus(bool value)
	{
		settings.setValue("Session/isLoop", value);
	}

	int getVolume()
	{
		int value = settings.value("Session/Volume", -1).toInt();
		if(value == -1)
		{
			qWarning() << "Not Volume value in config, changing to default value: 80";
			settings.setValue("Session/Volume", 80);
			value = 80;
		}
		return value;
	}
	void setVolume(int value)
	{
		settings.setValue("Session/Volume", qBound(0, value, 100));
	}

	COLLUMNS getColumns()
	{
		COLLUMNS columns;
		columns.Title = settings.value("Columns/Title").toBool();
		columns.Artist = settings.value("Columns/Artist").toBool();
		columns.Album = settings.value("Columns/Album").toBool();
		columns.Length = settings.value("Columns/Length").toBool();
		columns.ModifiedDate = settings.value("Columns/Modified-Date").toBool();
		columns.Year = settings.value("Columns/Year").toBool();

		return columns;
	}
	void setColumns(COLLUMNS columns)
	{
		settings.setValue("Columns/Title", columns.Title);
		settings.setValue("Columns/Artist", columns.Artist);
		settings.setValue("Columns/Album", columns.Album);
		settings.setValue("Columns/Length", columns.Length);
		settings.setValue("Columns/ModifiedDate", columns.ModifiedDate);
		settings.setValue("Columns/Year", columns.Year);
	}
}
