cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set (PROJECT_NAME Player)

set (CMAKE_BUILD_TYPE "Debug")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

project(${PROJECT_NAME})

add_executable(${PROJECT_NAME}
	src/Player.cpp
	src/Widgets/MainWindow.cpp
	src/Convert.cpp
	res/Resources.qrc
	src/Decoders/MP3Decoder.cpp
	src/TagReaders/ID3v2Reader.cpp
	src/AudioServers/PipeWire.cpp
	src/Widgets/TimeSlider.cpp
)

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MPG123 REQUIRED IMPORTED_TARGET libmpg123)
pkg_check_modules(libpipewire REQUIRED IMPORTED_TARGET libpipewire-0.3)
pkg_check_modules(taglib REQUIRED IMPORTED_TARGET taglib)

target_include_directories(${PROJECT_NAME} PRIVATE ${MPG123_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Core Qt::Widgets PkgConfig::MPG123 PkgConfig::libpipewire PkgConfig::taglib)

add_custom_target(run
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
    COMMENT "Running executable..."
)
