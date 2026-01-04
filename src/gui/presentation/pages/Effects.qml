pragma ComponentBehavior: Bound;

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CustomPlayer.Audio

import CustomPlayer

import "../components"

Item {
    id: root
    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true

            Text {
                Layout.margins: 10
                Layout.alignment: Qt.AlignLeft
                text: "Effects"
                font.pointSize: 14
                color: Colors.text_p
            }

            Item {
                Layout.fillWidth: true
            }

            SearchTextField {
                id: search
                Layout.alignment: Qt.AlignRight
                Layout.preferredWidth: 300
                Layout.preferredHeight: 45
                Layout.margins: 10
            }
        }

        ListView {
            id: effectsListView

            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.fillHeight: true
            Layout.preferredWidth: Math.min(root.width, 600)
            Layout.margins: 10
            clip: true
            spacing: 50

            model: AudioEffectManager

            delegate:  ColumnLayout {
                id: droot

                width: ListView.view.width

                required property var model

                RowLayout {
                    Text {
                        Layout.alignment: Qt.AlignTop | Qt.AlignLeft
                        font.pointSize: 13
                        color: Colors.text_p

                        text: droot.model.effectName 
                        font.bold: true
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                    Switch {
                        Layout.alignment: Qt.AlignTop | Qt.AlignRight
                        text: "Enabled"

                        checked: droot.model.effectEnabled

                        onClicked: droot.model.effectEnabled = checked
                    }
                }

                Repeater {
                    id: rep

                    Layout.bottomMargin: 30
                    model: droot.model.effectParameters

                    delegate: RowLayout {
                        id: r

                        visible: droot.model.effectEnabled

                        required property var modelData

                        Text {
                            id: text
                            verticalAlignment: Text.AlignVCenter
                            font.pointSize: 12
                            color: Colors.text_s

                            text: r.modelData.name 
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        Slider {
                            id: slider

                            from: r.modelData.min
                            to: r.modelData.max
                            value: r.modelData.value

                            wheelEnabled: true
                            stepSize: to / 100

                            onValueChanged: r.modelData.value = value

                            MouseArea {
                                anchors.fill: slider
                                acceptedButtons: Qt.RightButton
                                onPressed: (mouse) => {
                                    if (mouse.button === Qt.RightButton) {
                                        slider.value = r.modelData.defaultValue
                                    }
                                }
                            }

                            showMin: true
                            showMax: true
                            showValue: true

                            Layout.rightMargin: 20
                        }
                    }
                }
            }

            ScrollBar.vertical: ScrollBar { 
                id: scrollBar

                width: 10
                minimumSize: 0.06
            }
        }
    }
}
