#######################################
# Effect

set(colorpicker_SOURCES
    colorpicker.cpp
    colorpickerlayer.cpp
    colorpickersceneview.cpp
    main.cpp
)

nwin_add_builtin_effect(colorpicker ${colorpicker_SOURCES})
target_link_libraries(colorpicker PRIVATE
    nwin

    KF6::I18n

    Qt::DBus
)
