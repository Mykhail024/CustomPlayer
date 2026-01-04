add_library(effect_plugin_common STATIC 
  src/EffectParameter.cpp
  include/EffectParameter.h
  include/IAudioEffect.h
)
set_target_properties(effect_plugin_common PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(effect_plugin_common PUBLIC include)
target_link_libraries(effect_plugin_common PRIVATE Qt6::Core Qt6::Multimedia)
