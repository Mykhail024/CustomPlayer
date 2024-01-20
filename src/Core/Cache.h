#pragma once

#include <QObject>

#include <optional>

#include "Globals.h"

class Cache : public QObject
{
    Q_OBJECT
    public:
        Cache(QObject *parent);
        ~Cache();

        std::optional<SONG_METADATA> getSong(const QString &filePath);

        bool createDatabase();
        QSqlDatabase database();
        bool open();
        void close();

    public slots:
        bool update(const SONG_METADATA &metadata);
        bool update(const std::vector<SONG_METADATA> &metadata);

    private:
        bool updateInternal(const SONG_METADATA &metadata);
        std::optional<SONG_METADATA> getSongInternal(const QString &filePath);
};
