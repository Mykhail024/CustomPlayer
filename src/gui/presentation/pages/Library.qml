// pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import CustomPlayer.Models

import "../components"

ColumnLayout {
    objectName: "LibraryPage"

    SongsListView {
        id: listView

        Layout.alignment: Qt.AlignTop
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.topMargin: 30

        currentIndex: -1

        sourceModel: SongsModel {
            playlistFile: "/home/hong19/.config/CustomPlayer/Playlists/Default"
        }
    }
}
