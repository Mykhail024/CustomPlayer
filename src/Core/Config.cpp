#include <QFileInfo>
#include <QtMinMax>
#include <QSettings>

#include "Log.h"

#include "Config.h"

namespace Config {

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CustomPlayer", "Config");

	void init_config()
	{
		//Global
		if(settings.value("lang", "err") == "err")
		{
			settings.setValue("lang", "en");
		}
		if(settings.value("SoftwareVolumeControl", -1) == -1)
		{
			settings.setValue("SoftwareVolumeControl", false);
		}
		if(settings.value("FadeInTime_Primary", -1) == -1)
		{
			settings.setValue("FadeInTime_Primary", 2000);
		}
		if(settings.value("FadeInTime_Secondary", -1) == -1)
		{
			settings.setValue("FadeInTime_Secondary", 500);
		}
		if(settings.value("Forward_Backward_Time", -1) == -1)
		{
			settings.setValue("Forward_Backward_Time", 5);
		}

		//Session
		if(settings.value("Session/isLoop", -1) == -1)
		{
			settings.setValue("Session/isLoop", false);
		}
		if(settings.value("Session/Volume", -1) == -1)
		{
			settings.setValue("Session/Volume", 0.5);
		}
		if(settings.value("Session/ShuffleState", -1) == -1)
		{
			settings.setValue("Session/ShuffleState", false);
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
		if(settings.value("Columns/sortColumn", -1) == -1)
		{
			settings.setValue("Columns/sortColumn", 0);
		}
		if(settings.value("Columns/sortDescendingOrder", -1) == -1)
		{
			settings.setValue("Columns/sortDescendingOrder", false);
		}
	}

	QString getConfigPath()
	{
		return QFileInfo(settings.fileName()).path();
	}

	QString getPlaylistsPath()
	{
		QDir dir(getConfigPath() + "/Playlists");
		if(!dir.exists())
		{
			Log_Warning("No playlists directory, creating...");
			if(!dir.mkpath(dir.absolutePath()))
			{
				Log_Error("Unable to create playlists directory");
			}
			Log_Info("Playlists dir successful created");
		}
		return dir.absolutePath();
	}

	bool getLoopStatus()
	{
		return settings.value("Session/isLoop", false).toBool();
	}

	void setLoopStatus(bool value)
	{
		settings.setValue("Session/isLoop", value);
	}

	float getVolume()
	{
		float value = settings.value("Session/Volume", -1).toFloat();
		if(value == -1)
		{
			Log_Warning("Not Volume value in config, changing to default value: 0.5");
			settings.setValue("Session/Volume", 0.5);
			value = 0.5;
		}
		if(value > 1.0 || value < 0.0)
		{
			Log_Warning(QString("Incorrect volume value in config (%1), changing to default value: 0.5").arg(value));
			settings.setValue("Session/Volume", 0.5);
			value = 0.5;
		}
		return value;
	}
	void setVolume(const float &value)
	{
		settings.setValue("Session/Volume", value);
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
		settings.setValue("Columns/Modified-Date", columns.ModifiedDate);
		settings.setValue("Columns/Year", columns.Year);
	}

	bool getShuffleState()
	{
		return settings.value("Session/ShuffleState", false).toBool();
	}
	void setShuffleState(bool state)
	{
		settings.setValue("Session/ShuffleState", state);
	}

	bool getSoftwareVolumeControl()
	{
		return settings.value("SoftwareVolumeControl", false).toBool();
	}
	void setSoftwareVolumeControl(bool state)
	{
		settings.setValue("SoftwareVolumeControl", state);
	}

	void setFadeIn_Primary(const int &time)
	{
		settings.setValue("FadeInTime_Primary", time);
	}
	int getFadeIn_Primary()
	{
		return settings.value("FadeInTime_Primary", 2000).toInt();
	}
	void setFadeIn_Secondary(const int &time)
	{
		settings.setValue("FadeInTime_Secondary", time);
	}
	int getFadeIn_Secondary()
	{
		return settings.value("FadeInTime_Secondary", 500).toInt();
	}

	unsigned int getForward_Backward_Time()
	{
		return settings.value("Forward_Backward_Time", 5).toUInt();
	}
	void setForward_Backward_Time(const unsigned int &time)
	{
		settings.setValue("Forward_Backward_Time", time);
	}

	bool isSortDescendingOrder()
	{
		return settings.value("Columns/sortDescendingOrder", false).toBool();
	}
	void setSortDescendingOrder(const bool &order)
	{
		settings.setValue("Columns/sortDescendingOrder", order);
	}
	unsigned int getSortColumn()
	{
		return settings.value("Columns/sortColumn", 0).toUInt();
	}
	void setSortColumn(const unsigned int &index)
	{
		settings.setValue("Columns/sortColumn", index);
	}
}

