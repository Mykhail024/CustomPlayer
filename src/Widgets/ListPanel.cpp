#include <QHeaderView>

#include "../Config/Config.h"
#include "ListPanel.h"

ListPanel::ListPanel(QString dataBase, QWidget *parent) : QTableWidget(parent), dataBase(dataBase), columns(Config::getColumns())
{
	connect(this, &ListPanel::update, &ListPanel::onUpdate);
	connect(this, &ListPanel::setupColumns, &ListPanel::onSetupColumns);
}

void ListPanel::onUpdate()
{

}

void ListPanel::onSetupColumns()
{
	QStringList headerLabels;

	for(const auto& colName : columns)
	{
		if(colName.second)
		{
			if(colName.first == "Modified-Date") headerLabels << "Modified Date";
			else headerLabels << colName.first;
		}
	}

	this->setColumnCount(headerLabels.size());
	this->setHorizontalHeaderLabels(headerLabels);

	for (int i = 0; i < headerLabels.size(); ++i)
	{
		this->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
	}
}
