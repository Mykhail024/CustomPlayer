#pragma once

#include <QSortFilterProxyModel>

class PlaylistProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    public:
        PlaylistProxyModel(QObject *parent = nullptr);
        void sort(int column, Qt::SortOrder order) override;

    signals:
        void onSortChange(const int &colIndex, const Qt::SortOrder &sortOrder);
};
