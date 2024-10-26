import QtQuick 2.15
import QtQuick.Window
import QtCore
import QtQuick.Controls 2.15

ApplicationWindow {
    id: root
    visible: true
    title: "CustomPlayer"
    color: Colors.background
    width: (Qt.platform.os === "android" || Qt.platform.os === "ios") ? Screen.width : Screen.width * 0.8
    height: (Qt.platform.os === "android" || Qt.platform.os === "ios") ? Screen.height : Screen.height * 0.8
}
