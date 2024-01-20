#include <QTableView>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QMenu>

#include "Core/EventHandler.h"

#include "Core/Playlist/PlaylistManager.h"
#include "Core/Playlist/PlaylistModel.h"

#include "PlaylistView.h"

#include "SongsWidget.h"

#include "moc_SongsWidget.cpp"

SongsWidget::SongsWidget(QWidget *parent) : QTabWidget(parent)
{
    this->setObjectName("SongsWidget");
    this->setTabsClosable(true);

    setupTabs();

    connect(eventHandler(), &EventHandler::onNextPlaylist, this, &SongsWidget::selectNextTab);
    connect(eventHandler(), &EventHandler::onPrevPlaylist, this, &SongsWidget::selectPrevTab);
    connect(eventHandler(), &EventHandler::onAddPlaylist, this, &SongsWidget::addPlaylist);
    connect(eventHandler(), &EventHandler::onRenamePlaylist, this, &SongsWidget::setTabText);
    connect(this, &QTabWidget::currentChanged, playlistManager(), &PlaylistManager::setActive);
    connect(this, &QTabWidget::tabCloseRequested, this, &SongsWidget::onTabClose);
    connect(this->tabBar(), &QTabBar::tabBarDoubleClicked, this, &SongsWidget::renameTab);
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

void SongsWidget::addPlaylist()
{
    auto model = playlistManager()->operator[](playlistManager()->count() - 1);
    PlaylistView *widget = new PlaylistView(model, this);
    this->addTab(widget, model->name());
}

void SongsWidget::setupTabs()
{
    if(this->count() != 0) {
        this->clear();
    }

    QIcon closeIcon = QIcon::fromTheme("window-close");

    for(int i = 0; i < playlistManager()->count(); i++) {
        auto model = playlistManager()->operator[](i);
        PlaylistView *widget = new PlaylistView(model, this);

        this->addTab(widget, model->name());
    }
    if(playlistManager()->count() != 0) {
        this->setCurrentIndex(playlistManager()->active() % this->count());
    }
}

void SongsWidget::onTabClose(const int &index)
{
    this->removeTab(index);
    playlistManager()->removePlaylist(index);
}

void SongsWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        int tabIndex = tabBar()->tabAt(event->pos());
        if (tabIndex != -1) {
            QMenu menu(this);
            QAction *renameAction = menu.addAction("Rename");
            QAction *deleteAction = menu.addAction("Delete");

#ifdef __linux__
            deleteAction->setIcon(QIcon::fromTheme("edit-delete"));
            renameAction->setIcon(QIcon::fromTheme("document-open"));
#else
            deleteAction->setIcon(this->style()->standardIcon(QStyle::SP_TrashIcon));
            renameAction->setIcon(this->style()->standardIcon(QStyle::SP_FileDialogNewFolder));
#endif

            connect(renameAction, &QAction::triggered, this, [&]{
                        this->renameTab(tabIndex);
                    });
            connect(deleteAction, &QAction::triggered, this, [&]{
                        this->onTabClose(tabIndex);
                    });
            menu.exec(mapToGlobal(event->pos()));
        }
    }
    QTabWidget::mousePressEvent(event);
}

void SongsWidget::renameTab(const int &index)
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Rename playlist"), tr("New playlist Name:"), QLineEdit::Normal, "", &ok);
    if(ok && !name.isEmpty()) {
        playlistManager()->renamePlaylist(index, name);
    }
}
