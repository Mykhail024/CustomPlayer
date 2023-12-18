#include <QTableView>
#include <QMenu>
#include <QPropertyAnimation>
#include <QHeaderView>
#include <qpropertyanimation.h>

#include "../../Core/Globals.h"
#include "../../Core/EventHandler.h"
#include "../../Core/PlaylistModel.h"

#include "PlaylistView.h"

#include "moc_PlaylistView.cpp"

PlaylistView::PlaylistView(PlaylistModel *model, QWidget *parent)
	: QTableView(parent)
	, m_model(model)
	, m_proxyModel(new QSortFilterProxyModel(this))
	, blinkAnimationTimer(new QTimer(this))
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

	m_proxyModel->setFilterKeyColumn(-1);
	m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

	QHeaderView *horizontalHeader = this->horizontalHeader();
	horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	for (int i = 1; i < m_model->columnCount(); ++i) {
		horizontalHeader->setSectionResizeMode(i, QHeaderView::Stretch);
	}

	setupColumns();

	blinkAnimationTimer->setInterval(750);

	connect(eventHandler(), &EventHandler::onPlaylistFind, this, &PlaylistView::find);
	connect(this, &QTableView::doubleClicked, this, &PlaylistView::onSelect);
	connect(this, &QTableView::customContextMenuRequested, this, [=](const QPoint &pos){
			QMenu contextMenu(this);
			QAction deleteAction("Delete");
			contextMenu.addAction(&deleteAction);
			connect(&deleteAction, &QAction::triggered, this, &PlaylistView::onDelete);

			contextMenu.exec(mapToGlobal(pos));
			});

	connect(blinkAnimationTimer, &QTimer::timeout, this, [=]{
				m_model->enable_disable_Opacity();
				this->viewport()->update();
			});
	connect(eventHandler(), &EventHandler::onPlaybackStatusChanged, this, [=]{
				auto ps = globals()->playbackStatus();
				if(ps.state == PLAYING)
				{
					if(!blinkAnimationTimer->isActive())
					{
						blinkAnimationTimer->start();
					}
				}
				else if(ps.state == PAUSED)
				{
					blinkAnimationTimer->stop();
					m_model->setOpacity(false);
					this->viewport()->update();
				}
				else if(ps.state == STOPPED)
				{
					blinkAnimationTimer->stop();
					m_model->setOpacity(false);
					m_model->setBlinkRow(-1);
					this->viewport()->update();
				}
			});
	connect(eventHandler(), &EventHandler::onPlaySong, this, [=](const QString &path){
				int index = m_model->indexFromPath(path);
				blinkAnimationTimer->stop();
				if(index != -1)
				{
					m_model->setOpacity(true);
					m_model->setBlinkRow(index);
					this->viewport()->update();
					blinkAnimationTimer->start();
				}
				else
				{
					m_model->setBlinkRow(-1);
					m_model->setOpacity(false);
					this->viewport()->update();
				}
			});
}

void PlaylistView::setupColumns()
{
	auto columns = globals()->columns();
	if(!columns.Title) this->setColumnHidden(1, true);
	if(!columns.Artist) this->setColumnHidden(2, true);
	if(!columns.Artist) this->setColumnHidden(3, true);
	if(!columns.Length) this->setColumnHidden(4, true);
	if(!columns.ModifiedDate) this->setColumnHidden(5, true);
	if(!columns.Year) this->setColumnHidden(6, true);
	this->setColumnHidden(7, true);
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
		blinkAnimationTimer->stop();
		m_model->setOpacity(false);

		this->viewport()->update();

		eventHandler()->PlaySong(m_model->index(sourceIndex.row(), 7).data().toString());

		m_model->setBlinkRow(sourceIndex.row());
		m_model->setOpacity(true);
		blinkAnimationTimer->start();

		this->selectionModel()->clearSelection();
	}
}

void PlaylistView::onDelete()
{
	auto selected = this->selectionModel()->selectedRows();
	std::sort(selected.begin(), selected.end(), [](const QModelIndex &a, const QModelIndex &b){ return a.row() > b.row(); });
	for (const auto &row : selected) {
		if(row == m_proxyModel->mapFromSource(m_model->index(m_model->blinkRow(), 0)))
		{
			blinkAnimationTimer->stop();
			m_model->setOpacity(false);
			m_model->setBlinkRow(-1);
			this->viewport()->update();
		}
        m_proxyModel->removeRow(row.row());
		m_model->submitAll();
		m_model->select();
    }
}

void PlaylistView::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Return)
	{
		onSelect(this->selectionModel()->currentIndex());
	}
	QTableView::keyPressEvent(event);
}
