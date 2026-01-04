import QtQuick 6.0
import QtQuick.Effects

Rectangle {
    id: root

    visible: true
    color: "transparent"

    property real radius: 0
    property alias source: sourceItem.source
    property alias mipmap: sourceItem.mipmap
    property alias fillMode: sourceItem.fillMode
    property alias asynchronous: sourceItem.asynchronous
    property alias cache : sourceItem.cache
    property alias smooth : sourceItem.smooth
    
    Image {
        id: sourceItem
        anchors.fill: parent
        visible: false
    }

    MultiEffect {
        source: sourceItem
        anchors.fill: sourceItem
        maskEnabled: true
        maskThresholdMin: 0.5
        maskSpreadAtMin: 1.0
        maskSource: mask
    }

    Item {
        id: mask
        anchors.fill: parent
        layer.enabled: true
        visible: false

        Rectangle {
            anchors.fill: parent
            radius: root.radius
            color: "black"
            smooth: true
        }
    }
}
