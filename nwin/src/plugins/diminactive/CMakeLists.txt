#######################################
# Effect

set(diminactive_SOURCES
    diminactive.cpp
    main.cpp
)

kconfig_add_kcfg_files(diminactive_SOURCES
    diminactiveconfig.kcfgc
)

nwin_add_builtin_effect(diminactive ${diminactive_SOURCES})
target_link_libraries(diminactive PRIVATE
    nwin

    KF6::ConfigGui
)

#######################################
# Config
if (NWIN_BUILD_KCMS)
    set(nwin_diminactive_config_SRCS diminactive_config.cpp)
    ki18n_wrap_ui(nwin_diminactive_config_SRCS diminactive_config.ui)
    kconfig_add_kcfg_files(nwin_diminactive_config_SRCS diminactiveconfig.kcfgc)

    nwin_add_effect_config(nwin_diminactive_config ${nwin_diminactive_config_SRCS})

    target_link_libraries(nwin_diminactive_config
        KF6::KCMUtils
        KF6::CoreAddons
        KF6::I18n
        Qt::DBus
        NWinEffectsInterface
    )
endif()
