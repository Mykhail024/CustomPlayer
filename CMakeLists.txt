cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(CustomPlayer VERSION 0.1 LANGUAGES CXX)

include(GNUInstallDirs)

project(CustomPlayer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(
        /W4
        /WX
        /permissive-
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Debug>:/Zi>
        $<$<CONFIG:Release>:/O2>
    )
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Debug>:-g3>
        $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
        $<$<CONFIG:Release>:-O2>
    )
endif()

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(QT_QML_GENERATE_QMLLS_INI ON)
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(TSAN OFF CACHE STRING "ThreadSanitizer support")

string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type_lower)
if(build_type_lower STREQUAL "debug")
  message(STATUS "Building in debug mode")

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
  endif()

  if(TSAN)
    include(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=thread")
    check_cxx_source_compiles("int main() { return 0; }" HAS_TSAN_WORKAROUND)
    unset(CMAKE_REQUIRED_FLAGS)

    if(HAS_TSAN_WORKAROUND)
      message(STATUS "Building with ThreadSanitizer support")
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=thread")
      set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=thread")
    endif()
  endif()
else()
  message(STATUS "Building in release mode")
endif()

include_directories("${CMAKE_BINARY_DIR}")

find_package(Qt6 REQUIRED COMPONENTS Core Gui QuickControls2 LinguistTools)

add_subdirectory(src)

configure_file(qmlls.ini.in ${CMAKE_SOURCE_DIR}/.qmlls.ini)

if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    file(COPY "${CMAKE_BINARY_DIR}/compile_commands.json"
        DESTINATION "${CMAKE_SOURCE_DIR}")
    message(STATUS "Copied compile_commands.json to source dir")
endif()
