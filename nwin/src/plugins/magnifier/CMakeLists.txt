#######################################
# Effect

set(magnifier_SOURCES
    magnifier.cpp
    main.cpp
)

kconfig_add_kcfg_files(magnifier_SOURCES
    magnifierconfig.kcfgc
)

nwin_add_builtin_effect(magnifier ${magnifier_SOURCES})
target_link_libraries(magnifier PRIVATE
    nwin

    KF6::ConfigGui
    KF6::GlobalAccel
    KF6::I18n
)
