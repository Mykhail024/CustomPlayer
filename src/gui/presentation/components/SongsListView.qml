// pragma ComponentBehavior: Bound;

import QtQuick
import QtQuick.Controls

import CustomPlayer.Audio

ListView {
    id: songsView
    clip: true
    spacing: 10
    cacheBuffer: 10
    // reuseItems: true
    keyNavigationEnabled: true
    keyNavigationWraps: true
    headerPositioning: ListView.PullBackHeader

    header: SongsListViewHeader{
        onFilterStringChanged: {
            model.filterString = filterString
        }
    }

    // currentIndex: model.currentIndex

    // onModelChanged: {
    //     Qt.callLater(() => {
    //         const playlistId = playlistsModel.currentIndex;
    //
    //         if (!songsView.model || playlistId < 0) return;
    //
    //         if (playlistId in playlistsModel.savedPositions) {
    //             songsView.contentY = playlistsModel.savedPositions[playlistId];
    //         }
    //     });
    // }

    delegate: SongsListDelegate {
        onDoubleClicked: (index) => {
            ListView.view.currentIndex = index
        }
    }


    ScrollBar.vertical: ScrollBar { 
        id: scrollBar

        width: 10
        minimumSize: 0.06
    }

    Component.onCompleted: {
        MusicPlayer.endOfFile.connect(function() {
            ListView.view.currentIndex = (ListView.view.currentIndex + 1) % model.rowCount();
            console.log(model.name);
        });
    }
}

