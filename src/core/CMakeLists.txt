qt_add_library(core STATIC 
    src/DiskImageCache.cpp
    src/EventsHandler.cpp
    src/QueueManager.cpp
    src/ICache.cpp

    include/DiskImageCache.h
    include/EventsHandler.h
    include/QueueManager.h
    include/ICache.h
    include/ISingleton.h
    include/IDiskCache.h
)

target_include_directories(core PUBLIC include)

target_link_libraries(core PRIVATE common tags Qt6::Core Qt6::Gui)
