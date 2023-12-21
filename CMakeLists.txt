cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME Player)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

project(${PROJECT_NAME})

set(PCH_HEADER precompiled.h)

set(source_files
	res/Resources.qrc
    src/Player.cpp
	src/Widgets/ControlsPanel.cpp
	src/Widgets/Window.cpp
	src/Convert.cpp
	src/Config/Config.cpp
	src/Widgets/Controls/TimeSlider.cpp
	src/Widgets/Controls/LineEdit.h
	src/TagReaders/ID3v2Reader.cpp
	src/Audio/AudioServer.h
	src/Core/EventHandler.cpp
	src/Core/Globals.cpp
	src/Core/PlaylistManager.cpp
	src/Core/PlaylistModel.cpp
	src/Core/SortFilterProxyModel.cpp

	src/Widgets/SongsWidget.cpp
	src/Widgets/Playlist/PlaylistView.cpp
	src/Widgets/PlaylistControl.cpp
	src/Widgets/GlobalEventFilter.h
)

find_package(Qt6 COMPONENTS Core Widgets Sql REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(SndFile)

pkg_check_modules(taglib REQUIRED IMPORTED_TARGET taglib)

if(UNIX AND NOT APPLE)
	find_package(Qt6 COMPONENTS DBus REQUIRED)
	pkg_check_modules(libpipewire REQUIRED IMPORTED_TARGET libpipewire-0.3)

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

add_executable(${PROJECT_NAME} ${source_files} ${PCH_SOURCE})

target_precompile_headers(${PROJECT_NAME} PRIVATE ${PCH_HEADER})

target_include_directories(${PROJECT_NAME} PRIVATE ${MPG123_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Core Qt::Widgets Qt6::Sql PkgConfig::taglib SndFile::sndfile)

if(UNIX AND NOT APPLE)
	target_link_libraries(${PROJECT_NAME} PRIVATE Qt::DBus PkgConfig::libpipewire)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "customplayer")

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/icon48x48.png
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/apps/
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        RENAME customplayer.png)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/icon.svg
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps/
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        RENAME customplayer.svg)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/customplayer.desktop
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

