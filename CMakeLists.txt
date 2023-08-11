cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME Player)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

project(${PROJECT_NAME})

find_package(Qt6 COMPONENTS Core Widgets DBus REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MPG123 REQUIRED IMPORTED_TARGET libmpg123)
pkg_check_modules(libpipewire REQUIRED IMPORTED_TARGET libpipewire-0.3)
pkg_check_modules(taglib REQUIRED IMPORTED_TARGET taglib)

#qt_add_dbus_interface(
#    GENERATED_SOURCES
#    src/dbus/org.mpris.MediaPlayer2.xml
#    src/dbus/org.mpris.MediaPlayer2.hpp
#)
#qt_add_dbus_interface(
#	GENERATED_SOURCES_PLAYER
#    src/dbus/org.mpris.MediaPlayer2.Player.xml
#    src/dbus/org.mpris.MediaPlayer2.Player.hpp
#)

# Move generated header to the desired directory
#set_source_files_properties(${GENERATED_SOURCES} ${GENERATED_SOURCES_PLAYER} PROPERTIES
#    GENERATED TRUE
#    HEADER_FILE_ONLY TRUE
#    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dbus"
#)

#add_library(DBusInterfaceLib STATIC ${GENERATED_SOURCES})

add_executable(${PROJECT_NAME}
	res/Resources.qrc
    src/Player.cpp
    src/Widgets/ControlsPanel.cpp
    src/Widgets/InfoPanel.cpp
    src/Widgets/ListPanel.cpp
    src/Widgets/Window.cpp
	src/Convert.cpp
	src/Config/Config.cpp
	src/Widgets/Controls/TimeSlider.cpp
	src/TagReaders/ID3v2Reader.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${MPG123_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Core Qt::Widgets Qt::DBus PkgConfig::MPG123 PkgConfig::libpipewire PkgConfig::taglib)

add_custom_target(run
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
    COMMENT "Running executable..."
)

