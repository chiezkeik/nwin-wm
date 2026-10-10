# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_nwindecoration\")

add_subdirectory(declarative-plugin)

set(kcmnwindecoration_SRCS
    declarative-plugin/buttonsmodel.cpp
    decorationmodel.cpp
    kcm.cpp
    utils.cpp
)

kcmutils_generate_module_data(
    kcmnwindecoration_SRCS
    MODULE_DATA_HEADER nwindecorationdata.h
    MODULE_DATA_CLASS_NAME NWinDecorationData
    SETTINGS_HEADERS nwindecorationsettings.h
    SETTINGS_CLASSES NWinDecorationSettings
)

kconfig_add_kcfg_files(kcmnwindecoration_SRCS nwindecorationsettings.kcfgc GENERATE_MOC)

kcmutils_add_qml_kcm(kcm_nwindecoration SOURCES ${kcmnwindecoration_SRCS})

target_link_libraries(kcm_nwindecoration PRIVATE
    KDecoration3::KDecoration
    KF6::I18n
    KF6::KCMUtils
    KF6::KCMUtilsQuick
    Qt::Quick
    Qt::DBus
)

set(nwin-applywindowdecoration_SRCS
    nwin-applywindowdecoration.cpp
    decorationmodel.cpp
    utils.cpp
)

kconfig_add_kcfg_files(nwin-applywindowdecoration_SRCS nwindecorationsettings.kcfgc GENERATE_MOC)
add_executable(nwin-applywindowdecoration ${nwin-applywindowdecoration_SRCS})

target_link_libraries(nwin-applywindowdecoration
    KDecoration3::KDecoration
    Qt::DBus
    KF6::I18n
    KF6::KCMUtils
)

configure_file(window-decorations.knsrc.cmake ${CMAKE_CURRENT_BINARY_DIR}/window-decorations.knsrc)

install(FILES nwindecorationsettings.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/window-decorations.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})
install(TARGETS nwin-applywindowdecoration DESTINATION ${KDE_INSTALL_LIBEXECDIR})
