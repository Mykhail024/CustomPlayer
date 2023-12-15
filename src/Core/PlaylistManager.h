#pragma once

#include <QObject>
#include <QObjectList>
#include <QStringList>

#include "PlaylistModel.h"

class PlaylistManager : public QObject
{
	Q_OBJECT
	public:
		PlaylistManager();
		~PlaylistManager();
		void removePlaylist(const size_t &index);
		void createPlaylist(const QString &name);
		int active() const;
		void setActive(const int &index);
		size_t count();

		PlaylistModel *operator[](const size_t &index);

	private:
		QList<PlaylistModel*> m_playlists;

		int m_active = 0;
		QString globalPlaylistsFile;
		const QString connectionName = "Playlists";
};

void initPlaylistManager();
void deinitPlaylistManager();
PlaylistManager *playlistManager();
