#######################################
# Effect

set(invert_SOURCES
    invert.cpp
    invert.qrc
    main.cpp
)

nwin_add_builtin_effect(invert ${invert_SOURCES})
target_link_libraries(invert PRIVATE
    nwin

    KF6::GlobalAccel
    KF6::I18n
)
