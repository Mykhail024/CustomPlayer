set(SINGLETONE_QML_FILES
    Colors.qml
)

set_source_files_properties(${SINGLETONE_QML_FILES} PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(style
    STATIC
    VERSION 1.0
    URI "CustomPlayer"
    QML_FILES
        Button.qml
        Slider.qml
        ${SINGLETONE_QML_FILES}
    RESOURCE_PREFIX /Style
    NO_GENERATE_EXTRA_QMLDIRS
)
