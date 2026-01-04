import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Effects

import CustomPlayer

import "../components"

Dialog {
    id: dialog

    property bool isRequirementsProvided: false

    background: Rectangle {
        color: Colors.background
        radius: 4
        border.color: Colors.shadow
        border.width: 1

        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: Colors.shadow
            shadowBlur: 0.5
        }
    }


    header: Label {
        text: "Create New Playlist"
        font.pixelSize: 20
        font.bold: true
        leftPadding: 24
        topPadding: 24
        bottomPadding: 12
        color: Colors.text_p
    }

    property var providerUiObj: null
    property var providerFactory: null

    onAccepted: {
        playlistsModel.addPlaylist(providerFactory)
        playlistsModel.update();
    }

    contentItem: ColumnLayout {
        spacing: 16
        // padding: 24

        TextField {
            id: nameField
            Layout.fillWidth: true
            placeholderText: "Playlist name"
            font.pixelSize: 14
            color: Colors.text_p
            selectByMouse: true

            background: Rectangle {
                color: "transparent"
                border.color: Colors.accent
                border.width: 1
                radius: 4
            }
        }

        ComboBox {
            id: providerSelector
            Layout.fillWidth: true
            model: playlistsModel.providersRegistry
            textRole: "factoryName"
            visible: playlistsModel.providersRegistry.rowCount() > 1
            onCurrentIndexChanged: {
                dialog.providerFactory = playlistsModel.providersRegistry.getFactory(currentIndex)
                dialog.providerUiObj = dialog.providerFactory.providerUi(dialog)
                if(dialog.providerUiObj) {
                    // obj.parent = uiLoader
                    uiLoader.setSourceObject(dialog.providerUiObj);
                    loaderScrollView.visible = true
                } else {
                    uiLoader.setSourceObject(null);
                    // uiLoader.sourceComponent = null
                    loaderScrollView.visible = false
                }
            }

            background: Rectangle {
                color: "transparent"
                border.color: Colors.accent
                border.width: 1
                radius: 4
            }
        }

        ScrollView {
            id: loaderScrollView
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            visible: false
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn

            ObjectLoader {
                id: uiLoader

                anchors.fill: parent
            }

            // Loader {
            //     clip: true
            //
            //     id: uiLoader
            //     anchors.fill: parent
            // }
        }
    }


    footer: DialogButtonBox {
        padding: 12
        background: Rectangle {
            color: "transparent"
        }

        Button {
            id: cancelButton
            text: "Cancel"
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            onClicked: dialog.reject()
            flat: true
            background: Rectangle {
                radius: 6
                color: Colors.accent
                opacity: cancelButton.hovered ? 1.0 : 0.0

                Behavior on opacity {
                    NumberAnimation { duration: 75 }
                }
            }
        }

        Button {
            id: createButton
            text: "Create"
            enabled: nameField.text.length > 0 && (dialog.providerUiObj ? dialog.providerFactory.isRequirementsProvided : true)
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            flat: true
            onClicked: dialog.accept()

            background: Rectangle {
                radius: 6
                color: Colors.accent
                opacity: createButton.hovered ? 1.0 : 0.0

                Behavior on opacity {
                    NumberAnimation { duration: 75 }
                }
            }
        }
    }
}

