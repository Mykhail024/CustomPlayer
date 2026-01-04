import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "components"
import "."

import BitPass.Presentation

Rectangle {
    id: dashboardPage
    color: Constants.primaryDark
    
    property int selectedFolderId: 0
    property var passwords: [
        { id: 1, service: "Gmail", username: "user@gmail.com", password: "encrypted...", folder: 1, isPinned: true, description: "Personal email" },
        { id: 2, service: "GitHub", username: "username", password: "encrypted...", folder: 2, isPinned: false, description: "Development account" },
        { id: 3, service: "Bank", username: "account123", password: "encrypted...", folder: 3, isPinned: false, description: "Savings account" }
    ]
    property var folders: [
        { id: 0, name: "All Passwords", icon: "☆", count: 3 },
        { id: 1, name: "Email", icon: "✉", count: 1 },
        { id: 2, name: "Social Media", icon: "◉", count: 1 },
        { id: 3, name: "Banking", icon: "⚙", count: 1 }
    ]
    
    Rectangle {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 60
        color: Constants.primaryMedium
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: Constants.spacingMedium
            spacing: Constants.spacingMedium

            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                Layout.minimumWidth: 32
                Layout.minimumHeight: 32
                radius: Constants.radiusMedium
                color: Constants.accentBlue

                Image {
                    width: 16
                    height: 16
                    anchors.centerIn: parent
                    source: "qrc:/resources/icons/shield_black.svg"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }
            }

            Text {
                text: "BitPass"
                font.pixelSize: Constants.fontSizeLarge
                font.bold: true
                color: Constants.accentBlue
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: "⚙ Settings"
                contentItem: Text {
                    text: parent.text
                    color: Constants.textLight
                    font.pixelSize: Constants.fontSizeBase
                }
                background: Rectangle {
                    color: Constants.primaryAccent
                    radius: Constants.radiusSmall
                }
                onClicked: settingsModal.open()
            }
            
            Button {
                text: "⎋ Logout"
                contentItem: Text {
                    text: parent.text
                    color: Constants.textLight
                    font.pixelSize: Constants.fontSizeBase
                }
                background: Rectangle {
                    color: Constants.accentRed
                    radius: Constants.radiusSmall
                }
                onClicked: PasswordModel.lock();
            }
        }
    }
    
    RowLayout {
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        spacing: 0
        
        FolderSidebar {
            id: folderSidebar
            selectedFolderId: dashboardPage.selectedFolderId
            folders: dashboardPage.folders
            
            onFolderSelected: (folderId) => {
                dashboardPage.selectedFolderId = folderId
            }
            
            onNewFolderRequested: newFolderModal.open()
        }
        
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Constants.primaryDark
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Constants.spacingLarge
                spacing: Constants.spacingMedium

                StyledTextField {
                    id: searchInput

                    Layout.fillWidth: true

                    font.pixelSize: Constants.fontSizeBase
                    placeholderText: "🔍 Search passwords..."
                }

                Button {
                    Layout.fillWidth: true
                    height: 40
                    text: "+ Add Password"
                    contentItem: Text {
                        text: parent.text
                        color: Constants.textLight
                        font.pixelSize: Constants.fontSizeBase
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                    }
                    background: Rectangle {
                        color: Constants.accentGreen
                        radius: Constants.radiusSmall
                    }
                    onClicked: addPasswordModal.open()
                }
                
                PasswordListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    passwords: dashboardPage.passwords
                    
                    onEditPassword: (passwordItem) => {
                        console.log("Edit:", passwordItem.service)
                    }
                    
                    onDeletePassword: (id) => {
                        console.log("Delete:", id)
                    }
                }
            }
        }
    }
    
    AddPasswordModal {
        id: addPasswordModal
        
        onPasswordAdded: (passwordData) => {
            console.log("Password added:", passwordData.service)
        }
    }
    
    SettingsModal {
        id: settingsModal
        
        onMasterPasswordChanged: {
            console.log("Master password changed")
        }
        
        onDataExported: (format) => {
            console.log("Data exported as:", format)
        }
        
        onDataImported: {
            console.log("Data imported")
        }
    }
    
    Dialog {
        id: newFolderModal
        title: "New Folder"
        anchors.centerIn: parent
        width: 400
        
        contentItem: ColumnLayout {
            spacing: Constants.spacingMedium
            
            TextField {
                id: folderNameInput
                Layout.fillWidth: true
                placeholderText: "Folder Name"
                background: Rectangle {
                    color: Constants.primaryMedium
                    radius: Constants.radiusSmall
                }
            }
            
            RowLayout {
                Layout.fillWidth: true
                spacing: Constants.spacingMedium
                
                Button {
                    Layout.fillWidth: true
                    text: "Create"
                    background: Rectangle {
                        color: Constants.accentGreen
                        radius: Constants.radiusSmall
                    }
                    onClicked: newFolderModal.close()
                }
                
                Button {
                    Layout.fillWidth: true
                    text: "Cancel"
                    background: Rectangle {
                        color: Constants.primaryAccent
                        radius: Constants.radiusSmall
                    }
                    onClicked: newFolderModal.close()
                }
            }
        }
    }
}
