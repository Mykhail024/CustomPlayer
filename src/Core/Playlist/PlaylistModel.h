#pragma once

#include <QString>
#include <QList>
#include <QObject>
#include <QFutureWatcher>
#include <QAbstractTableModel>

#include "Core/Globals.h"

class Cache;

class PlaylistModel : public QAbstractTableModel
{
    Q_OBJECT
    public:
        PlaylistModel(const QString &filePath, Cache *cache, QObject *parent);
        ~PlaylistModel();

        size_t size() const;
        QString name() const;
        QString filePath() const { return m_filePath; }

        SONG_METADATA getSongMetadata(const size_t &index) const;

        void insertSong(const QString &filePath);
        void insertSong(const QString &filePath, const size_t &index);
        bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    public slots:
        void startIndexing();
        void stopIndexing();

        bool load(const QString &filePath);
        bool save(const QString &filePath);
        void rename(const QString &name);
        bool save();

    signals:
        void onUpdateSelection(const QPersistentModelIndex &index, QItemSelectionModel::SelectionFlags flags) const;
        void onSelectNext() const;
        void onSelectRandom() const;
    protected:
        Qt::ItemFlags flags(const QModelIndex &index = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    private:
        QModelIndex m_active;
        QString m_name;
        QString m_filePath;

        QFutureWatcher<void> *m_indexingWatcher;

        std::vector<SONG_METADATA> m_songs;

        Cache *m_cache;

        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override { return false; };
        bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override { return false; };
};
