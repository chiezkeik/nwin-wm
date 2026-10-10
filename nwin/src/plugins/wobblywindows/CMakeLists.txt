#######################################
# Effect

set(wobblywindows_SOURCES
    main.cpp
    wobblywindows.cpp
)

kconfig_add_kcfg_files(wobblywindows_SOURCES
    wobblywindowsconfig.kcfgc
)

nwin_add_builtin_effect(wobblywindows ${wobblywindows_SOURCES})
target_link_libraries(wobblywindows PRIVATE
    nwin

    KF6::ConfigGui
)

#######################################
# Config
if (NWIN_BUILD_KCMS)
    set(nwin_wobblywindows_config_SRCS wobblywindows_config.cpp)
    ki18n_wrap_ui(nwin_wobblywindows_config_SRCS wobblywindows_config.ui)
    kconfig_add_kcfg_files(nwin_wobblywindows_config_SRCS wobblywindowsconfig.kcfgc)

    nwin_add_effect_config(nwin_wobblywindows_config ${nwin_wobblywindows_config_SRCS})

    target_link_libraries(nwin_wobblywindows_config
        KF6::KCMUtils
        KF6::CoreAddons
        KF6::I18n
        Qt::DBus
        NWinEffectsInterface
    )
endif()
