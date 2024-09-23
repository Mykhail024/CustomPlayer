#pragma once

#include <QWidget>
#include <QVBoxLayout>

#include "ControlsPanel.h"
#include "macros.h"

class MainWindow : public QWidget
{
    Q_OBJECT
    DEFAULT_WIDGET_PAINT_EVENT
    public:
        MainWindow();

    private:
        QVBoxLayout *m_layout;

        ControlsPanel *m_controlsPanel;
};
