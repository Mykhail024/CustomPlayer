#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QThread>
#include <QSqlRecord>
#include <QFont>
#include <QFile>
#include <qdatetime.h>

#include "EventHandler.h"
#include "Globals.h"
#include "PlaylistsCache.h"
#include "Log.h"
#include "Tools.h"

#include "PlaylistModel.h"

#include "moc_PlaylistModel.cpp"

PlaylistModel::PlaylistModel(const QString &filePath, PlaylistCache *cache, QObject *parent)
	: QAbstractTableModel(parent), m_cache(cache)
{
	load(filePath);
}
PlaylistModel::~PlaylistModel()
{
}
int PlaylistModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : m_songs.size();
}
int PlaylistModel::columnCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : 6;
}
QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		switch (section) {
			case 0:
				return tr("Title");
			case 1:
				return tr("Artist");
			case 2:
				return tr("Album");
			case 3:
				return tr("Length");
			case 4:
				return tr("MidifiedDate");
			case 5:
				return tr("Year");
		}
	}
	return QVariant();
}
QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
	if(role == Qt::DisplayRole)
	{
		auto col = index.column();
		auto metadata = m_songs[index.row()];

		switch (col) {
			case 0:
				if(metadata.Title.isEmpty())
				{
					return QFileInfo(metadata.Path).baseName();
				}
				return metadata.Title;
			case 1:
				return metadata.Artist;
			case 2:
				return metadata.Album;
			case 3:
				return secondsToMinutes(metadata.Length / 1000);
			case 4:
				return QDateTime::fromSecsSinceEpoch(metadata.ModifiedDate);
			case 5:
				return metadata.Year;
		}
	}
	return QVariant();
}
bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	return false;
}
bool PlaylistModel::insertRows(int position, int rows, const QModelIndex &index)
{
	return false;
}
bool PlaylistModel::removeRows(int position, int rows, const QModelIndex &index)
{
	beginRemoveRows(index, position, position + rows - 1);
	m_songs.erase(m_songs.begin() + position, m_songs.begin() + position + rows);
	endRemoveRows();
	return true;
}
Qt::ItemFlags PlaylistModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
	{
		return Qt::ItemIsEnabled;
	}

	return QAbstractTableModel::flags(index);
}
void PlaylistModel::rename(const QString &name)
{
	m_name = name;
}
bool PlaylistModel::load(const QString &filePath)
{
	QFile file(filePath);
	if(!file.exists())
	{
		Log_Warning(QString("Unable to load playlist, file %1 does not exist").arg(filePath));
		return false;
	}
	if(!file.open(QFile::ReadOnly | QFile::Text))
	{
		Log_Warning(QString("Unable to load playlist %1, %2").arg(filePath).arg(file.errorString()));
		return false;
	}

	std::vector<SONG_METADATA> songs;

	QTextStream stream(&file);
	bool nameFinded = false;
	while(!stream.atEnd())
	{
		QString line = stream.readLine();
		if(!nameFinded && line.contains("!#"))
		{
			int index = line.indexOf("!#");
            if (index != -1 && index + 2 < line.length())
			{
				m_name = line.mid(index + 2);
				nameFinded = true;
				continue;
            }
		}
		if(line.isEmpty()) continue;
		if(!QFile(line).exists())
		{
			Log_Warning(QString("Unable to load file, file %1 does not exist").arg(line));
			continue;
		}
		songs.push_back(m_cache->getCachedSong(line));
	}

	if(nameFinded)
	{
		beginResetModel();
		m_songs = songs;
		m_filePath = filePath;
		endResetModel();
		Log_Info(QString("Playlist %1 (%2) loaded successfully, %3 songs loaded.").arg(m_name).arg(filePath).arg(m_songs.size()));
	}
	else
	{
		Log_Warning("No playlist name in file: " + filePath);
		return false;
	}

	file.close();
	return true;
}
bool PlaylistModel::save(const QString &filePath)
{
	QFile file(filePath);
	if(!file.exists())
	{
		Log_Warning(QString("Unable to save playlist, file %1 does not exist").arg(filePath));
		return false;
	}
	if(!file.open(QFile::WriteOnly | QFile::Text))
	{
		Log_Warning(QString("Unable to save playlist %1, %2").arg(filePath).arg(file.errorString()));
		return false;
	}

	QTextStream stream(&file);

	stream << QString("!#%1\n").arg(m_name);
	for(const auto &song : m_songs)
	{
		stream << song.Path << "\n";
	}

	file.close();
	Log_Info(QString("Playlist %1 (%2) successful saved").arg(m_name).arg(filePath));
	return true;
}
bool PlaylistModel::save()
{
	if(m_filePath.isEmpty())
	{
		Log_Warning("Unable to save playlist, please load or create playlist before saving");
		return false;
	}
	return save(m_filePath);
}

QString PlaylistModel::name() const
{
	return m_name;
}
size_t PlaylistModel::size() const
{
	return this->rowCount();
}
void PlaylistModel::setSongData(const SONG_METADATA &song, const int &index)
{

}
void PlaylistModel::insertSong(const QString &filePath)
{
	beginResetModel();
	m_songs.push_back(m_cache->getCachedSong(filePath));
	endResetModel();
	QFile file(m_filePath);
	if(!file.exists())
	{
		Log_Warning(QString("Unable add songs to playlist, playlist file %1 does not exist").arg(filePath));
		return;
	}
	if(!file.open(QFile::Append | QFile::Text))
	{
		Log_Warning(QString("Unable to add song to playlist %1, %2").arg(filePath).arg(file.errorString()));
		return;
	}
	QTextStream stream(&file);
	stream << filePath << "\n";
	file.close();
}
void PlaylistModel::insertSong(const QString &filePath, const size_t &index)
{

}
SONG_METADATA PlaylistModel::operator[](const size_t &index)
{
    return m_songs[index];
}
