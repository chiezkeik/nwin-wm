# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_nwin_scripts\")

kcmutils_add_qml_kcm(kcm_nwin_scripts INSTALL_NAMESPACE plasma/kcms/systemsettings)

target_sources(kcm_nwin_scripts PRIVATE
    module.cpp
    nwinscriptsdata.cpp
)

target_link_libraries(kcm_nwin_scripts PRIVATE
    Qt::DBus

    KF6::I18n
    KF6::KCMUtilsCore
    KF6::KCMUtils
    KF6::Package
    KF6::KCMUtilsQuick
)

install(FILES nwinscripts.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})
