cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME Player)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

project(${PROJECT_NAME})

find_package(Qt6 COMPONENTS Core Widgets DBus REQUIRED)
find_package(SQLiteCpp REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(SndFile)

pkg_check_modules(libpipewire REQUIRED IMPORTED_TARGET libpipewire-0.3)
pkg_check_modules(taglib REQUIRED IMPORTED_TARGET taglib)

set(source_files
	res/Resources.qrc
    src/Player.cpp
	src/Widgets/ControlsPanel.cpp
	src/Widgets/InfoPanel.cpp
	src/Widgets/ListPanel.cpp
	src/Widgets/ListControlsPanel.cpp
	src/Widgets/Window.cpp
	src/Convert.cpp
	src/Config/Config.cpp
	src/Widgets/Controls/TimeSlider.cpp
	src/TagReaders/ID3v2Reader.cpp
	src/DataBase/SQLiteHandler.cpp
	src/Audio/AudioServer.h
)

if(UNIX AND NOT APPLE)
	list(APPEND source_files
		src/Audio/PipeWire.cpp
		src/DBus/MediaPlayer2.cpp
	)

	qt6_add_dbus_adaptor(source_files
		./src/DBus/org.mpris.MediaPlayer2.xml
		../src/DBus/MediaPlayer2.h
		MediaPlayer2
		mprisroot MPrisRoot)
	qt6_add_dbus_adaptor(source_files
		./src/DBus/org.mpris.MediaPlayer2.Player.xml
		../src/DBus/MediaPlayer2.h
		MediaPlayer2
		mprisplayer MPrisPlayer)
endif()

add_executable(${PROJECT_NAME} ${source_files})

target_include_directories(${PROJECT_NAME} PRIVATE ${MPG123_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Core Qt::Widgets Qt::DBus PkgConfig::taglib PkgConfig::libpipewire SQLiteCpp SndFile::sndfile)

