import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls

import CustomPlayer

ListView {
    id: sidebar

    signal itemClicked(int index)

    clip: true
    // highlight: Rectangle {
    //     color: Colors.accent 
    //
    //     Behavior on opacity {
    //         NumberAnimation {
    //             duration: 75
    //         }
    //     }
    // }

    currentIndex: model.currentIndex
    highlightMoveDuration: 0

    delegate: Item {
        id: item
        height: 50
        width: ListView.view.width

        required property string name
        required property int index

        Button {
            id: btn
            anchors.fill: parent
            // anchors.verticalCenter: parent.verticalCenter
            icon.width: 28
            icon.height: 28

            text: item.name
            onClicked: () => {
                model.currentIndex = item.index
                sidebar.itemClicked(item.index)
            }

            background: Rectangle {
                color: Colors.accent
                opacity: sidebar.currentIndex == index ? 1.0 : 0.0

                Behavior on opacity {
                    NumberAnimation { duration: 75 }
                }
            }
        }
    }
}

