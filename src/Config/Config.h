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

	void init_config();
	QString getConfigPath();

	bool getLoopStatus();
	void setLoopStatus(bool value);

	int getVolume();
	void setVolume(int value);

	COLLUMNS getColumns();
	void setColumns(COLLUMNS columns);

	unsigned int getAutosaveInterval();
	void setAutosaveInterval(unsigned int value);
}
