#pragma once

#include <QString>
#include <QList>
#include <QObject>
#include <QAbstractTableModel>

#include "Globals.h"

class PlaylistCache;

class PlaylistModel : public QAbstractTableModel
{
	Q_OBJECT
	public:
		PlaylistModel(const QString &filePath, PlaylistCache *cache, QObject *parent);
		~PlaylistModel();

		size_t size() const;
		QString name() const;
		void rename(const QString &name);
		QString filePath() const { return m_filePath; }
		bool load(const QString &filePath);
		bool save(const QString &filePath);
		bool save();

		SONG_METADATA operator[](const size_t &index);

		void setSongData(const SONG_METADATA &song, const int &index);
		void insertSong(const QString &filePath);
		void insertSong(const QString &filePath, const size_t &index);
		bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

		int rowCount(const QModelIndex &parent = QModelIndex()) const override;
		int columnCount(const QModelIndex &parent = QModelIndex()) const override;

		QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	protected:
		Qt::ItemFlags flags(const QModelIndex &index = QModelIndex()) const override;
		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
		bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	private:
		QString m_name;
		QString m_filePath;
		std::vector<SONG_METADATA> m_songs;

		PlaylistCache *m_cache;
};
