#pragma once

#include <QSortFilterProxyModel>

class SortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
	public:
		SortFilterProxyModel(QObject *parent = nullptr);
		void sort(int column, Qt::SortOrder order) override;

	signals:
		void onSortChange(const int &colIndex, const Qt::SortOrder &sortOrder);
};
