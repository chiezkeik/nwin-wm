# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmnwincommon\")

set(kcmnwincommon_SRC
    effectsmodel.cpp
)

qt_add_dbus_interface(kcmnwincommon_SRC
    ${NWin_SOURCE_DIR}/src/org.kde.NWin.Effects.xml nwin_effects_interface
)

add_library(kcmnwincommon SHARED ${kcmnwincommon_SRC})

target_link_libraries(kcmnwincommon
    Qt::Core
    Qt::DBus
    Qt::Quick
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::I18n
    KF6::Package
    KF6::KCMUtils
    KF6::XmlGui # KAboutApplicationDialog
)

set_target_properties(kcmnwincommon PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 6
)

install(TARGETS kcmnwincommon ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

set(kcm_nwin4_genericscripted_SRCS genericscriptedconfig.cpp)
qt_add_dbus_interface(kcm_nwin4_genericscripted_SRCS ${nwin_effects_dbus_xml} nwineffects_interface)
add_library(kcm_nwin4_genericscripted MODULE ${kcm_nwin4_genericscripted_SRCS})
target_link_libraries(kcm_nwin4_genericscripted
    KF6::KCMUtils #KCModule
    KF6::I18n
    Qt::DBus
    Qt::UiTools
)
install(TARGETS kcm_nwin4_genericscripted DESTINATION ${KDE_INSTALL_PLUGINDIR}/nwin/effects/configs)
