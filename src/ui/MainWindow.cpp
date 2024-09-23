#include <QLabel>
#include <QLineEdit>
#include <QGuiApplication>
#include <QScreen>

#include "MainWindow.h"
#include "ControlsPanel.h"

MainWindow::MainWindow()
    : QWidget()
    , m_layout(new QVBoxLayout(this))
    , m_controlsPanel(new ControlsPanel(this))
{
    this->setWindowTitle("CustomPlayer");
    this->setObjectName("MainWindow");

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    this->resize(screenGeometry.size() * 0.8);

    m_layout->setContentsMargins(0, 0, 0, 0);

    m_layout->addWidget(m_controlsPanel, 0, Qt::AlignTop);
}
