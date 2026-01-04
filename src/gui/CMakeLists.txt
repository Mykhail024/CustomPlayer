set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

# find_package(Qt6 REQUIRED COMPONENTS Qml Quick QuickControls2 LinguistTools)
#
# set(translation_files
#     translations/CustomPlayer_uk.ts
# )

add_subdirectory(utils)
add_subdirectory(presentation)
add_subdirectory(translations)

# add_library(gui SHARED
#     presentation/QmlResources.qrc
#     styles/StyleResources.qrc
#     icons/IconsResources.qrc
# )

# target_compile_definitions(gui PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

# qt_add_translations(gui TS_FILES ${translation_files} RESOURCE_PREFIX "/translations" INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR})

# set_target_properties(gui PROPERTIES
#     OUTPUT_NAME "customplayer-gui"
# )

# target_link_libraries(gui PRIVATE common core Qt6::Core Qt6::Widgets Qt6::Qml Qt6::Quick Qt6::QuickControls2)
#
# if(UNIX)
#     install(TARGETS gui
#             DESTINATION lib)
# endif()


# if(ANDROID)
# add_custom_target(install_apk
#     COMMAND adb install -r "${CMAKE_BINARY_DIR}/src/ui/android-build/ui.apk"
#     COMMENT "Installing APK on the device"
#     DEPENDS ${CMAKE_BINARY_DIR}/src/ui/android-build/ui.apk
# )
# add_custom_target(run_apk
#     COMMAND adb shell am start -n "org.mykhail024.customplayer/org.qtproject.qt.android.bindings.QtActivity"
#     COMMENT "Running APK on the device"
#     DEPENDS ${CMAKE_BINARY_DIR}/src/ui/android-build/ui.apk
# )
# endif()
#
# install(TARGETS gui DESTINATION /usr/bin)
# install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/icon48x48.png
#         DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/apps/
#         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
#         RENAME customplayer.png)
# install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/icon.svg
#         DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps/
#         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
#         RENAME customplayer.svg)
# install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/customplayer.desktop
#         DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/
#         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
#
#
# qt_finalize_executable(gui)
