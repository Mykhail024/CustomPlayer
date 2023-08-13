#include <QHeaderView>

#include "../Convert.cpp"
#include "ListPanel.h"

static int findColumn(QTableWidget *widget,const QString& name)
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

ListPanel::ListPanel(QString dataBase, QWidget *parent) : QTableWidget(parent), columns(Config::getColumns())
{
	db.Open("main.db3");
	songs = db.ReadSongs();

	connect(this, &ListPanel::update, &ListPanel::onUpdate);
	connect(this, &ListPanel::setupColumns, &ListPanel::onSetupColumns);
	connect(this, &ListPanel::setupRows, &ListPanel::onSetupRows);

	this->verticalHeader()->setVisible(false);
	this->setEditTriggers(QAbstractItemView::NoEditTriggers);
	this->setSelectionMode(QAbstractItemView::SingleSelection);
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->setShowGrid(false);
	this->setAlternatingRowColors(true);
}

void ListPanel::onUpdate()
{
	//db.updateSongs("/home/hong19/Music/");
	auto songs = db.ReadSongs();
	onSetupRows();
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

	this->setColumnCount(headerLabels.size());
	this->setHorizontalHeaderLabels(headerLabels);

	for (const auto& label : headerLabels)
	{
		int column = findColumn(this, label);
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
	this->setRowCount(songs.size());
	for(int row = 0; row < songs.size(); row++)
	{
		int column = 0;
		if(columns.Title)
		{
			this->setItem(row, column, new QTableWidgetItem(QString::fromStdString(songs[row].Title)));
			column++;
		}
		if(columns.Artist)
		{
			this->setItem(row, column, new QTableWidgetItem(QString::fromStdString(songs[row].Artist)));
			column++;
		}
		if(columns.Album)
		{
			this->setItem(row, column, new QTableWidgetItem(QString::fromStdString(songs[row].Album)));
			column++;
		}
		if(columns.Length)
		{
			this->setItem(row, column, new QTableWidgetItem(Convert::Seconds::toMinutes(songs[row].length / 1000)));
			column++;
		}
		if(columns.Year)
		{
			this->setItem(row, column, new QTableWidgetItem(QString::number(songs[row].year)));
			column++;
		}
		if(columns.ModifiedDate)
		{
			this->setItem(row, column, new QTableWidgetItem(QString::number(songs[row].modifiedDate)));
			column++;
		}
	}
}
