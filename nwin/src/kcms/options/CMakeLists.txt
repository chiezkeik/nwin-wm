########### next target ###############
# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmkwm\")

set(kcm_nwinoptions_PART_SRCS
    main.cpp
    mouse.cpp
    windows.cpp
)

ki18n_wrap_ui(kcm_nwinoptions_PART_SRCS
    actions.ui
    advanced.ui
    focus.ui
    mouse.ui
    moving.ui
    pip.ui
)

kcmutils_generate_module_data(
    kcm_nwinoptions_PART_SRCS
    MODULE_DATA_HEADER nwinoptionsdata.h
    MODULE_DATA_CLASS_NAME NWinOptionsData
    SETTINGS_HEADERS nwinoptions_settings.h nwinoptions_kdeglobals_settings.h
    SETTINGS_CLASSES NWinOptionsSettings NWinOptionsKDEGlobalsSettings
)

kconfig_add_kcfg_files(kcm_nwinoptions_PART_SRCS nwinoptions_settings.kcfgc GENERATE_MOC)
kconfig_add_kcfg_files(kcm_nwinoptions_PART_SRCS nwinoptions_kdeglobals_settings.kcfgc GENERATE_MOC)

qt_add_dbus_interface(kcm_nwinoptions_PART_SRCS ${NWin_SOURCE_DIR}/src/org.kde.NWin.Effects.xml nwin_effects_interface)

kcoreaddons_add_plugin(kcm_nwinoptions SOURCES ${kcm_nwinoptions_PART_SRCS} INSTALL_NAMESPACE "plasma/kcms/systemsettings_qwidgets")
kcmutils_generate_desktop_file(kcm_nwinoptions)
target_link_libraries(kcm_nwinoptions nwin Qt::DBus KF6::KCMUtils KF6::I18n KF6::WindowSystem)
