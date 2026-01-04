add_subdirectory(style)

set(QML_FILES
    #root
    main.qml 

    #Controls
    controls/ControlsPanel.qml

    #Pages
    # pages/Favorites.qml
    # pages/Home.qml
    # pages/Library.qml
    pages/Settings.qml
    pages/Effects.qml

    #components
    components/RoundedImage.qml
    components/SongsListDelegate.qml
    components/SearchTextField.qml
    components/SongsListView.qml
    components/SongsListViewHeader.qml
    components/Sidebar.qml
    components/ObjectLoader.qml
    components/SettingsGroup.qml

    #Dialogs
    dialogs/CreatePlaylistDialog.qml
    dialogs/AudioFileDialog.qml
)

qt_add_qml_module(presentation
    STATIC
    VERSION 1.0
    URI "CustomPlayer.Presentation"
    QML_FILES
        ${QML_FILES}
    RESOURCE_PREFIX /
    NO_GENERATE_EXTRA_QMLDIRS
)
