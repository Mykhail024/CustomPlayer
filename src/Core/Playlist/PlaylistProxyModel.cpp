#include "PlaylistProxyModel.h"

#include "moc_PlaylistProxyModel.cpp"

PlaylistProxyModel::PlaylistProxyModel(QObject *parent) : QSortFilterProxyModel(parent){}

void PlaylistProxyModel::sort(int column, Qt::SortOrder order)
{
    QSortFilterProxyModel::sort(column, order);
    emit onSortChange(column, this->sortOrder());
}
