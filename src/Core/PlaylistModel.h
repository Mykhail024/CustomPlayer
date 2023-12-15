#pragma once

#include <QString>
#include <QList>
#include <QObject>
#include <QSqlTableModel>
#include <QThread>
#include <QVariantAnimation>

#include "Globals.h"

class PlaylistModel;

class IndexingThread : public QThread
{
	public:
		IndexingThread(PlaylistModel *playlist, QObject *parent = nullptr);
		~IndexingThread();
		void run() override;

	private:
		PlaylistModel *m_playlists = nullptr;

};

class PlaylistModel : public QSqlTableModel
{
	Q_OBJECT
	public:
		PlaylistModel(const QString &connectionName, const QString &tableName, QObject *parent);
		~PlaylistModel();

		size_t size() const;
		QString name() const;

		void enable_disable_Opacity() { m_brush.opacity = !m_brush.opacity; }
		void setOpacity(bool op) { m_brush.opacity = op; }
		bool opacity() const { return m_brush.opacity; }
		void setBlinkRow(const int &row) { m_brush.row = row; }
		int blinkRow() const { return m_brush.row; }

		SONG_METADATA operator[](const size_t &index);

		void startIndexing();
		void stopIndexing();
		void fetch();
		void setSongData(const SONG_METADATA &song, const int &index);
		void insertSong(const QString &filePath);
		int indexFromPath(const QString &filePath);
		void insertSong(const QString &filePath, const size_t &index);
		QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	private:
		QString m_name;
		QString m_connectionName;

		struct BackgroundBrush{
			QColor selectionColor;
			bool opacity = true;
			int row = -1;
		};
		BackgroundBrush m_brush;

		IndexingThread *playlistIndexingThread;
};
