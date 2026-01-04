qt_add_qml_module(qml_utils
    STATIC
    VERSION 1.0
    URI "CustomPlayer.Utils"
    SOURCES
        src/Object.cpp include/Object.h
    RESOURCE_PREFIX /
)

target_include_directories(qml_utils PUBLIC include)
target_link_libraries(qml_utils PRIVATE Qt6::Core Qt6::Qml)
