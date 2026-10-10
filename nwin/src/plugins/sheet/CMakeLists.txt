#######################################
# Effect

# Source files
set(sheet_SOURCES
    main.cpp
    sheet.cpp
)
kconfig_add_kcfg_files(sheet_SOURCES sheetconfig.kcfgc)

nwin_add_builtin_effect(sheet ${sheet_SOURCES})
target_link_libraries(sheet PRIVATE
    nwin

    KF6::ConfigGui
)
