import QtQuick
import QtQuick.Controls.impl
import QtQuick.Controls

import CustomPlayer

TextField {
    id: searchField
    
    leftPadding: 48
    font.pointSize: 12

    placeholderText: qsTr("Search...")
    
    hoverEnabled: true
    background: Rectangle {
        radius: Math.min(searchField.width, searchField.height) / 2.2
        color: searchField.focus | searchField.hovered ? Colors.accent : Colors.secondary
        border.color: Colors.primary
        border.width: 1

        Behavior on color {
            ColorAnimation { duration: 100 }
        }
    }

    IconLabel {
        icon.name: "search"
        anchors.left: parent.left
        anchors.leftMargin: 13
        anchors.verticalCenter: parent.verticalCenter
        icon.width: 28
        icon.height: 28
        width: 28
        height: 28
    }

    Button {
        width: 30
        height: 30
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        visible: searchField.text.length > 0
        icon.name: "edit-clear"

        focusPolicy: Qt.NoFocus
        onClicked: searchField.clear()
    }

    Keys.onEscapePressed: {
        searchField.focus = false
    }
}
