import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import CustomPlayer
import "../components"

Item {
    id: settingsRoot
    property var settingsGroups: []
    
    function filterSettings(searchText) {
        const lowerSearch = searchText.toLowerCase()
        
        for (let i = 0; i < settingsGroups.length; i++) {
            const group = settingsGroups[i]
            let hasMatch = false
            
            // Перевіряємо заголовок групи
            if (group.title.toLowerCase().includes(lowerSearch)) {
                hasMatch = true
            } 
            // Перевіряємо всі дочірні елементи групи
            else {
                const children = group.content
                for (let j = 0; j < children.length; j++) {
                    const child = children[j]
                    if (containsText(child, lowerSearch)) {
                        hasMatch = true
                        break
                    }
                }
            }
            
            group.visible = hasMatch
            // group.Layout.preferredHeight = hasMatch ? undefined : 0
        }
    }
    
    function containsText(item, searchText) {
        if (!item) return false
        
        // Перевіряємо різні типи тексту
        if (item.text && item.text.toLowerCase().includes(searchText)) return true
        if (item.displayText && item.displayText.toLowerCase().includes(searchText)) return true
        if (item.placeholderText && item.placeholderText.toLowerCase().includes(searchText)) return true
        
        // Перевіряємо дочірні елементи
        if (item.children) {
            for (let i = 0; i < item.children.length; i++) {
                if (containsText(item.children[i], searchText)) {
                    return true
                }
            }
        }
        
        return false
    }
    
    property var languages: ["English", "Українська"]
    property var qualityOptions: ["Low (128 kbps)", "Medium (192 kbps)", "High (320 kbps)", "Lossless"]
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        
        // Заголовок та пошук
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            Layout.topMargin: 10
            Layout.leftMargin: 10
            Layout.rightMargin: 10

            Text {
                text: qsTr("Settings")
                font.pointSize: 18
                font.bold: true
                color: Colors.text_p
            }

            Item { Layout.fillWidth: true }

            SearchTextField {
                id: searchField
                Layout.preferredWidth: 300
                Layout.preferredHeight: 40
                placeholderText: qsTr("Search settings...")
                
                onTextChanged: filterSettings(text)
            }
        }

        // Основний контент з прокруткою
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                id: settingsColumn
                width: parent.width
                spacing: 20

                // Секція загальних налаштувань
                SettingsGroup {
                    id: generalGroup
                    title: qsTr("General Settings")
                    Layout.fillWidth: true
                    
                    Component.onCompleted: settingsRoot.settingsGroups.push(generalGroup)
                    
                    content: GridLayout {
                        id: generalContent
                        columns: 2
                        columnSpacing: 20
                        rowSpacing: 15
                        width: parent.width
                        
                        Label {
                            text: qsTr("Language:")
                            color: Colors.text_s
                        }
                        ComboBox {
                            id: languageCombo
                            Layout.fillWidth: true
                            model: languages
                            currentIndex: 1
                        }
                        
                        Label {
                            text: qsTr("Launch on system startup:")
                            color: Colors.text_s
                        }
                        Switch {
                            id: startupSwitch
                            checked: true
                        }
                        
                        Label {
                            text: qsTr("Minimize to tray:")
                            color: Colors.text_s
                        }
                        Switch {
                            id: traySwitch
                            checked: true
                        }
                    }
                }

                // Секція аудіо
                SettingsGroup {
                    id: audioGroup
                    title: qsTr("Audio Settings")
                    Layout.fillWidth: true
                    
                    Component.onCompleted: settingsRoot.settingsGroups.push(audioGroup)

                    content: GridLayout {
                        id: audioContent
                        columns: 2
                        columnSpacing: 20
                        rowSpacing: 15
                        width: parent.width
                        
                        Label {
                            text: qsTr("Streaming quality:")
                            color: Colors.text_s
                        }
                        ComboBox {
                            id: qualityCombo
                            Layout.fillWidth: true
                            model: qualityOptions
                            currentIndex: 2
                        }
                        
                        Label {
                            text: qsTr("Crossfade duration:")
                            color: Colors.text_s
                        }
                        SpinBox {
                            id: crossfadeSpin
                            Layout.fillWidth: true
                            from: 0
                            to: 10
                            value: 3

                            property string suffix: "sec"

                            textFromValue: function(value, locale) {
                                return Number(value).toLocaleString(locale, 'f', 0) + suffix
                            }
                        }
                        
                        Label {
                            text: qsTr("Replay gain:")
                            color: Colors.text_s
                        }
                        ComboBox {
                            id: gainCombo
                            Layout.fillWidth: true
                            model: ["Off", "Track", "Album"]
                        }
                    }
                }

                // Секція кешу
                SettingsGroup {
                    id: cacheGroup
                    title: qsTr("Cache Settings")
                    Layout.fillWidth: true
                    
                    Component.onCompleted: settingsRoot.settingsGroups.push(cacheGroup)

                    content: GridLayout {
                        id: cacheContent
                        columns: 2
                        columnSpacing: 20
                        rowSpacing: 15
                        width: parent.width
                        
                        Label {
                            text: qsTr("Cache size:")
                            color: Colors.text_s
                        }
                        RowLayout {
                            SpinBox {
                                id: cacheSize
                                from: 100
                                to: 5000
                                value: 1024
                                stepSize: 128
                            }
                            Label { text: "MB" }
                        }
                        
                        Label {
                            text: qsTr("Current cache usage:")
                            color: Colors.text_s
                        }
                        Label {
                            text: "843 MB"
                            color: Colors.text_p
                        }
                    }
                }

                // Секція бібліотеки
                SettingsGroup {
                    id: libraryGroup
                    title: qsTr("Library Settings")
                    Layout.fillWidth: true
                    
                    Component.onCompleted: settingsRoot.settingsGroups.push(libraryGroup)

                    content: GridLayout {
                        id: libraryContent
                        columns: 2
                        columnSpacing: 20
                        rowSpacing: 15
                        width: parent.width
                        
                        Label {
                            text: qsTr("Library path:")
                            color: Colors.text_s
                        }
                        RowLayout {
                            TextField {
                                id: libraryPath
                                Layout.fillWidth: true
                                text: "/home/hong19/Music"
                                readOnly: true
                            }
                            Button {
                                text: qsTr("Browse...")
                                onClicked: folderDialog.open()
                            }
                        }
                        
                        Label {
                            text: qsTr("Auto scan for changes:")
                            color: Colors.text_s
                        }
                        Switch {
                            id: scanSwitch
                            checked: true
                        }
                        
                        Label {
                            text: qsTr("Scan interval:")
                            color: Colors.text_s
                        }
                        ComboBox {
                            id: intervalCombo
                            Layout.fillWidth: true
                            model: ["Every 30 minutes", "Hourly", "Daily"]
                            currentIndex: 2
                        }
                    }
                }
            }
        }
    }

    FolderDialog {
        id: folderDialog
        title: qsTr("Select Music Folder")
        onAccepted: libraryPath.text = selectedFolder
    }
}
