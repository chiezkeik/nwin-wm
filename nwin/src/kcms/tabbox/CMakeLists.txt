# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_nwintabbox\")

set(kcm_nwintabbox_PART_SRCS
    ${NWin_SOURCE_DIR}/src/tabbox/tabboxconfig.cpp
    main.cpp
    nwintabboxconfigform.cpp
    nwintabboxdata.cpp
    shortcutsettings.cpp
)

ki18n_wrap_ui(kcm_nwintabbox_PART_SRCS main.ui)
qt_add_dbus_interface(kcm_nwintabbox_PART_SRCS ${NWin_SOURCE_DIR}/src/org.kde.NWin.Effects.xml nwin_effects_interface)

kconfig_add_kcfg_files(kcm_nwintabbox_PART_SRCS nwintabboxsettings.kcfgc nwinswitcheffectsettings.kcfgc)
kcoreaddons_add_plugin(kcm_nwintabbox SOURCES ${kcm_nwintabbox_PART_SRCS} INSTALL_NAMESPACE "plasma/kcms/systemsettings_qwidgets")

kcmutils_generate_desktop_file(kcm_nwintabbox)
target_link_libraries(kcm_nwintabbox
    KF6::GlobalAccel
    KF6::I18n
    KF6::KCMUtils
    KF6::NewStuffWidgets
    KF6::Package
    KF6::Service
    KF6::XmlGui # For kkeysequencewidget
    KF6::WidgetsAddons
)
if (NWIN_BUILD_X11)
    target_link_libraries(kcm_nwintabbox
        XCB::XCB
    )
endif()

# TabBox preview helper
add_executable(nwin-tabbox-preview
    layoutpreview.cpp
    thumbnailitem.cpp
)
qt_add_resources(nwin-tabbox-preview "thumbnails"
    PREFIX "/nwin-tabbox-preview"
    BASE "thumbnails"
    FILES
        thumbnails/falkon.png
        thumbnails/kmail.png
        thumbnails/systemsettings.png
        thumbnails/dolphin.png
        thumbnails/desktop.png
)
target_link_libraries(nwin-tabbox-preview
    Qt::Quick
    KF6::I18n
    KF6::I18nQml
)
install(TARGETS nwin-tabbox-preview DESTINATION ${KDE_INSTALL_LIBEXECDIR})

# KNewStuff
install(FILES nwinswitcher.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})
