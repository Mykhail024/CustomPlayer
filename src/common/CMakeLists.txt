qt_add_qml_module(common
    STATIC
    VERSION 1.0
    URI "CustomPlayer.Common"
    SOURCES
        include/Log.h
        include/SongMetadata.h
        src/Utils.cpp include/Utils.h
    RESOURCE_PREFIX /
)

target_include_directories(common PUBLIC include)
target_link_libraries(common PRIVATE Qt6::Core)
