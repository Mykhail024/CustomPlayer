#pragma once

#include <map>
#include <QString>

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
	QString getPlaylistsPath();

	bool getLoopStatus();
	void setLoopStatus(bool value);

	float getVolume();
	void setVolume(const float &value);

	COLLUMNS getColumns();
	void setColumns(COLLUMNS columns);

	bool getShuffleState();
	void setShuffleState(bool state);

	bool getSoftwareVolumeControl();
	void setSoftwareVolumeControl(bool state);

	void setFadeIn_Primary(const int &time);
	int getFadeIn_Primary();
	void setFadeIn_Secondary(const int &time);
	int getFadeIn_Secondary();

	unsigned int getForward_Backward_Time();
	void setForward_Backward_Time(const unsigned int &time);

	bool isSortDescendingOrder();
	void setSortDescendingOrder(const bool &order);
	unsigned int getSortColumn();
	void setSortColumn(const unsigned int &index);
}
