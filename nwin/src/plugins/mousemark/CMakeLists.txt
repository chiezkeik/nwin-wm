#######################################
# Effect

set(mousemark_SOURCES
    main.cpp
    mousemark.cpp
)

kconfig_add_kcfg_files(mousemark_SOURCES
    mousemarkconfig.kcfgc
)

nwin_add_builtin_effect(mousemark ${mousemark_SOURCES})
target_link_libraries(mousemark PRIVATE
    nwin

    KF6::ConfigGui
    KF6::GlobalAccel
    KF6::I18n
)

ecm_qt_declare_logging_category(mousemark
        HEADER mousemarklogging.h
        IDENTIFIER NWIN_MOUSEMARK
        CATEGORY_NAME nwin_effect_mousemark
        DEFAULT_SEVERITY Warning
)

#######################################
# Config
if (NWIN_BUILD_KCMS)
    set(nwin_mousemark_config_SRCS mousemark_config.cpp)
    ki18n_wrap_ui(nwin_mousemark_config_SRCS mousemark_config.ui)
    kconfig_add_kcfg_files(nwin_mousemark_config_SRCS mousemarkconfig.kcfgc)

    nwin_add_effect_config(nwin_mousemark_config ${nwin_mousemark_config_SRCS})

    target_link_libraries(nwin_mousemark_config
        KF6::KCMUtils
        KF6::CoreAddons
        KF6::GlobalAccel
        KF6::I18n
        KF6::XmlGui
        NWinEffectsInterface
    )
endif()
