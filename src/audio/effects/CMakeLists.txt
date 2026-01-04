find_package(Qt6 REQUIRED COMPONENTS Core Multimedia)

message(STATUS "Effect plugins will be installed to ${EFFECT_PLUGIN_INSTALL_DIR}")

set(EFFECT_SOURCES
  GainEffect.cpp
  BassBoostEffect.cpp
  EqualizerEffect.cpp
  ReverbEffect.cpp
)

foreach(src IN LISTS EFFECT_SOURCES)
  get_filename_component(name ${src} NAME_WE)

  add_library(${name} SHARED ${src})

  target_include_directories(${name} PRIVATE include)

  target_link_libraries(${name} PRIVATE Qt6::Core Qt6::Multimedia effect_plugin_common)

  set_target_properties(${name} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/effects
  )

  install(TARGETS ${name}
    LIBRARY DESTINATION ${EFFECT_PLUGIN_INSTALL_DIR}
    RUNTIME DESTINATION ${EFFECT_PLUGIN_INSTALL_DIR}
  )
endforeach()

add_subdirectory(common)
