#pragma once

#include <QObject>
#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>

class FilterProxyModel : public QSortFilterProxyModel
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(QString filterString READ filterString WRITE setFilterString NOTIFY filterStringChanged)
    public:
        explicit FilterProxyModel(QObject *parent = nullptr);

        QString filterString() const { return m_filterString; }
        void setFilterString(const QString &filterString);

    signals:
        void filterStringChanged(const QString &new_filterString);

    protected:
        bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    private:
        QString m_filterString;
};
