#######################################
# Effect

set(slide_SOURCES
    main.cpp
    slide.cpp
    springmotion.cpp
)

kconfig_add_kcfg_files(slide_SOURCES
    slideconfig.kcfgc
)

nwin_add_builtin_effect(slide ${slide_SOURCES})
target_link_libraries(slide PRIVATE
    nwin

    KF6::ConfigGui
)

#######################################
# Config
if (NWIN_BUILD_KCMS)
    set(nwin_slide_config_SRCS slide_config.cpp)
    ki18n_wrap_ui(nwin_slide_config_SRCS slide_config.ui)
    kconfig_add_kcfg_files(nwin_slide_config_SRCS slideconfig.kcfgc)

    nwin_add_effect_config(nwin_slide_config ${nwin_slide_config_SRCS})

    target_link_libraries(nwin_slide_config
        KF6::KCMUtils
        KF6::CoreAddons
        KF6::I18n
        Qt::DBus
        NWinEffectsInterface
    )
endif()
