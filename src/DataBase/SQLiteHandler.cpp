#include <QFile>
#include <QDir>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>
#include <vector>
#include <iostream>

#include "SQLiteHandler.h"
#include "../Config/Config.h"
#include "../TagReaders/TagReader.h"

namespace DB {
	SQLiteHandler::SQLiteHandler()
	{

	}

	void SQLiteHandler::Open(const QString& dbPath, bool create_new)
	{
		QString path = Config::getConfigPath() + "/Playlists/" + dbPath;
		if(create_new)
		{
			QFile::remove(path);
		}
		db = new SQLite::Database(path.toStdString(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

		db->exec("CREATE TABLE IF NOT EXISTS Songs ("
				 "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
    			 "Title TEXT,"
    			 "Artist TEXT,"
    			 "Album TEXT,"
    			 "Length INTEGER,"
    			 "Year INTEGER,"
    			 "FilePath TEXT,"
    			 "ModifiedDate BIGINT);");
	}

	std::vector<DB::SONG> SQLiteHandler::ReadSongs()
	{
		std::vector<SONG> songs;

		SQLite::Statement query(*db, "SELECT Title, Artist, Album, Length, Year, FilePath, ModifiedDate FROM Songs");

		while(query.executeStep())
		{
			songs.emplace_back(query.getColumn(0),
							   query.getColumn(1),
							   query.getColumn(2),
							   query.getColumn(3),
							   query.getColumn(4),
							   query.getColumn(5),
							   query.getColumn(6));
		}

		return songs;
	}
	void SQLiteHandler::updateSongs(QString folderPath)
	{
		QDir dir(folderPath);

		QFileInfoList filelist = dir.entryInfoList(QStringList("*.mp3"), QDir::Files, QDir::Time);

		SQLite::Transaction transaction(*db);

		SQLite::Statement query(*db, "INSERT INTO Songs (Title, Artist, Album, Length, Year, FilePath, ModifiedDate) "
                                     "VALUES (?, ?, ?, ?, ?, ?, ?)");
		for(const auto& i : filelist)
		{
			auto id3v2 = TagReaders::id3v2_read(i.filePath().toStdString());
			query.bind(1, id3v2.title);
			query.bind(2, id3v2.artist);
			query.bind(3, id3v2.album);
			query.bind(4, id3v2.length);
			query.bind(5, id3v2.year);
			query.bind(6, i.filePath().toStdString());
			query.bind(7, static_cast<int64_t>(i.lastModified().toSecsSinceEpoch()));

			query.exec();
			query.reset();
		}
		transaction.commit();
	}
}
