import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Controls 2.15
import QtQuick.Effects
import CustomPlayer.Audio
import CustomPlayer.Common

import CustomPlayer

Rectangle {
    color: Colors.background
    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: Colors.shadow
        shadowVerticalOffset: -0.1
        shadowBlur: 0.5
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15

        RowLayout {
            Layout.alignment: Qt.AlignCenter
            Button {
                id: shuffleButton
                icon.name: "media-playlist-shuffle"
                icon.color: Colors.text_p
            }
            Button {
                id: prevButton
                icon.name: "media-skip-backward"
                icon.color: Colors.text_p

                enabled: MusicPlayer.playbackState != MusicPlayer.StoppedState
            }

            Button {
                id: playButton

                enabled: MusicPlayer.playbackState != MusicPlayer.StoppedState

                icon.name: {
                    switch(MusicPlayer.playbackState) {
                        case MusicPlayer.PlayingState: return "media-playback-pause"
                        case MusicPlayer.PausedState: return "media-playback-start"
                        case MusicPlayer.StoppedState: return "media-playback-stop"
                    }
                }
                icon.color: Colors.text_p

                icon.width: prevButton.width * 1.05
                icon.height: prevButton.width * 1.05

                onClicked: {
                    if(MusicPlayer.playbackState == MusicPlayer.PlayingState) {
                        MusicPlayer.pause()
                    } else {
                        MusicPlayer.resume()
                    }
                    
                }
            }
            Button {
                id: nextButton
                icon.name: "media-skip-forward"
                icon.color: Colors.text_p

                enabled: MusicPlayer.playbackState != MusicPlayer.StoppedState
            }
            Button {
                id: repeatButton
                icon.name: MusicPlayer.loop ? "media-playlist-repeat-song" : "media-playlist-repeat"
                icon.color: Colors.text_p

                onClicked: {
                    MusicPlayer.loop = !MusicPlayer.loop
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Item { Layout.fillWidth: true }
            Text {
                Layout.leftMargin: volumeSlider.width + volumeButton.width
                text: MusicPlayer.playbackState == MusicPlayer.StoppedState ? "00:00" : Utils.msecToDuration(MusicPlayer.position)
                color: Colors.text_p
            }
            Slider {
                id: timelineSlider
                Layout.preferredWidth: Window.width * 0.4
                from: 0
                to: MusicPlayer.duration
                value: MusicPlayer.position
                onPressedChanged: {
                    if(pressed) {
                        MusicPlayer.live = false
                    } else {
                        MusicPlayer.position = value
                        MusicPlayer.live = true
                    }
                }

                enabled: MusicPlayer.playbackState != MusicPlayer.StoppedState
            }
            Text {
                text: MusicPlayer.playbackState == MusicPlayer.StoppedState ? "00:00" : Utils.msecToDuration(MusicPlayer.duration)
                color: Colors.text_p
            }
            Item { Layout.fillWidth: true }
            Button {
                id: volumeButton
                Layout.preferredWidth: 32
                Layout.preferredHeight: 32
                Layout.rightMargin: -12
                icon.name: "audio-volume-muted"

                onClicked: {
                    MusicPlayer.muted = !MusicPlayer.muted
                }

                states: [
                    State {
                        name: "muted"; when: volumeSlider === 0.0 || MusicPlayer.muted
                        PropertyChanges { volumeButton.icon.name: "audio-volume-muted"}
                    },
                    State {
                        name: "low"; when: volumeSlider.value > 0.0 && volumeSlider.value <= 0.33
                        PropertyChanges { volumeButton.icon.name: "audio-volume-low"}
                    },
                    State {
                        name: "medium"; when: volumeSlider.value > 0.33 && volumeSlider.value <= 0.66
                        PropertyChanges { volumeButton.icon.name: "audio-volume-medium"}
                    },
                    State {
                        name: "high"; when: volumeSlider.value > 0.66
                        PropertyChanges { volumeButton.icon.name: "audio-volume-high"}
                    }
                ]
            }
            Slider {
                id: volumeSlider
                Layout.alignment: Qt.AlignRight
                Layout.preferredWidth: 120

                from: 0.0
                to: 1.0
                value: MusicPlayer.volume

                wheelEnabled: true
                stepSize: 0.05

                onValueChanged: {
                    MusicPlayer.volume = value
                }
                ToolTip {
                    parent: volumeSlider.handle
                    visible: volumeSlider.hovered
                    text: Number(volumeSlider.value).toFixed(2)
                }
            }
        }
    }
}
