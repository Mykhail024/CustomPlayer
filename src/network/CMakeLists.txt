qt_add_qml_module(network
    STATIC
    VERSION 1.0
    URI "CustomPlayer.Network"
    SOURCES
      src/BroadcastService.cpp include/BroadcastService.h
    RESOURCE_PREFIX /
)

target_include_directories(network PUBLIC include)

target_link_libraries(network PRIVATE common Qt6::Core Qt6::Network)
