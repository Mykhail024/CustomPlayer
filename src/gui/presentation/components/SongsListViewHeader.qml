import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import CustomPlayer

import "../dialogs"

Rectangle {
    id: root

    width: parent.width;
    height: 70
    z: 1000
    color: "transparent"

    property string filterString

    AudioFileDialog {
        id: audioFileDialog
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 6

        Button {
            id: addButton
            text: "+"
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            Layout.bottomMargin: 15

            onClicked: {
                audioFileDialog.open()
            }

            background: Rectangle {
                radius: 20
                color: addButton.hovered ? Colors.accent : Colors.secondary
                border.color: Colors.primary
                border.width: 1

                Behavior on color {
                    ColorAnimation { duration: 100 }
                }
            }
        }

        SearchTextField {
            Layout.alignment: Qt.AlignRight
            Layout.fillHeight: true
            Layout.rightMargin: 15
            Layout.bottomMargin: 15

            Layout.preferredWidth: focus ? parent.width/3 : parent.width/6

            Behavior on Layout.preferredWidth {
                PropertyAnimation { duration: 100 }
            }

            onTextChanged: root.filterString = text
        } 
    }
} 
