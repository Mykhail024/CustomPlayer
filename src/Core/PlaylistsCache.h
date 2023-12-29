#pragma once

#include <QObject>

#include "Globals.h"

class PlaylistCache : public QObject
{
	Q_OBJECT
	public:
		PlaylistCache(QObject *parent);
		~PlaylistCache();

		SONG_METADATA getCachedSong(const QString &filePath);
		std::vector<SONG_METADATA> getCachedSongs() const;

	public slots:
		void update(const QString &path);
		void update(const QStringList &pathes);
		void updateAll();

	private:
		SONG_METADATA updateInternal(QSqlDatabase &db, const QString &path);
};
