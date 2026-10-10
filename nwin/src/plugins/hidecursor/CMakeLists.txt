nwin_add_builtin_effect(hidecursor)

target_sources(hidecursor PRIVATE
    hidecursor.cpp
    main.cpp
)

kconfig_add_kcfg_files(hidecursor
    hidecursorconfig.kcfgc
)

target_link_libraries(hidecursor PRIVATE
    nwin

    KF6::ConfigGui
)

#######################################
# Config
if (NWIN_BUILD_KCMS)
    set(nwin_hidecursor_config_SRCS hidecursor_config.cpp)
    ki18n_wrap_ui(nwin_hidecursor_config_SRCS hidecursor_config.ui)
    kconfig_add_kcfg_files(nwin_hidecursor_config_SRCS hidecursorconfig.kcfgc)

    nwin_add_effect_config(nwin_hidecursor_config ${nwin_hidecursor_config_SRCS})

    target_link_libraries(nwin_hidecursor_config
        KF6::KCMUtils
        KF6::CoreAddons
        KF6::I18n
        KF6::XmlGui
        NWinEffectsInterface
    )
endif()
