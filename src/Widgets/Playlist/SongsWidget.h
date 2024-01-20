#pragma once

#include <QTabWidget>

class SongsWidget : public QTabWidget
{
    Q_OBJECT
    public:
        SongsWidget(QWidget *parent = nullptr);
    public slots:
        void selectNextTab();
        void selectPrevTab();
        void addPlaylist();
        void setupTabs();

    private slots:
        void renameTab(const int &index);

    protected:
        void mousePressEvent(QMouseEvent *event) override;

    private slots:
        void onTabClose(const int &index);
};
