qt_add_qml_module(cpp_models
    STATIC
    VERSION 1.0
    URI "CustomPlayer.Models"
    SOURCES
        src/PlaylistModel.cpp include/PlaylistModel.h
        src/PlaylistsModel.cpp include/PlaylistsModel.h
        src/PlaylistProvidersRegistry.cpp include/PlaylistProvidersRegistry.h
        src/FilterProxyModel.cpp include/FilterProxyModel.h
    RESOURCE_PREFIX /
)

target_include_directories(cpp_models PUBLIC include)

target_link_libraries(cpp_models PRIVATE common core tags playlist_provider_plugin_common)

add_subdirectory(providers)
