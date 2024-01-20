#pragma once

#include <QWidget>
#include <QSqlTableModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPropertyAnimation>

class PlaylistModel;
class PlaylistProxyModel;

class PlaylistView : public QTableView
{
    Q_OBJECT
    public:
        PlaylistView(PlaylistModel *model, QWidget *parent);

    public slots:
        void setupColumns();
        inline void select(const QModelIndex &index, const QItemSelectionModel::SelectionFlags selection = (QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect));
        void find(const QString &text);
        void onDoubleClick(const QModelIndex &index);
        void goNext();
        void goRand();

        void onDelete();

    protected:
        void keyPressEvent(QKeyEvent *event) override;

    private:
        PlaylistModel *m_model;
        PlaylistProxyModel *m_proxyModel;
};
