import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

T.Slider {
    id: control

    property bool showMax: false
    property bool showMin: false
    property bool showValue: false

    property int fontPixelSize: 12
    property int textPadding: 14

    property int leftTextPadding: textPadding + (fromText.text.length * (fontPixelSize)/2)
    property int rightTextPadding: textPadding + (toText.text.length * (fontPixelSize/2))

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitHandleWidth + leftPadding + rightPadding) + ((showMin ? 1 : 0) + (showMax ? 1 : 0)) * textPadding
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitHandleHeight + topPadding + bottomPadding)

    padding: 6
    leftPadding: showMin ? leftTextPadding : padding
    rightPadding: showMax ? rightTextPadding : padding
    bottomPadding: showValue ? textPadding : padding

    handle: Rectangle {
        x: control.leftPadding + (control.horizontal ? control.visualPosition * (control.availableWidth - width) : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : control.visualPosition * (control.availableHeight - height))
        implicitHeight:  18
        implicitWidth: 18
        radius: width / 2
        color: control.pressed ? control.palette.dark : control.palette.light
        border.width: control.visualFocus ? 2 : 1
        border.color: control.visualFocus ? control.palette.highlight : control.enabled ? control.palette.mid : control.palette.midlight

        Text {
            anchors.top: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: control.fontPixelSize
            text: Number(control.value).toFixed(2)
            color: control.palette.text
            visible: control.showValue
        }
    }

    background: Rectangle {
        x: control.leftPadding + (control.horizontal ? 0 : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : 0)
        implicitWidth: control.horizontal ? 200 : 6
        implicitHeight: control.horizontal ? 6 : 200
        width: control.horizontal ? control.availableWidth : implicitWidth
        height: control.horizontal ? implicitHeight : control.availableHeight
        radius: 3
        color: control.palette.midlight
        scale: control.horizontal && control.mirrored ? -1 : 1

        Rectangle {
            y: control.horizontal ? 0 : control.visualPosition * parent.height
            width: control.horizontal ? control.position * parent.width : 6
            height: control.horizontal ? 6 : control.position * parent.height

            radius: 3
            color: control.palette.dark
        }
    }

    Text {
        id: fromText
        anchors.left: control.left
        anchors.verticalCenter: control.background.verticalCenter
        anchors.rightMargin: control.leftTextPadding
        font.pixelSize: control.fontPixelSize
        text: Number(control.from).toFixed(0)
        color: control.palette.text
        visible: control.showMin
    }

    Text {
        id: toText
        anchors.right: control.right
        anchors.verticalCenter: control.background.verticalCenter
        anchors.leftMargin: control.rightTextPadding
        font.pixelSize: control.fontPixelSize
        text: Number(control.to).toFixed(0)
        color: control.palette.text
        visible: control.showMax
    }
}

