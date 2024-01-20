#include <QTableView>
#include <QMenu>
#include <QPropertyAnimation>
#include <QHeaderView>
#include <QRandomGenerator>

#include "Core/Globals.h"
#include "Core/EventHandler.h"

#include "Core/Config.h"
#include "Core/Log.h"

#include "Core/Playlist/PlaylistModel.h"
#include "Core/Playlist/PlaylistProxyModel.h"
#include "Core/Playlist/PlaylistManager.h"
#include "Core/Playlist/History.h"

#include "PlaylistView.h"

#include "moc_PlaylistView.cpp"

PlaylistView::PlaylistView(PlaylistModel *model, QWidget *parent)
    : QTableView(parent)
    , m_model(model)
    , m_proxyModel(new PlaylistProxyModel(this))
{
    m_proxyModel->setSourceModel(m_model);
    this->setModel(m_proxyModel);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setSortingEnabled(true);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->verticalHeader()->setHidden(true);
    this->horizontalHeader()->setHidden(true);
    this->verticalHeader()->setDefaultSectionSize(25);
    this->setObjectName("PlaylistView");
    this->setAlternatingRowColors(true);

    setupColumns();

    QHeaderView *horizontalHeader = this->horizontalHeader();
    horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    for (int i = 2; i < m_model->columnCount(); ++i) {
        horizontalHeader->setSectionResizeMode(i, QHeaderView::Stretch);
    }

    connect(eventHandler(), &EventHandler::onPlaylistFind, this, &PlaylistView::find);
    connect(this, &QTableView::doubleClicked, this, &PlaylistView::onDoubleClick);
    connect(this, &QTableView::customContextMenuRequested, this, [&](const QPoint &pos){
            QMenu contextMenu(this);
            QAction deleteAction("Delete");

#ifdef __linux__
            deleteAction.setIcon(QIcon::fromTheme("edit-delete"));
#else
            deleteAction.setIcon(this->style()->standardIcon(QStyle::SP_TrashIcon));
#endif

            contextMenu.addAction(&deleteAction);
            connect(&deleteAction, &QAction::triggered, this, &PlaylistView::onDelete);

            contextMenu.exec(mapToGlobal(pos));
            });
    connect(m_proxyModel, &PlaylistProxyModel::onSortChange, this, [&](const int &column, const Qt::SortOrder &order){
                Config::setSortColumn(m_proxyModel->mapToSource(m_proxyModel->index(0, column)).column());
                Config::setSortDescendingOrder((order == Qt::DescendingOrder) ? true : false);
            });
    connect(m_model, &PlaylistModel::onUpdateSelection, this, [&](const QPersistentModelIndex &index, QItemSelectionModel::SelectionFlags flags){
                auto proxy_index = m_proxyModel->mapFromSource(index);
                if(proxy_index.isValid())
                {
                    Log_Debug(QString("Selected row: proxy - %1, source - %2").arg(proxy_index.row()).arg(index.row()));
                    this->select(proxy_index, flags);
                }
            });
    connect(m_model, &PlaylistModel::onSelectNext, this, &PlaylistView::goNext);
    connect(m_model, &PlaylistModel::onSelectRandom, this, &PlaylistView::goRand);
}

void PlaylistView::setupColumns()
{
    m_proxyModel->setFilterKeyColumn(-1);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    auto index = m_proxyModel->mapFromSource(m_model->index(0, Config::getSortColumn())).column();
    auto sortOrder = Config::isSortDescendingOrder() ? Qt::DescendingOrder : Qt::AscendingOrder;

    auto columns = globals()->columns();
    if(!columns.Id) this->setColumnHidden(0, true);
    if(!columns.Title) this->setColumnHidden(1, true);
    if(!columns.Artist) this->setColumnHidden(2, true);
    if(!columns.Album) this->setColumnHidden(3, true);
    if(!columns.Length) this->setColumnHidden(4, true);
    if(!columns.ModifiedDate) this->setColumnHidden(5, true);
    if(!columns.Year) this->setColumnHidden(6, true);

    if(this->isColumnHidden(index)) {
        this->sortByColumn(0, sortOrder);
        Config::setSortColumn(0);
    } else {
        this->sortByColumn(index, sortOrder);
    }
}

void PlaylistView::find(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);
}

void PlaylistView::onDelete()
{
    auto selected = this->selectionModel()->selectedRows();
    std::sort(selected.begin(), selected.end(), [](const QModelIndex &a, const QModelIndex &b){ return a.row() > b.row(); });
    for (const auto &row : selected) {
        m_proxyModel->removeRow(row.row());
    }
    m_model->save();
}

void PlaylistView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return) {
        onDoubleClick(this->selectionModel()->currentIndex());
    }
    QTableView::keyPressEvent(event);
}

inline void PlaylistView::select(const QModelIndex &index, const QItemSelectionModel::SelectionFlags selection)
{
    if(index.isValid())
    {
        this->selectionModel()->setCurrentIndex(index, selection);
    }
}

void PlaylistView::onDoubleClick(const QModelIndex &index)
{
    auto sourceIndex = m_proxyModel->mapToSource(index);
    if(sourceIndex.isValid()) {
        eventHandler()->PlayPlaylistEntry(sourceIndex, true);
    }
}

void PlaylistView::goNext()
{
    eventHandler()->FindClear();
    auto index = m_proxyModel->mapFromSource(globals()->history()->entry());
    onDoubleClick(index.siblingAtRow(index.row()+1));
}

void PlaylistView::goRand()
{
    eventHandler()->FindClear();
    onDoubleClick(m_proxyModel->index(QRandomGenerator::global()->bounded(m_proxyModel->rowCount()), 0));
}
