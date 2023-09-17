#include <QFileInfo>
#include <QtMinMax>
#include <QSettings>
#include <QCoreApplication>

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
		if(settings.value("Session/AutosaveInterval", -1) == -1)
		{
			settings.setValue("Session/AutosaveInterval", 5);
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

	unsigned int getAutosaveInterval()
	{
		return settings.value("Session/AutosaveInterval", 0).toUInt();
	}
	void setAutosaveInterval(unsigned int value)
	{
		settings.setValue("Session/AutosaveInterval", value);
	}
}
