set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

set(PROJECT_SOURCES
    resources/Resources.qrc

    Application.cpp
    MainWindow.cpp
    ControlsPanel.cpp

    Custom/PushButton.cpp
)

qt_add_executable(ui
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(ui PRIVATE Qt6::Core Qt6::Widgets)

set_target_properties(ui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_NAME "customplayer"
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)


if(ANDROID)
add_custom_target(install_apk
    COMMAND adb install -r "${CMAKE_BINARY_DIR}/src/ui/android-build/ui.apk"
    COMMENT "Installing APK on the device"
    DEPENDS ${CMAKE_BINARY_DIR}/src/ui/android-build/ui.apk
)
add_custom_target(run_apk
    COMMAND adb shell am start -n "org.mykhail024.customplayer/org.qtproject.qt.android.bindings.QtActivity"
    COMMENT "Running APK on the device"
    DEPENDS ${CMAKE_BINARY_DIR}/src/ui/android-build/ui.apk
)
endif()

install(TARGETS ui DESTINATION /usr/bin)
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


qt_finalize_executable(ui)
