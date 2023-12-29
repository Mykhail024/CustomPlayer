
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "Config.h"
#include "Log.h"
#include "TagReaders/TagReader.h"

#include "PlaylistsCache.h"

#include "moc_PlaylistsCache.cpp"

const QString dbConectionName = QStringLiteral("CachedSongs");
const QString createTableQuery = QStringLiteral("CREATE TABLE IF NOT EXISTS SongsMetadata ("
                          "Path TEXT UNIQUE, "
                          "Title TEXT, "
                          "Artist TEXT, "
                          "Album TEXT, "
                          "Length INTEGER, "
                          "ModifiedDate INTEGER, "
                          "Year INTEGER, "
                          "SampleRate INTEGER, "
                          "Channels INTEGER, "
                          "BitRate INTEGER"
                          ");");

PlaylistCache::PlaylistCache(QObject *parent) : QObject(parent)
{
	if(QSqlDatabase::contains(dbConectionName))
	{
		Log_Warning("Db already created");
		return;
	}

	auto db = QSqlDatabase::addDatabase("QSQLITE", dbConectionName);
	db.setDatabaseName(Config::getPlaylistsPath() + "/Cached.db");
	if(!db.open())
	{
		QSqlDatabase::removeDatabase(dbConectionName);
		Log_Error("Unable to open cached database, " + db.lastError().text());
	}
	QSqlQuery q(db);
	if(!q.exec(createTableQuery))
	{
		db.close();
		Log_Error("Failed to create cached table, " + q.lastError().text());
	}
	db.close();
}
PlaylistCache::~PlaylistCache()
{
	if(QSqlDatabase::contains(dbConectionName))
	{
		QSqlDatabase::removeDatabase(dbConectionName);
	}
}

void PlaylistCache::update(const QString &path)
{
	QSqlDatabase db;
	if(QSqlDatabase::contains(dbConectionName))
	{
		db = QSqlDatabase::database(dbConectionName);
		if(!db.open())
		{
			db.close();
			Log_Error("Unable to open cached database, " + db.lastError().text());
		}
	}
	else
	{
		Log_Error("Cached database not created");
	}

	updateInternal(db, path);

	db.close();
}
void PlaylistCache::update(const QStringList &pathes)
{
	QSqlDatabase db;
	if(QSqlDatabase::contains(dbConectionName))
	{
		db = QSqlDatabase::database(dbConectionName);
		if(!db.open())
		{
			db.close();
			Log_Error("Unable to open cached database, " + db.lastError().text());
		}
	}
	else
	{
		Log_Error("Cached database not created");
	}

	if(!db.transaction())
	{
		db.close();
		Log_Error("Failed to start transaction for update database rows");
	}

	for(const auto path : pathes)
	{
		updateInternal(db, path);
	}

	if(!db.commit())
	{
		Log_Warning("Failed to commit transaction");
		if(!db.rollback())
		{
			Log_Warning("Failed to rollback failed transaction");
		}
	}

	db.close();
}
void PlaylistCache::updateAll()
{
	QSqlDatabase db;
	if(QSqlDatabase::contains(dbConectionName))
	{
		db = QSqlDatabase::database(dbConectionName);
		if(!db.open())
		{
			db.close();
			Log_Error("Unable to open cached database, " + db.lastError().text());
		}
	}
	else
	{
		Log_Error("Cached database not created");
	}

	QSqlQuery query(db);
	if(!query.exec("SELECT Path, ModifiedDate FROM SongsMetadata"))
	{
		db.close();
		Log_Error("Failed to fetch data from SongMetadata table: " + query.lastError().text());
	}

	while(query.next())
	{
		QString path = query.value(0).toString();
		qint64 dbModifiedDate = query.value(1).toLongLong();

		QFileInfo fileInfo(path);
		if(!fileInfo.exists())
		{
			Log_Warning("File does not exist: " + path);
			continue;
		}

		if(dbModifiedDate < fileInfo.lastModified().toSecsSinceEpoch())
		{
			updateInternal(db, path);
		}
	}
	db.close();
}
SONG_METADATA PlaylistCache::updateInternal(QSqlDatabase &db, const QString &path)
{
	auto metadata = TagReaders::id3v2_read(path);
	QSqlQuery q(db);
	q.prepare("INSERT OR REPLACE INTO SongsMetadata (Path, Title, Artist, Album, Length, ModifiedDate, Year, SampleRate, Channels, BitRate) "
			"VALUES (:Path, :Title, :Artist, :Album, :Length, :ModifiedDate, :Year, :SampleRate, :Channels, :BitRate)");
	q.bindValue(":Path", path);
	q.bindValue(":Title", metadata.Title);
	q.bindValue(":Artist", metadata.Artist);
	q.bindValue(":Album", metadata.Album);
	q.bindValue(":Length", metadata.Length);
	q.bindValue(":ModifiedDate", metadata.ModifiedDate);
	q.bindValue(":Year", metadata.Year);
	q.bindValue(":SampleRate", metadata.SampleRate);
	q.bindValue(":Channels", metadata.Channels);
	q.bindValue(":BitRate", metadata.BitRate);
	if(!q.exec())
	{
		Log_Warning("Unable to update song in cached database, " + q.lastError().text());
	}
	return metadata;
}

SONG_METADATA PlaylistCache::getCachedSong(const QString &filePath)
{
	SONG_METADATA metadata;

	QSqlDatabase db;
	if(QSqlDatabase::contains(dbConectionName))
	{
		db = QSqlDatabase::database(dbConectionName);
		if(!db.open())
		{
			Log_Error("Unable to open cached database, " + db.lastError().text());
		}
	}
	else
	{
		Log_Error("Cached database not created");
	}

	QSqlQuery q(db);
	q.prepare("SELECT * FROM SongsMetadata WHERE Path = :Path");
	q.bindValue(":Path", filePath);

	if(q.exec())
	{
		if(q.next())
		{
			metadata.Path = q.value(0).toString();
			metadata.Title = q.value(1).toString();
			metadata.Artist = q.value(2).toString();
			metadata.Album = q.value(3).toString();
			metadata.Length = q.value(4).toLongLong();
			metadata.ModifiedDate = q.value(5).toLongLong();
			metadata.Year = q.value(6).toInt();
			metadata.SampleRate = q.value(7).toInt();
			metadata.Channels = q.value(8).toInt();
			metadata.BitRate = q.value(9).toInt();
		}
		else
		{
			metadata = updateInternal(db, filePath);
		}
	}
	else
	{
		Log_Warning("Failed to fetch song metadata for path: " + filePath + ", " + q.lastError().text());
	}

	db.close();

	return metadata;
}
std::vector<SONG_METADATA> PlaylistCache::getCachedSongs() const
{
	std::vector<SONG_METADATA> metadataVector;

	QSqlDatabase db;
	if(QSqlDatabase::contains(dbConectionName))
	{
		db = QSqlDatabase::database(dbConectionName);
		if(!db.open())
		{
			Log_Error("Unable to open cached database, " + db.lastError().text());
		}
	}
	else
	{
		Log_Error("Cached database not created");
	}

	QSqlQuery q(db);
	q.prepare("SELECT * FROM SongMetadata");

	if(q.exec())
	{
		while(q.next())
		{
			SONG_METADATA metadata;
			metadata.Path = q.value(0).toString();
			metadata.Title = q.value(1).toString();
			metadata.Artist = q.value(2).toString();
			metadata.Album = q.value(3).toString();
			metadata.Length = q.value(4).toLongLong();
			metadata.ModifiedDate = q.value(5).toLongLong();
			metadata.Year = q.value(6).toInt();
			metadata.SampleRate = q.value(7).toInt();
			metadata.Channels = q.value(8).toInt();
			metadata.BitRate = q.value(9).toInt();

			metadataVector.push_back(metadata);
		}
	}
	else
	{
		Log_Warning("Failed to fetch songs metadata forom cached database, " + q.lastError().text());
	}
	return metadataVector;
}
