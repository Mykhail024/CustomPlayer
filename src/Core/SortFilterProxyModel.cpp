#include "SortFilterProxyModel.h"

#include "moc_SortFilterProxyModel.cpp"

SortFilterProxyModel::SortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent){}
void SortFilterProxyModel::sort(int column, Qt::SortOrder order)
{
	QSortFilterProxyModel::sort(column, order);
	emit onSortChange(column, this->sortOrder());
}
