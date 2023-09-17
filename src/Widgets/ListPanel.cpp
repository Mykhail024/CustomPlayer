#include <QHeaderView>
#include <QStandardPaths>
#include <QFileInfo>

#include "../Convert.cpp"
#include "ListPanel.h"

static int findColumn(QStandardItemModel *widget,const QString& name)
{
	int result = -1;
	for(int i = 0; i < widget->columnCount(); i++)
	{
		auto headerItem = widget->horizontalHeaderItem(i);
		if(headerItem && headerItem->text() == name)
		{
			result = i;
			break;
		}
	}
	return result;
}

ListPanel::ListPanel(QString dataBase, QWidget *parent) : QTableView(parent), columns(Config::getColumns()), db_path(dataBase)
{
	model = new QStandardItemModel();
	proxyModel = new QSortFilterProxyModel();
	proxyModel->setSourceModel(model);
	proxyModel->setFilterKeyColumn(-1);
	proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	this->setModel(proxyModel);

	db.Open(db_path);
	songs = db.ReadSongs();

	connect(this, &ListPanel::setupColumns, &ListPanel::onSetupColumns);
	connect(this, &ListPanel::setupRows, &ListPanel::onSetupRows);

	this->verticalHeader()->setVisible(false);
	this->verticalHeader()->setDefaultSectionSize(this->height() * 0.06);
	this->setEditTriggers(QAbstractItemView::NoEditTriggers);
	this->setSelectionMode(QAbstractItemView::SingleSelection);
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->setShowGrid(false);
	this->setAlternatingRowColors(true);

	this->setupColumns();
	this->setupRows();

	connect(this, &ListPanel::update, &ListPanel::onUpdate);
	connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ListPanel::Play);
	connect(this, &ListPanel::getSelected, &ListPanel::onGetSelected);
	connect(this, &ListPanel::setSelected, &ListPanel::onSetSelected);
	connect(this, &ListPanel::selectNext, &ListPanel::onSelectNext);
	connect(this, &ListPanel::selectPrev, &ListPanel::onSelectPrev);
	connect(this, &ListPanel::find, &ListPanel::onFind);
}

void ListPanel::onUpdate(QString path)
{
	this->setUpdatesEnabled(false);
	db.Open(db_path, true);
	db.updateSongs(path);
	songs = db.ReadSongs();
	onSetupRows();
	this->setUpdatesEnabled(true);
}

void ListPanel::onSetupColumns()
{
	QStringList headerLabels;
	if(columns.Title) headerLabels << "Title";
	if(columns.Artist) headerLabels << "Artist";
	if(columns.Album) headerLabels << "Album";
	if(columns.Length) headerLabels << "Length";
	if(columns.Year) headerLabels << "Year";
	if(columns.ModifiedDate) headerLabels << "Modified Date";

	model->setColumnCount(headerLabels.size());
	model->setHorizontalHeaderLabels(headerLabels);

	for (const auto& label : headerLabels)
	{
		int column = findColumn(model, label);
		if (column != -1)
		{
			if (label == "Title")
			{
				this->horizontalHeader()->setSectionResizeMode(column, QHeaderView::ResizeToContents);
			}
			else
			{
				this->horizontalHeader()->setSectionResizeMode(column, QHeaderView::Stretch);
			}
		}
	}
}

void ListPanel::onSetupRows()
{
	int rowsCount = songs.size();
	model->setRowCount(rowsCount);
	for(int row = 0; row < rowsCount; row++)
	{
		int column = 0;
		if(columns.Title)
		{
			if(!songs[row].Title.empty())
			{
				model->setItem(row, column, new QStandardItem(QString::fromStdString(songs[row].Title)));
			}
			else
			{
				QString name = QFileInfo(QString::fromStdString(songs[row].filePath)).baseName();
				model->setItem(row, column, new QStandardItem(name));
			}
			column++;
		}
		if(columns.Artist)
		{
			model->setItem(row, column++, new QStandardItem(QString::fromStdString(songs[row].Artist)));
		}
		if(columns.Album)
		{
			model->setItem(row, column++, new QStandardItem(QString::fromStdString(songs[row].Album)));
		}
		if(columns.Length)
		{
			model->setItem(row, column++, new QStandardItem(Convert::Seconds::toMinutes(songs[row].length / 1000)));
		}
		if(columns.Year)
		{
			model->setItem(row, column++, new QStandardItem(QString::number(songs[row].year)));
		}
		if(columns.ModifiedDate)
		{
			model->setItem(row, column++, new QStandardItem(QString::number(songs[row].modifiedDate)));
		}
	}
}

void ListPanel::Play(const QItemSelection &selected, const QItemSelection &deselected)
{
	if(while_sort)
	{
		return;
	}
	Q_UNUSED(deselected);

	QModelIndexList selectedIndexes = proxyModel->mapSelectionToSource(selected).indexes();

    if (!selectedIndexes.isEmpty()) {
        selected_index = selectedIndexes.first();
		int selectedRow = selected_index.row();

        if (selectedRow >= 0 && selectedRow < songs.size()) {
			m_trackNumber = selectedRow;
            emit onPlay(songs[selectedRow].filePath);
        }
	}
}

void ListPanel::onFind(const QString &text)
{
	while_sort = true;
	proxyModel->setFilterFixedString(text);

	if(selected_index.isValid())
	{
		auto index = proxyModel->mapFromSource(selected_index);
		if(index.isValid())
		{
			this->setCurrentIndex(index);
		}
		else
		{
			this->selectionModel()->clearSelection();
		}
	}

	while_sort = false;
}

int ListPanel::onGetSelected()
{
	QModelIndexList selected = selectedIndexes();
	if (!selected.empty()) {
		return selected.first().row();
	}
	return -1;
}
void ListPanel::onSetSelected(int rowIndex)
{
	if (rowIndex >= 0 && rowIndex < model->rowCount()) {
		this->selectRow(rowIndex);
	}
}
void ListPanel::onSelectNext() {
	int currentRow = getSelected();
	if (currentRow >= 0 && currentRow < model->rowCount() - 1) {
		setSelected(currentRow + 1);
	}
}

void ListPanel::onSelectPrev() {
	int currentRow = getSelected();
	if (currentRow > 0 && currentRow < model->rowCount()) {
		setSelected(currentRow - 1);
	}
}
