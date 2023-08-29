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

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
find_package(SQLiteCpp REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(SndFile)
find_package(sdbus-c++ REQUIRED)

pkg_check_modules(libpipewire REQUIRED IMPORTED_TARGET libpipewire-0.3)
pkg_check_modules(taglib REQUIRED IMPORTED_TARGET taglib)

add_executable(${PROJECT_NAME}
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
	src/Audio/PipeWire.cpp

	external/mpris-server/src/mpris_server.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${MPG123_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Core Qt::Widgets PkgConfig::taglib PkgConfig::libpipewire SQLiteCpp SndFile::sndfile SDBusCpp::sdbus-c++)

