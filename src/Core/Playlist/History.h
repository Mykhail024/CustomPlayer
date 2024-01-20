#pragma once

#include <QList>

class PlaylistView;

class History : public QObject
{
    public:
        History(QObject *parent = nullptr);

        void addEntry(const QPersistentModelIndex &entry);
        QPersistentModelIndex getEntry(int index) const;
        QPersistentModelIndex goEnd();
        QPersistentModelIndex goStart();
        QPersistentModelIndex goForward();
        QPersistentModelIndex goBack();
        QPersistentModelIndex entry();
        int currentIndex() const;
        int size() const;
        void clear();
        void clearFuture();
        void adjustSize();
        bool isEmpty() const;
        bool hasForward() const;
        bool hasBack() const;

    private:
        QList<QPersistentModelIndex> m_entries;
        int m_currentIndex = -1;
};
