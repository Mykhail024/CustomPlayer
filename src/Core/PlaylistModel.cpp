#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QThread>
#include <QApplication>
#include <QSqlRecord>

#include "Globals.h"
#include "../TagReaders/TagReader.h"
#include "../Convert.cpp"

#include "PlaylistModel.h"

#include "moc_PlaylistModel.cpp"


IndexingThread::IndexingThread(PlaylistModel *playlist, QObject *parent) : QThread(parent), m_playlists(playlist){}
IndexingThread::~IndexingThread()
{
	this->requestInterruption();
	this->wait();
}
void IndexingThread::run()
{
	if(!m_playlists) return;
	qDebug() << "Start indexing";
	auto size = m_playlists->size();
	for(int i = 0; i < size && !isInterruptionRequested(); i++)
	{
		if (!m_playlists) return;
		if(m_playlists->data(m_playlists->index(i, 1)).isNull())
		{
			auto metadata = TagReaders::id3v2_read(m_playlists->data(m_playlists->index(i, 7)).toString());
			m_playlists->setSongData(metadata, i);
		}
	}
}

PlaylistModel::PlaylistModel(const QString &connectionName, const QString &tableName, QObject *parent)
	: QSqlTableModel(parent, QSqlDatabase::database(connectionName))
	, m_connectionName(connectionName)
	, m_name(tableName)
	, playlistIndexingThread(new IndexingThread(this, this))
{
	setTable(tableName);
	setEditStrategy(QSqlTableModel::OnRowChange);
    select();

	connect(this, &QSqlTableModel::rowsInserted, this, &PlaylistModel::fetch);
	fetch();

	m_brush.selectionColor = QApplication::palette().color(QPalette::Highlight);
}
PlaylistModel::~PlaylistModel()
{
	playlistIndexingThread->requestInterruption();
	playlistIndexingThread->wait();
	delete playlistIndexingThread;
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		switch (section) {
			case 0:
				return tr("Id");
			case 1:
				return tr("Title");
			case 2:
				return tr("Artist");
			case 3:
				return tr("Album");
			case 4:
				return tr("Length");
			case 5:
				return tr("MidifiedDate");
			case 6:
				return tr("Year");
			case 7:
				return tr("Path");
			default:
				break;
		}
	}
	return QVariant();
}
QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
	if(role == Qt::DisplayRole)
	{
		if(index.column() == 0)
		{
			return index.row() + 1;
		}
	}
	else if(role == Qt::FontRole)
	{
		if(index.row() == m_brush.row)
		{
			QFont font;
			font.setBold(true);
			return font;
		}
	}
	else if(role == Qt::BackgroundRole)
	{
		if(index.row() == m_brush.row)
		{
			QBrush brush(m_brush.selectionColor);
			brush.setColor(QColor(brush.color().red(), brush.color().green(), brush.color().blue(), (m_brush.opacity ? 255 : 0)));
			return brush;
		}
	}
	return QSqlTableModel::data(index, role);
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
	this->setData(this->index(index, 1), song.Title);
	this->setData(this->index(index, 2), song.Artist);
	this->setData(this->index(index, 3), song.Album);
	this->setData(this->index(index, 4), QVariant::fromValue(Convert::Seconds::toMinutes(song.Length / 1e3)));
	this->setData(this->index(index, 5), QVariant::fromValue(song.ModifiedDate));
	this->setData(this->index(index, 6), QVariant::fromValue(song.Year));
	this->submitAll();
}
void PlaylistModel::insertSong(const QString &filePath)
{
	insertSong(filePath, this->rowCount());
}
void PlaylistModel::insertSong(const QString &filePath, const size_t &index)
{
	this->insertRow(index);
	this->setData(this->index(index, 7), filePath);
	this->submitAll();
}
void PlaylistModel::startIndexing()
{
	playlistIndexingThread->start(QThread::LowestPriority);
}
void PlaylistModel::stopIndexing()
{
	if(playlistIndexingThread->isRunning())
	{
		playlistIndexingThread->requestInterruption();
		playlistIndexingThread->wait();
	}
}
SONG_METADATA PlaylistModel::operator[](const size_t &index)
{
	QString path = this->data(this->index(index, 7)).toString();
    SONG_METADATA data = TagReaders::id3v2_read(path);
    return data;
}
int PlaylistModel::indexFromPath(const QString &filePath)
{
    QModelIndexList indexes = match(index(0, 7), Qt::DisplayRole, filePath, 1, Qt::MatchExactly);

    if (!indexes.isEmpty()) {
        return indexes.first().row();
    }

    return -1;
}

void PlaylistModel::fetch()
{
    if (!this->canFetchMore(QModelIndex{})) return;
    QTimer::singleShot(0, this, [this]{
      if (!this->canFetchMore(QModelIndex{})) return;
      this->fetchMore(QModelIndex());
      fetch();
    });
}
