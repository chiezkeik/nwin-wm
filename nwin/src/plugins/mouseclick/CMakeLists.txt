#######################################
# Effect

set(mouseclick_SOURCES
    main.cpp
    mouseclick.cpp
)

kconfig_add_kcfg_files(mouseclick_SOURCES
    mouseclickconfig.kcfgc
)

nwin_add_builtin_effect(mouseclick ${mouseclick_SOURCES})
target_link_libraries(mouseclick PRIVATE
    nwin

    KF6::ConfigGui
    KF6::GlobalAccel
    KF6::I18n
)

##########################
## configuration dialog
##########################
if (NWIN_BUILD_KCMS)
    set(nwin_mouseclick_config_SRCS mouseclick_config.cpp)
    ki18n_wrap_ui(nwin_mouseclick_config_SRCS mouseclick_config.ui)
    kconfig_add_kcfg_files(nwin_mouseclick_config_SRCS mouseclickconfig.kcfgc)

    nwin_add_effect_config(nwin_mouseclick_config ${nwin_mouseclick_config_SRCS})

    target_link_libraries(nwin_mouseclick_config
        KF6::KCMUtils
        KF6::CoreAddons
        KF6::GlobalAccel
        KF6::I18n
        KF6::XmlGui
        NWinEffectsInterface
    )
endif()
