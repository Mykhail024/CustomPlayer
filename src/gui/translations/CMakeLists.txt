qt_add_library(translations STATIC)

set(TRANSLATION_FILES
    CustomPlayer_uk.ts
    CustomPlayer_en.ts
)

qt6_add_translations(translations TS_FILES ${TRANSLATION_FILES} RESOURCE_PREFIX "/translations" INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR})

function(set_up_translations)
    qt_collect_translation_source_targets(TRANSLATION_TARGETS)
    qt_add_lupdate(
        SOURCE_TARGETS ${TRANSLATION_TARGETS}
        LUPDATE_TARGET update_translations
    )
endfunction()

cmake_language(DEFER CALL set_up_translations)
