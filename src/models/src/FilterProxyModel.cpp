#include "FilterProxyModel.h"

FilterProxyModel::FilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    this->setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void FilterProxyModel::setFilterString(const QString &filter_string)
{
    if (filter_string == m_filterString)
        return;

    beginFilterChange();
    m_filterString = filter_string;
    endFilterChange();
    emit filterStringChanged(filter_string);
}

bool FilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (m_filterString.isEmpty())
        return true;

    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    const QHash<int, QByteArray> roles = roleNames();

    for (auto it = roles.constBegin(); it != roles.constEnd(); ++it) {
        const QVariant data = index.data(it.key());
        if (data.toString().contains(m_filterString, this->filterCaseSensitivity()))
            return true;
    }

    return false;
}
