qt_add_library(tags STATIC 
    src/ID3V2Handler.cpp include/ID3V2Handler.h
)

target_include_directories(tags PUBLIC include)

target_link_libraries(tags PRIVATE common PkgConfig::TagLib PUBLIC Qt6::Core Qt6::Gui)
