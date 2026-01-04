cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(CustomPlayer VERSION 0.0.1 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic-errors -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(ANDROID_PACKAGE_NAME org.mykhailo.customplayer)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
        message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
    endif()

    include(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=thread")
    check_cxx_source_compiles("int main() { return 0; }" HAS_TSAN_WORKAROUND)
    unset(CMAKE_REQUIRED_FLAGS)

    if(HAS_TSAN_WORKAROUND AND TSAN)
	message(STATUS "Building with ThreadSanitizer support")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=thread")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=thread")
    endif()
else()
  message(STATUS "Building in release mode")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui QuickControls2 LinguistTools Multimedia)
find_package(PkgConfig REQUIRED)
find_package(ZLIB REQUIRED)
#find_package(TagLib REQUIRED)
pkg_check_modules(TagLib REQUIRED IMPORTED_TARGET taglib)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavcodec 
    libavformat 
    libavutil
    libswresample
)

# include(FetchContent)
# FetchContent_Declare(taglib
#     GIT_REPOSITORY https://github.com/taglib/taglib.git
#     GIT_TAG  v2.0.2
#     GIT_SHALLOW    TRUE
# )
# set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
# set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(taglib)


include_directories("${CMAKE_BINARY_DIR}")


#Installs
if(APPLE)
    set(PLUGIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/CustomPlayer.app/Contents/PlugIns")
    set(EFFECT_PLUGIN_INSTALL_DIR "${PLUGIN_INSTALL_DIR}/effects")
    set(PLAYLIST_PROVIDER_PLUGIN_INSTALL_DIR "${PLUGIN_INSTALL_DIR}/playlist_providers")
elseif(WIN32)
    set(PLUGIN_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}/plugins")
    set(EFFECT_PLUGIN_INSTALL_DIR "${PLUGIN_INSTALL_DIR}/effects")
    set(PLAYLIST_PROVIDER_PLUGIN_INSTALL_DIR "${PLUGIN_INSTALL_DIR}/playlist_providers")
elseif(ANDROID)
    set(EFFECT_PLUGIN_INSTALL_DIR "/assets/plugins/effects")
    set(PLAYLIST_PROVIDER_PLUGIN_INSTALL_DIR "/assets/plugins/playlist_providers")
else()
    # Unix (Linux)
    set(PLUGIN_INSTALL_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/customplayer/plugins")
    set(EFFECT_PLUGIN_INSTALL_DIR "${PLUGIN_INSTALL_DIR}/effects")
    set(PLAYLIST_PROVIDER_PLUGIN_INSTALL_DIR "${PLUGIN_INSTALL_DIR}/playlist_providers")
endif()


if(APPLE)
    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE ${CMAKE_SOURCE_DIR}/README.md
        DESTINATION "${CMAKE_INSTALL_PREFIX}/CustomPlayer.app/Contents/Resources/doc"
    )
elseif(WIN32)
    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE ${CMAKE_SOURCE_DIR}/README.md
        DESTINATION "${CMAKE_INSTALL_PREFIX}/docs"
    )
elseif(UNIX AND NOT ANDROID)
    install(FILES ${CMAKE_SOURCE_DIR}/res/icon.svg
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps
        RENAME customplayer.svg
    )

    install(FILES ${CMAKE_SOURCE_DIR}/res/customplayer.desktop
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
    )

    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE ${CMAKE_SOURCE_DIR}/README.md
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/customplayer
    )
endif()

configure_file(qmlls.ini.in ${CMAKE_SOURCE_DIR}/.qmlls.ini) 
configure_file(
    "${CMAKE_SOURCE_DIR}/config.h.in"
    "${CMAKE_BINARY_DIR}/config.h"
)

#SubDirs
add_subdirectory(${CMAKE_SOURCE_DIR}/src)

# CPack
include(CPack)

set(CPACK_PACKAGE_NAME "CustomPlayer")
set(CPACK_PACKAGE_VENDOR "Mykhail024")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Custom Audio Player")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

if(WIN32)
  set(CPACK_GENERATOR "ZIP;NSIS64;NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "CustomPlayer")
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY "bin")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
elseif(UNIX)
  set(CPACK_GENERATOR "TGZ;DEB;RPM;IFW;STDZ")
endif()

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Mykhail024")

# set(CPACK_DEBIAN_PACKAGE_DEPENDS 
#     "qt6-base (>= 6.2.3), libavcodec58, libavformat58, libtag1v5"
# )
# set(CPACK_RPM_PACKAGE_REQUIRES 
#     "qt6-qtbase >= 6.2.3, libavcodec >= 58, libavformat >= 58, taglib >= 1.11"
# )
