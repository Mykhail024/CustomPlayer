import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import CustomPlayer.Common
import CustomPlayer.Audio

import CustomPlayer

Item {
    id: item
    height: 60
    width: ListView.view.width - ListView.view.spacing - 5

    signal doubleClicked(int index)

    required property var model
    required property int index

    Rectangle {
        anchors.fill: parent
        radius: 2

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            
            hoverEnabled: true

            onDoubleClicked: {
                item.doubleClicked(item.index)
                MusicPlayer.playFile(item.model.uri);
            }
        }

        color: mouseArea.containsMouse || item.ListView.isCurrentItem ? Colors.accent : Colors.secondary

        Behavior on color {
            ColorAnimation { duration: 100 }
        }

        RowLayout {
            anchors.fill: parent
            RoundedImage {
                id: img
                radius: 4

                Layout.margins: 6
                Layout.preferredHeight: item.height - Layout.margins*2
                Layout.preferredWidth: Layout.preferredHeight

                source: "file://" + item.model.albumArt
                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                cache: true
                smooth: true
            }
            ColumnLayout {
                Text {
                    text: item.model.album ? item.model.album + " - " + item.model.title : item.model.title
                    color: Colors.text_p
                }
                Text {
                    text: item.model.artist
                    color: Colors.text_s
                }
            }
            Item { Layout.fillWidth: true }
            RowLayout {
                Text {
                    text: Utils.msecToDuration(item.model.length)
                    color: Colors.text_s
                }
                Button {
                    id: favorite
                    icon.name: item.model.favorite ? "favorite-favorited" : "favorite"
                    icon.color: Colors.text_p
                    onClicked: item.model.favorite = !item.model.favorite

                    ToolTip.visible: hovered
                    ToolTip.delay: 1000
                    ToolTip.text: item.model.favorite ? qsTr("Remove song from favorites") : qsTr("Add song to favorites")
                }
            }

        }
    }
}
