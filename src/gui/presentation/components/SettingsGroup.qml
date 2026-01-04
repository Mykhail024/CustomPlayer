import QtQuick
import QtQuick.Layouts
import QtQuick.Controls


import CustomPlayer

Rectangle {
    property string title

    width: parent.width
    implicitHeight: contentLayout.implicitHeight + 30
    color: "transparent"


    property alias content: groupContent.children

    ColumnLayout {
        id: contentLayout
        width: parent.width
        spacing: 10

        Label {
            text: title
            font.bold: true
            font.pointSize: 12
            color: Colors.text_p
            Layout.bottomMargin: 5
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Colors.primary
        }

        ColumnLayout {
            id: groupContent
            // width: parent.width
            Layout.fillWidth: true
            spacing: 10
            Layout.margins: 5
        }
    }
}
