#include <QScreen>
#include <QGuiApplication>
#include <QStyleOption>
#include <QPainter>
#include <QMenu>
#include <QFileDialog>
#include <QStandardPaths>
#include <QInputDialog>

#include "Core/EventHandler.h"

#include "Core/Playlist/PlaylistManager.h"
#include "Core/Playlist/PlaylistModel.h"

#include "Widgets/Controls/LineEdit.h"

#include "PlaylistControl.h"

#include "moc_PlaylistControl.cpp"

PlaylistControl::PlaylistControl(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QHBoxLayout(this))
    , m_findLineEdit(new Controls::LineEdit(this))
    , m_addButton(new QPushButton("+", this))
    , m_addMenu(new QMenu(this))
{
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    this->setObjectName("PlaylistControl");

    QAction *addFiles = m_addMenu->addAction(tr("Add files"));
    QAction *addFolder = m_addMenu->addAction(tr("Add folder"));
    QAction *newPlaylist = m_addMenu->addAction(tr("New Playlist"));

#ifdef __linux__
    addFiles->setIcon(QIcon::fromTheme("document-open"));
    addFolder->setIcon(QIcon::fromTheme("folder-open"));
    newPlaylist->setIcon(QIcon::fromTheme("document-new"));
#else
    addFiles->setIcon(this->style()->standardIcon(QStyle::SP_FileIcon));
    addFolder->setIcon(this->style()->standardIcon(QStyle::SP_DirOpenIcon));
    newPlaylist->setIcon(this->style()->standardIcon(QStyle::SP_FileDialogNewFolder));
#endif

    m_addMenu->adjustSize();

    m_addButton->setObjectName("AddSongButton");
    m_addButton->setContentsMargins(0, 0, 0, 0);
    m_addButton->setFixedSize(screenGeometry.width() * 0.02, screenGeometry.width() * 0.02);

    m_findLineEdit->setContentsMargins(0, 0, 0, 0);
    m_findLineEdit->setClearButtonEnabled(true);
    m_findLineEdit->setPlaceholderText(tr("Search"));

    m_layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    m_layout->addWidget(m_findLineEdit);
    m_layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    m_layout->addWidget(m_addButton);

    m_layout->setContentsMargins(3, 3, 3, 3);
    m_layout->setSpacing(0);

    connect(m_findLineEdit, &QLineEdit::textChanged, eventHandler(), &EventHandler::PlaylistFind);
    connect(eventHandler(), &EventHandler::onFindClear, this, [&]{ m_findLineEdit->clear(); m_findLineEdit->clearFocus(); });
    connect(m_addButton, &QPushButton::clicked, this, &PlaylistControl::onAddBtnClick);
    connect(eventHandler(), &EventHandler::onFindActivate, this, [&]{ m_findLineEdit->setFocus(); });
    connect(addFiles, &QAction::triggered, this, &PlaylistControl::onAddFiles);
    connect(newPlaylist, &QAction::triggered, this, &PlaylistControl::onNewPlaylist);
}

void PlaylistControl::paintEvent(QPaintEvent *pe)
{
    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}

void PlaylistControl::onAddBtnClick()
{
    m_addMenu->exec(QCursor::pos());
}

void PlaylistControl::onAddFiles()
{
    if(playlistManager()->count() == 0) return;
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        tr("Select Music Files"),
        QStandardPaths::writableLocation(QStandardPaths::MusicLocation),
        tr("Music Files (*.mp3)")
    );
    if(fileNames.empty()) return;

    int activeIndex = playlistManager()->active();
    auto *playlist = playlistManager()->operator[](activeIndex);

    for(const QString &file : fileNames) {
        playlist->insertSong(file);
    }
}

void PlaylistControl::onAddFolder()
{

}

void PlaylistControl::onNewPlaylist()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("New Playlist"), tr("Playlist Name:"), QLineEdit::Normal, "", &ok);
    if(ok && !name.isEmpty()) {
        playlistManager()->createPlaylist(name);
    }
}
