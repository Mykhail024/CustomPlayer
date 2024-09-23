#include <QApplication>
#include <QFile>

#include "MainWindow.h"

QString readTextFile(const QString &filePath)
{
    QFile file(filePath);
    if(file.open(QFile::ReadOnly | QFile::Text)) {
        return QString::fromUtf8(file.readAll());
    } else {
        qWarning() << "Error reading: " << filePath.toStdString();
        return "";
    }
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    app.setStyle("Fusion");
    app.setStyleSheet(readTextFile(":/Styles/Default.qss"));

    MainWindow window;
    window.show();

    return app.exec();
}
