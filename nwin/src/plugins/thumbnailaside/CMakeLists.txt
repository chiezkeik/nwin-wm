#######################################
# Effect

set(thumbnailaside_SOURCES
    main.cpp
    thumbnailaside.cpp
)

kconfig_add_kcfg_files(thumbnailaside_SOURCES
    thumbnailasideconfig.kcfgc
)

nwin_add_builtin_effect(thumbnailaside ${thumbnailaside_SOURCES})
target_link_libraries(thumbnailaside PRIVATE
    nwin

    KF6::ConfigGui
    KF6::GlobalAccel
    KF6::I18n
)

#######################################
# Config
if (NWIN_BUILD_KCMS)
    set(nwin_thumbnailaside_config_SRCS thumbnailaside_config.cpp)
    ki18n_wrap_ui(nwin_thumbnailaside_config_SRCS thumbnailaside_config.ui)
    kconfig_add_kcfg_files(nwin_thumbnailaside_config_SRCS thumbnailasideconfig.kcfgc)

    nwin_add_effect_config(nwin_thumbnailaside_config ${nwin_thumbnailaside_config_SRCS})

    target_link_libraries(nwin_thumbnailaside_config
        KF6::KCMUtils
        KF6::CoreAddons
        KF6::GlobalAccel
        KF6::I18n
        KF6::XmlGui
        NWinEffectsInterface
    )
endif()
