#######################################
# Effect

# Source files
set(kscreen_SOURCES
    kscreen.cpp
    main.cpp
)

kconfig_add_kcfg_files(kscreen_SOURCES kscreenconfig.kcfgc)
nwin_add_builtin_effect(kscreen ${kscreen_SOURCES})
target_link_libraries(kscreen PRIVATE
    nwin

    KF6::ConfigGui
)
