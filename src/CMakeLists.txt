qt_add_executable(customplayer main.cpp)

target_link_libraries(customplayer PRIVATE Qt6::Core Qt6::QuickControls2)

set_target_properties(customplayer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OUTPUT_NAME "customplayer"
)
install(TARGETS customplayer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
