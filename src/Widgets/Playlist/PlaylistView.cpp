#include <QTableView>
#include <QMenu>
#include <QPropertyAnimation>
#include <QHeaderView>

#include "../../Core/Globals.h"
#include "../../Core/EventHandler.h"
#include "../../Core/PlaylistModel.h"
#include "../../Core/SortFilterProxyModel.h"

#include "PlaylistView.h"

#include "moc_PlaylistView.cpp"

PlaylistView::PlaylistView(PlaylistModel *model, QWidget *parent)
	: QTableView(parent)
	, m_model(model)
	, m_proxyModel(new SortFilterProxyModel(this))
{
	m_proxyModel->setSourceModel(m_model);
	this->setModel(m_proxyModel);
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	this->setSortingEnabled(true);
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->setEditTriggers(QAbstractItemView::NoEditTriggers);
	this->verticalHeader()->hide();
	this->verticalHeader()->setDefaultSectionSize(25);
	this->setObjectName("PlaylistView");
	this->setAlternatingRowColors(true);

	setupColumns();

	QHeaderView *horizontalHeader = this->horizontalHeader();
	horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	for (int i = 1; i < m_model->columnCount(); ++i) {
		horizontalHeader->setSectionResizeMode(i, QHeaderView::Stretch);
	}

	connect(eventHandler(), &EventHandler::onPlaylistFind, this, &PlaylistView::find);
	connect(this, &QTableView::doubleClicked, this, &PlaylistView::onSelect);
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
	connect(m_proxyModel, &SortFilterProxyModel::onSortChange, this, [&](const int &column, const Qt::SortOrder &order){
				Config::setSortColumn(m_proxyModel->mapToSource(m_proxyModel->index(0, column)).column());
				Config::setSortDescendingOrder((order == Qt::DescendingOrder) ? true : false);
			});
}

void PlaylistView::setupColumns()
{
	m_proxyModel->setFilterKeyColumn(-1);
	m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

	auto index = m_proxyModel->mapFromSource(m_model->index(0, Config::getSortColumn())).column();
	auto sortOrder = Config::isSortDescendingOrder() ? Qt::DescendingOrder : Qt::AscendingOrder;

	auto columns = globals()->columns();
	if(!columns.Title) this->setColumnHidden(1, true);
	if(!columns.Artist) this->setColumnHidden(2, true);
	if(!columns.Album) this->setColumnHidden(3, true);
	if(!columns.Length) this->setColumnHidden(4, true);
	if(!columns.ModifiedDate) this->setColumnHidden(5, true);
	if(!columns.Year) this->setColumnHidden(6, true);
	this->setColumnHidden(7, true);

	if(this->isColumnHidden(index))
	{
		this->sortByColumn(0, sortOrder);
		Config::setSortColumn(0);
	}
	else
	{
		this->sortByColumn(index, sortOrder);
	}

}

void PlaylistView::find(const QString &text)
{
	m_proxyModel->setFilterFixedString(text);
}

void PlaylistView::onSelect(const QModelIndex &index)
{
	auto sourceIndex = m_proxyModel->mapToSource(index);
	if(sourceIndex.isValid())
	{
		eventHandler()->PlaySong((*m_model)[sourceIndex.row()].Path);
	}
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
	if(event->key() == Qt::Key_Return)
	{
		onSelect(this->selectionModel()->currentIndex());
	}
	QTableView::keyPressEvent(event);
}
