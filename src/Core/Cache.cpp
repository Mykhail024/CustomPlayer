#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "Config.h"
#include "Log.h"

#include "Cache.h"

#include "moc_Cache.cpp"

const QString dbConectionName = QStringLiteral("CachedSongs");
const QString tableName = QStringLiteral("SongsMetadata");
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
                          ")");

Cache::Cache(QObject *parent) : QObject(parent)
{
    if(!createDatabase()) {
        Log_Error("Failed to create cached table, " + database().lastError().text());
    }
}

Cache::~Cache()
{
    if(QSqlDatabase::contains(dbConectionName)) {
        QSqlDatabase::removeDatabase(dbConectionName);
    }
}

bool Cache::createDatabase()
{
    if(QSqlDatabase::contains(dbConectionName)) {
        Log_Warning("Db already created");
        return false;
    }

    auto db = QSqlDatabase::addDatabase("QSQLITE", dbConectionName);
    db.setDatabaseName(Config::getPlaylistsPath() + "/Cached.db");

    if(!db.open()) return false;

    QSqlQuery q(db);
    if(!q.exec(createTableQuery)) {
        db.close();
        return false;
    }

    db.close();
    return true;
}

QSqlDatabase Cache::database()
{
    if(!QSqlDatabase::contains(dbConectionName)) {
        Log_Error("Cached Database not created");
    }
    return QSqlDatabase::database(dbConectionName);
}

bool Cache::open()
{
    if(!QSqlDatabase::contains(dbConectionName)) {
        Log_Warning("Cached database not created");
        return false;
    }
    QSqlDatabase db = database();
    if(db.isOpen()) return true;
    if(!db.open()) {
        Log_Error("Unable to open cached database, " + db.lastError().text());
        return false;
    }
    return true;
}

void Cache::close()
{
    auto db = database();
    if(db.isOpen()) {
        db.close();
    }
}

bool Cache::update(const SONG_METADATA &metadata)
{
    if(!open()) {
        return false;
    }
    auto db = database();
    if(!updateInternal(metadata)) {
        close();
        return false;
    }
    close();

    return true;
}

bool Cache::update(const std::vector<SONG_METADATA> &metadata)
{
    if(!open()) {
        return false;
    }
    auto db = database();
    if(!db.transaction()) {
        Log_Error("Failed to start transaction to cache database");
    }
    for(const auto &m : metadata) {
        auto song = getSongInternal(m.Path);
        if(song.has_value()) {
            QFileInfo newFileInfo(m.Path);
            if(newFileInfo.lastModified().toSecsSinceEpoch() == song.value().ModifiedDate) continue;
        }
        if(!updateInternal(m)) {
            db.rollback();
            close();
            return false;
        }
    }
    if(!db.commit()) {
        db.rollback();
        close();
        Log_Error("Failed to commit transaction to cache database, " + db.lastError().text());
    }
    close();
    return true;
}

bool Cache::updateInternal(const SONG_METADATA &metadata)
{
    QSqlQuery q(database());
    q.prepare("INSERT OR REPLACE INTO " + tableName + " (Path, Title, Artist, Album, Length, ModifiedDate, Year, SampleRate, Channels, BitRate) "
            "VALUES (:Path, :Title, :Artist, :Album, :Length, :ModifiedDate, :Year, :SampleRate, :Channels, :BitRate)");
    q.bindValue(":Path", metadata.Path);
    q.bindValue(":Title", metadata.Title);
    q.bindValue(":Artist", metadata.Artist);
    q.bindValue(":Album", metadata.Album);
    q.bindValue(":Length", metadata.Length);
    q.bindValue(":ModifiedDate", metadata.ModifiedDate);
    q.bindValue(":Year", metadata.Year);
    q.bindValue(":SampleRate", metadata.SampleRate);
    q.bindValue(":Channels", metadata.Channels);
    q.bindValue(":BitRate", metadata.BitRate);
    if(!q.exec()) {
        Log_Warning("Unable to update song in cached database, " + q.lastError().text());
        return false;
    }
    return true;
}

std::optional<SONG_METADATA> Cache::getSongInternal(const QString &filePath)
{
    auto db = database();
    QSqlQuery q(db);
    q.prepare("SELECT * FROM " + tableName + " WHERE Path = :Path");
    q.bindValue(":Path", filePath);

    if(q.exec()) {
        if(q.next()) {
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

            return metadata;
        }
    }
    return std::nullopt;
}

std::optional<SONG_METADATA> Cache::getSong(const QString &filePath)
{
    open();

    auto result = getSongInternal(filePath);

    close();
    return result;
}
