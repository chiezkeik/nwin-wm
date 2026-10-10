nwin_add_builtin_effect(zoom)

target_sources(zoom PRIVATE
    focustracker.cpp
    main.cpp
    textcarettracker.cpp
    zoom.cpp
    zoom.qrc
)

kconfig_add_kcfg_files(zoom zoomconfig.kcfgc)

target_link_libraries(zoom PRIVATE
    nwin

    KF6::ConfigGui
    KF6::GlobalAccel
    KF6::I18n
)

if (NWIN_BUILD_QACCESSIBILITYCLIENT)
    target_link_libraries(zoom PRIVATE QAccessibilityClient6)
endif()
