#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenu>

namespace Controls
{
    class LineEdit;
}

class PlaylistControl : public QWidget
{
    Q_OBJECT
    public:
        PlaylistControl(QWidget *parent);

    public slots:
        void onAddBtnClick();
        void onAddFiles();
        void onAddFolder();
        bool onNewPlaylist();

    protected:
        void paintEvent(QPaintEvent *pe) override;

    private:
        QMenu *m_addMenu;
        QHBoxLayout *m_layout;
        Controls::LineEdit *m_findLineEdit;
        QPushButton *m_addButton;
};
