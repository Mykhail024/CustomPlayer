import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Effects
import QtQuick.Controls

import CustomPlayer
import CustomPlayer.Models

import "components"
import "pages"

ApplicationWindow {
    id: root
    visible: true
    title: qsTr("CustomPlayer")
    color: Colors.background

    palette.dark: Colors.primary
    palette.mid: Colors.secondary
    palette.accent: Colors.accent
    palette.buttonText: Colors.text_p

    readonly property bool isMobile:  (Qt.platform.os === "android" || Qt.platform.os === "ios")

    width: isMobile ? Screen.width : Screen.width * 0.8
    height: isMobile ? Screen.height : Screen.height * 0.8
    minimumWidth: isMobile ? width : 800
    minimumHeight: isMobile ? height : 400


    Component {
        id: settingsPage
        Settings {
            id: settings
        }
    }

    Component {
        id: effectsPage
        Effects {
            id: effects
        }
    }

    Component {
        id: songsView
        SongsListView {
            model: FilterProxyModel { sourceModel: playlistsModel.currentPlaylist } // if playlist null ASSERT failure in QList::at: "index out of range"
        }
    }

    PlaylistsModel {
        id: playlistsModel

        // property var savedPositions: ({})

        // onCurrentIndexAboutToChange: (old_index) => {
        //     if(listView.contentY > 0 && listView.model && old_index >= 0) {
        //         savedPositions[old_index] = listView.contentY;
        //     }
        // }
    }

    CreatePlaylistDialog {
        id: dialog

        anchors.centerIn: parent
        modal: true

        width: 300
        // height: 200
    }


    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Rectangle {
                id: prect
                Layout.preferredWidth: 125
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignLeft
                color: Colors.background
                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    shadowColor: Colors.shadow
                    shadowHorizontalOffset: 0.1
                    shadowVerticalOffset: 0
                    shadowBlur: 0.5
                }

                property int currentSizebarItem: -1;

                Sidebar {
                    id: psidebar
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: effectsButton.top
                    model: playlistsModel

                    onItemClicked: (index) => {
                        if(index >= 0) {
                            pageLoader.sourceComponent = songsView
                            psidebar.currentIndex = index
                            prect.currentSizebarItem = 0
                        }
                    }
                }

                Button {
                    id: newButton
                    text: "New...."
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: separator.top
                    anchors.bottomMargin: 10
                    height: 50

                    onClicked: {
                        dialog.open()
                    }

                    background: Rectangle {
                        color: Colors.accent
                        opacity: newButton.hovered ? 1.0 : 0.0

                        Behavior on opacity {
                            NumberAnimation { duration: 75 }
                        }
                    }
                }

                Rectangle {
                    id: separator
                    color: "white"
                    opacity: 0.3
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: effectsButton.top
                    height: 1
                    anchors.margins: 10
                }

                Button {
                    id: effectsButton
                    text: "Effects"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: settingsButton.top
                    height: 50

                    property bool active: prect.currentSizebarItem == 1

                    background: Rectangle {
                        color: Colors.accent
                        opacity: effectsButton.active ? 1.0 : 0.0

                        Behavior on opacity {
                            NumberAnimation { duration: 75 }
                        }
                    }

                    onClicked: {
                        pageLoader.sourceComponent = effectsPage
                        psidebar.currentIndex = -1
                        prect.currentSizebarItem = 1
                    }
                }

                Button {
                    id: settingsButton
                    text: "Settings"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 10
                    height: 50

                    property bool active: prect.currentSizebarItem == 2

                    background: Rectangle {
                        color: Colors.accent
                        opacity: settingsButton.active ? 1.0 : 0.0

                        Behavior on opacity {
                            NumberAnimation { duration: 75 }
                        }
                    }

                    onClicked: {
                        pageLoader.sourceComponent = settingsPage
                        psidebar.currentIndex = -1
                        prect.currentSizebarItem = 2
                    }
                }
            }

            Loader {
                id: pageLoader
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: 10
            }
        }

        ControlsPanel {
            id: controlsPanel
            Layout.alignment: Qt.AlignBottom
            Layout.fillWidth: true
            implicitHeight: 100
        }
    }
}
