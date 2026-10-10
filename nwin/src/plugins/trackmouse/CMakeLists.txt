#######################################
# Effect
# Data files
install(FILES data/tm_inner.png data/tm_outer.png DESTINATION ${KDE_INSTALL_DATADIR}/nwin-wayland)

set(trackmouse_SOURCES
    main.cpp
    trackmouse.cpp
)

kconfig_add_kcfg_files(trackmouse_SOURCES
    trackmouseconfig.kcfgc
)

nwin_add_builtin_effect(trackmouse ${trackmouse_SOURCES})
target_link_libraries(trackmouse PRIVATE
    nwin

    KF6::ConfigGui
    KF6::GlobalAccel
    KF6::I18n
)

#######################################
# Config
if (NWIN_BUILD_KCMS)
    set(nwin_trackmouse_config_SRCS trackmouse_config.cpp)
    ki18n_wrap_ui(nwin_trackmouse_config_SRCS trackmouse_config.ui)
    kconfig_add_kcfg_files(nwin_trackmouse_config_SRCS trackmouseconfig.kcfgc)

    nwin_add_effect_config(nwin_trackmouse_config ${nwin_trackmouse_config_SRCS})

    target_link_libraries(nwin_trackmouse_config
        KF6::KCMUtils
        KF6::CoreAddons
        KF6::GlobalAccel
        KF6::I18n
        KF6::XmlGui
        NWinEffectsInterface
    )
endif()
