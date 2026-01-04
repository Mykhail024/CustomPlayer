macro(add_playlist_provider)
    set(_provider_name "")
    set(_provider_sources "")
    set(_install_dir "")

    set(_arg_mode "")
    foreach(arg ${ARGV})
        if("${arg}" STREQUAL "PROVIDER_NAME")
            set(_arg_mode "name")
        elseif("${arg}" STREQUAL "PROVIDER_SOURCES")
            set(_arg_mode "sources")
        elseif("${arg}" STREQUAL "INSTALL_DIR")
            set(_arg_mode "install")
        else()
            if("${_arg_mode}" STREQUAL "name")
                set(_provider_name "${arg}PlaylistProvider")
            elseif("${_arg_mode}" STREQUAL "sources")
                list(APPEND _provider_sources "${arg}")
            elseif("${_arg_mode}" STREQUAL "install")
                set(_install_dir "${arg}")
            else()
                message(FATAL_ERROR "Unknown argument or missing keyword before '${arg}'")
            endif()
        endif()
    endforeach()

    if("${_provider_name}" STREQUAL "")
        message(FATAL_ERROR "PROVIDER_NAME is not set")
    endif()
    if("${_provider_sources}" STREQUAL "")
        message(FATAL_ERROR "PROVIDER_SOURCES is not set")
    endif()
    if("${_install_dir}" STREQUAL "")
        message(FATAL_ERROR "INSTALL_DIR is not set")
    endif()

    add_library(${_provider_name} SHARED ${_provider_sources})

    target_link_libraries(${_provider_name} PRIVATE
        Qt6::Core
        Qt6::Multimedia
        Qt6::Gui
        Qt6::QuickControls2
        playlist_provider_plugin_common
    )

    set_target_properties(${_provider_name} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/playlist_providers
    )

    install(TARGETS ${_provider_name}
        LIBRARY DESTINATION ${_install_dir}
        RUNTIME DESTINATION ${_install_dir}
    )
endmacro()
