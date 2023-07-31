#include <QApplication>

#include "Widgets/MainWindow.hpp"
#include "AudioServers/PipeWire.hpp"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

	AudioServers::PipeWire *pipewire = new AudioServers::PipeWire();

    Widgets::MainWindow window(pipewire);
    window.addFolder("/home/hong19/Music/");
    window.show();
    window.updateSongList();
    return app.exec();
}

