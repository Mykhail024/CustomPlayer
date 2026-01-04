qt_add_qml_module(audio
    STATIC
    VERSION 1.0
    URI "CustomPlayer.Audio"
    SOURCES
        include/MusicPlayer.h src/MusicPlayer.cpp
        include/FFmpegAudioDevice.h src/FFmpegAudioDevice.cpp
        include/AudioEffectManager.h src/AudioEffectManager.cpp
    RESOURCE_PREFIX /
)

target_include_directories(audio PUBLIC include)

target_link_libraries(audio PRIVATE Qt6::Multimedia PkgConfig::LIBAV common effect_plugin_common)

target_compile_options(audio PRIVATE -Wno-c99-extensions -Wno-mismatched-tags)

add_subdirectory(effects)
