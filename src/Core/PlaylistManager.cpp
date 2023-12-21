#include <QFile>
#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QApplication>
#include <QSqlQuery>

#include "../Config/Config.h"

#include "PlaylistManager.h"

#include "moc_PlaylistManager.cpp"

PlaylistManager::PlaylistManager() : QObject()
{
	globalPlaylistsFile = Config::getPlaylistsPath() + "/" + "Playlists.db";

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
	db.setDatabaseName(globalPlaylistsFile);

	if(!db.open())
	{
		qCritical() << "Error load playlist database";
		QApplication::exit(-1);
	}

	auto tables = db.tables();
	for(int i = 0 ; i < tables.size(); i++)
	{
		if(tables[i] != "sqlite_sequence")
		{
			PlaylistModel *p = new PlaylistModel(connectionName, tables[i], this);
			m_playlists.push_back(p);
		}
	}
}

int PlaylistManager::active() const
{
	return m_active;
}

void PlaylistManager::setActive(const int &index)
{
	m_active = index;
}

PlaylistManager::~PlaylistManager()
{
	for(auto p : m_playlists)
	{
		delete p;
	}
	QSqlDatabase::database(connectionName).close();
	QSqlDatabase::removeDatabase(connectionName);
}

void PlaylistManager::createPlaylist(const QString &name)
{
	for(const auto p : m_playlists)
	{
		if(p->name() == name)
		{
			return;
		}
	}
	QSqlDatabase db = QSqlDatabase::database(connectionName);
	QSqlQuery q(db);
	q.exec(QString("CREATE TABLE IF NOT EXISTS `%1` (id INTEGER PRIMARY KEY, title TEXT, artist TEXT, album TEXT, length INTEGER, ModifiedDate INTEGER, year INTEGER, path TEXT)").arg(name));

	PlaylistModel *p = new PlaylistModel(connectionName, name, this);
	m_playlists.push_back(p);
	this->setActive(this->count());
}

void PlaylistManager::removePlaylist(const size_t &index)
{
	QSqlDatabase db = QSqlDatabase::database(connectionName);
	QSqlQuery q(db);
	q.exec(QString("DROP TABLE `%1`").arg(m_playlists[index]->name()));
	m_playlists.erase(m_playlists.begin() + index);
}
size_t PlaylistManager::count()
{
	return m_playlists.size();
}
PlaylistModel* PlaylistManager::operator[](const size_t &index)
{
	return m_playlists[index];
}

PlaylistManager *_playlistManager = nullptr;
void initPlaylistManager()
{
	_playlistManager = new PlaylistManager();
}
void deinitPlaylistManager()
{
	if(_playlistManager)
	{
		delete _playlistManager;
		_playlistManager = nullptr;
	}
}
PlaylistManager *playlistManager()
{
	return _playlistManager;
}

