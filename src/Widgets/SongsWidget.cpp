#include <QTableView>
#include <QSortFilterProxyModel>
#include <QHeaderView>

#include "../Core/PlaylistManager.h"
#include "../Core/PlaylistModel.h"
#include "../Core/EventHandler.h"
#include "Playlist/PlaylistView.h"

#include "SongsWidget.h"

#include "moc_SongsWidget.cpp"

SongsWidget::SongsWidget(QWidget *parent) : QTabWidget(parent)
{
	this->setObjectName("SongsWidget");

	for(int i = 0; i < playlistManager()->count(); i++)
	{
		auto model = playlistManager()->operator[](i);
		PlaylistView *widget = new PlaylistView(model, this);

		this->addTab(widget, model->name());
	}
	if(playlistManager()->count() != 0)
	{
		this->setCurrentIndex(playlistManager()->active() % this->count());
	}

	connect(eventHandler(), &EventHandler::onNextPlaylist, this, &SongsWidget::selectNextTab);
	connect(eventHandler(), &EventHandler::onPrevPlaylist, this, &SongsWidget::selectPrevTab);
	connect(eventHandler(), &EventHandler::onAddPlaylists, this, &SongsWidget::addPlaylists);
	connect(this, &QTabWidget::currentChanged, playlistManager(), &PlaylistManager::setActive);
}

void SongsWidget::selectNextTab()
{
	this->setCurrentIndex((this->currentIndex() + 1) % this->count());
}
void SongsWidget::selectPrevTab()
{
	int count = this->count();
	this->setCurrentIndex((this->currentIndex() - 1 + count) % count);
}

void SongsWidget::addPlaylists()
{
	auto model = playlistManager()->operator[](playlistManager()->count() - 1);
	PlaylistView *widget = new PlaylistView(model, this);
	this->addTab(widget, model->name());
}