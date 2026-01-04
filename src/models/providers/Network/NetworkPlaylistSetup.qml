import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    width: parent ? parent.width : 0
    implicitHeight: layout.implicitHeight

    ColumnLayout {
        id: layout
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 12

        ComboBox {
            id: modeComb
            model: factory.modes
            Layout.fillWidth: true
            onCurrentTextChanged: factory.mode = currentText
        }

        TextField {
            id: createPassword
            Layout.fillWidth: true
            echoMode: TextInput.Password
            placeholderText: "password"
            onTextChanged: factory.password = text
            visible: factory.mode == "Create"
        }

        // Заголовок списку
        Label {
            text: "Discovered Devices:"
            font.bold: true
            Layout.topMargin: 10
            visible: modeComb.currentText == "Connect"
        }

        // Список знайдених пристроїв
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            background: Rectangle {
                color: "#272b3c"
            }

            visible: modeComb.currentText == "Connect"

            ScrollView {
                anchors.fill: parent
                clip: true

                Column {
                    id: devicesColumn
                    width: parent.width
                    spacing: 5

                    // Модель для зберігання пристроїв
                    ListModel {
                        id: devicesModel
                    }

                    // Відображення елементів моделі
                    Repeater {
                        model: devicesModel
                        delegate: Rectangle {
                            width: devicesColumn.width
                            height: deviceInfo.implicitHeight + 10
                            color: factory.ipAddress == ip ? "#5B76BC" : "#6A76AA"
                            radius: 3

                            ColumnLayout {
                                id: deviceInfo
                                anchors.fill: parent
                                anchors.margins: 5

                                Text {
                                    text: `<b>${name}</b>`
                                    font.pixelSize: 14
                                    Layout.fillWidth: true
                                }

                                Text {
                                    text: `IP: ${ip}:${port}`
                                    font.pixelSize: 12
                                    color: "#555555"
                                    Layout.fillWidth: true
                                }
                            }

                            // Кнопка вибору пристрою
                            Button {
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.rightMargin: 5
                                text: "Select"
                                onClicked: {
                                    factory.ipAddress = ip
                                }
                            }
                        }
                    }
                }
            }
        }
        TextField {
            id: connectPassword
            Layout.fillWidth: true
            echoMode: TextInput.Password
            placeholderText: "password"
            onTextChanged: factory.password = text
            visible: factory.mode == "Connect" && factory.ipAddress.length > 0
        }
    }


    // Підключення сигналу знайденого пристрою
    Connections {
        target: factory.networkDiscovery
        function onPeerDiscovered(address, port, playlistName) {
            // Перевірка на дублікати
            for (var i = 0; i < devicesModel.count; i++) {
                if (devicesModel.get(i).name === playlistName) {
                    devicesModel.set(i, {
                        "ip": address,
                        "port": port,
                        "name": playlistName
                    })
                    return
                }
            }
            
            // Додавання нового пристрою
            devicesModel.append({
                "ip": address,
                "port": port,
                "name": playlistName
            })
        }
    }
}
