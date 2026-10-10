# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmnwinscreenedges\")

set(kcm_screenedges_SRCS
    monitor.cpp
    screenpreviewwidget.cpp
    nwinscreenedge.cpp
    nwinscreenedgeconfigform.cpp
)

qt_add_dbus_interface(kcm_screenedges_SRCS ${NWin_SOURCE_DIR}/src/org.kde.NWin.Effects.xml nwin_effects_interface)

set(kcm_nwinscreenedges_PART_SRCS main.cpp ${kcm_screenedges_SRCS})

kcmutils_generate_module_data(
    kcm_nwinscreenedges_PART_SRCS
    MODULE_DATA_HEADER nwinscreenedgedata.h
    MODULE_DATA_CLASS_NAME NWinScreenEdgeData
    SETTINGS_HEADERS nwinscreenedgesettings.h
    SETTINGS_CLASSES NWinScreenEdgeSettings
    NAMESPACE NWin
)

ki18n_wrap_ui(kcm_nwinscreenedges_PART_SRCS main.ui)
kconfig_add_kcfg_files(kcm_nwinscreenedges_PART_SRCS nwinscreenedgesettings.kcfgc nwinscreenedgescriptsettings.kcfgc nwinscreenedgeeffectsettings.kcfgc)
kcoreaddons_add_plugin(kcm_nwinscreenedges SOURCES ${kcm_nwinscreenedges_PART_SRCS} INSTALL_NAMESPACE "plasma/kcms/systemsettings_qwidgets")
set(kcm_screenedges_LIBS
    Qt::DBus

    KF6::ConfigCore
    KF6::KCMUtils
    KF6::I18n
    KF6::Package
    KF6::Svg
)
target_link_libraries(kcm_nwinscreenedges ${X11_LIBRARIES} ${kcm_screenedges_LIBS})

qt_add_dbus_interface(kcm_nwintouchscreen_dbus_SRCS ${NWin_SOURCE_DIR}/src/org.kde.NWin.InputDeviceManager.xml inputdevicemanager_interface)

set(kcm_nwintouchscreenedges_PART_SRCS
    touch.cpp
    nwintouchscreenedgeconfigform.cpp
    nwintouchscreenmoduledata.cpp
    ${kcm_screenedges_SRCS}
    ${kcm_nwintouchscreen_dbus_SRCS}
)

kcmutils_generate_module_data(
    kcm_nwintouchscreenedges_PART_SRCS
    MODULE_DATA_HEADER nwintouchscreendata.h
    MODULE_DATA_CLASS_NAME NWinTouchScreenData
    SETTINGS_HEADERS nwintouchscreensettings.h
    SETTINGS_CLASSES NWinTouchScreenSettings
    NAMESPACE NWin
)

ki18n_wrap_ui(kcm_nwintouchscreenedges_PART_SRCS main.ui touch.ui)
kconfig_add_kcfg_files(kcm_nwintouchscreenedges_PART_SRCS nwintouchscreensettings.kcfgc nwintouchscreenscriptsettings.kcfgc nwintouchscreenedgeeffectsettings.kcfgc)
kcoreaddons_add_plugin(kcm_nwintouchscreen SOURCES ${kcm_nwintouchscreenedges_PART_SRCS} INSTALL_NAMESPACE "plasma/kcms/systemsettings_qwidgets")
target_link_libraries(kcm_nwintouchscreen ${X11_LIBRARIES} ${kcm_screenedges_LIBS})
