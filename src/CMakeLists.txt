add_subdirectory(common)
# add_subdirectory(network)
add_subdirectory(models)
add_subdirectory(tags)
add_subdirectory(audio)
add_subdirectory(core)
add_subdirectory(gui)

qt_add_executable(customplayer main.cpp)

target_link_libraries(customplayer PRIVATE presentationplugin styleplugin cpp_models audio translations common tags core Qt6::Core Qt6::QuickControls2 Qt6::Multimedia)

if(ANDROID)
  set_property(TARGET customplayer APPEND PROPERTY
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/android
  )
endif()

if(ANDROID)
message(STATUS "Building for android")
add_custom_target(install_apk
    COMMAND adb install -r "${CMAKE_BINARY_DIR}/src/android-build/customplayer.apk"
    COMMENT "Installing APK on the device"
    DEPENDS ${CMAKE_BINARY_DIR}/src/android-build/customplayer.apk
)
add_custom_target(run_apk
    COMMAND adb shell am start -n "org.mykhailo.customplayer/org.qtproject.qt.android.bindings.QtActivity"
    COMMENT "Running APK on the device"
    DEPENDS ${CMAKE_BINARY_DIR}/src/android-build/customplayer.apk
	)
endif()

if(NOT ANDROID)
set_target_properties(customplayer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OUTPUT_NAME "customplayer"
  )
  install(TARGETS customplayer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
endif()
