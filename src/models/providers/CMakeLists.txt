add_subdirectory(common)

include(cmake/PlaylistProviderPlugin.cmake)

add_playlist_provider(
  PROVIDER_NAME UPL 
  PROVIDER_SOURCES UPL/UPLPlaylistProvider.h UPL/UPLPlaylistProvider.cpp UPL/UPLPlaylistProviderFactory.cpp 
  INSTALL_DIR ${PLAYLIST_PROVIDER_PLUGIN_INSTALL_DIR}
)

add_playlist_provider(
  PROVIDER_NAME Network
  PROVIDER_SOURCES Network/NetworkPlaylistProvider.h 
                   Network/NetworkPlaylistProvider.cpp 
                   Network/NetworkPlaylistProviderFactory.cpp 
                   Network/NetworkDiscoverer.cpp
                   Network/NetworkDiscoverer.h
                   Network/NetworkBroadcaster.cpp
                   Network/NetworkBroadcaster.h
                   Network/resources.qrc
  INSTALL_DIR ${PLAYLIST_PROVIDER_PLUGIN_INSTALL_DIR}
)

