# KI18N Translation Domain for this library.
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_nwin_effects\")

########### next target ###############

set(kcm_nwin_effects_PART_SRCS
    kcm.cpp
    effectsfilterproxymodel.cpp
    desktopeffectsdata.cpp
)

kcmutils_add_qml_kcm(kcm_nwin_effects SOURCES ${kcm_nwin_effects_PART_SRCS})

target_link_libraries(kcm_nwin_effects PRIVATE
    Qt::DBus
    Qt::Quick

    KF6::KCMUtils
    KF6::I18n
    KF6::KCMUtils
    KF6::KCMUtilsQuick
    KF6::XmlGui

    kcmnwincommon
)

install(FILES nwineffect.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})
