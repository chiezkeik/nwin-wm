#######################################
# Effect

set(magiclamp_SOURCES
    magiclamp.cpp
    main.cpp
)

kconfig_add_kcfg_files(magiclamp_SOURCES
    magiclampconfig.kcfgc
)

nwin_add_builtin_effect(magiclamp ${magiclamp_SOURCES})
target_link_libraries(magiclamp PRIVATE
    nwin

    KF6::ConfigGui
)

#######################################
# Config
if (NWIN_BUILD_KCMS)
    set(nwin_magiclamp_config_SRCS magiclamp_config.cpp)
    ki18n_wrap_ui(nwin_magiclamp_config_SRCS magiclamp_config.ui)
    kconfig_add_kcfg_files(nwin_magiclamp_config_SRCS magiclampconfig.kcfgc)

    nwin_add_effect_config(nwin_magiclamp_config ${nwin_magiclamp_config_SRCS})

    target_link_libraries(nwin_magiclamp_config
        KF6::KCMUtils
        KF6::CoreAddons
        KF6::I18n
        Qt::DBus
        NWinEffectsInterface
    )
endif()
