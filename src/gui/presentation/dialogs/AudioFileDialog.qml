import QtCore
import QtQuick
import QtQuick.Dialogs

FileDialog {
    id: fileDialog
    currentFolder: StandardPaths.standardLocations(StandardPaths.MusicLocation)[0]
    nameFilters: ["Audio Files (*.mp3)"]
    title: qsTr("Select audio files")
    fileMode: FileDialog.OpenFiles

    onAccepted: {
        model.sourceModel.addSongs(selectedFiles)
    }
}
