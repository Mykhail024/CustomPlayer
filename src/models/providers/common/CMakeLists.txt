add_library(playlist_provider_plugin_common STATIC
        include/IPlaylistProvider.h
        include/IEditablePlaylistProvider.h
        include/IPlaylistProviderFactory.h
        include/IPlaylistProviderInterface.h
        include/PlaylistFormat.h
)
set_target_properties(playlist_provider_plugin_common PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(playlist_provider_plugin_common PUBLIC include)
target_link_libraries(playlist_provider_plugin_common PUBLIC common Qt6::Core Qt6::Gui)
