#######################################
# Effect

set(blur_SOURCES
    blur.cpp
    blur.qrc
    main.cpp
)

kconfig_add_kcfg_files(blur_SOURCES
    blurconfig.kcfgc
)

nwin_add_builtin_effect(blur ${blur_SOURCES})
target_link_libraries(blur PRIVATE
    nwin

    KF6::ConfigGui

    KDecoration3::KDecoration
)

#######################################
# Config
if (NWIN_BUILD_KCMS)
    set(nwin_blur_config_SRCS blur_config.cpp)
    ki18n_wrap_ui(nwin_blur_config_SRCS blur_config.ui)
    kconfig_add_kcfg_files(nwin_blur_config_SRCS blurconfig.kcfgc)

    nwin_add_effect_config(nwin_blur_config ${nwin_blur_config_SRCS})

    target_link_libraries(nwin_blur_config
        KF6::KCMUtils
        KF6::CoreAddons
        KF6::I18n
        Qt::DBus
        NWinEffectsInterface
    )
endif()
