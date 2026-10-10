# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_nwinrules\")
add_definitions(-DKCMRULES)

set(nwinrules_SRCS
    ../../rulebooksettings.cpp
    ../../rules.cpp
    ../../utils/common.cpp
    ../../virtualdesktopsdbustypes.cpp
    optionsmodel.cpp
    ruleitem.cpp
    rulesmodel.cpp
    rulebookmodel.cpp
)

kconfig_add_kcfg_files(nwinrules_SRCS ../../rulesettings.kcfgc)
kconfig_add_kcfg_files(nwinrules_SRCS ../../rulebooksettingsbase.kcfgc)

add_library(NWinRulesObjects STATIC ${nwinrules_SRCS})
set_property(TARGET NWinRulesObjects PROPERTY POSITION_INDEPENDENT_CODE ON)

if (NWIN_BUILD_X11)
    set(nwin_kcm_rules_XCB_LIBS
        XCB::XCB
        XCB::XFIXES
    )
endif()

set(kcm_libs
    Qt::Quick

    KF6::KCMUtils
    KF6::I18n
    KF6::KCMUtilsQuick
    KF6::WindowSystem
    KF6::XmlGui
    KF6::ColorScheme
)

if (NWIN_BUILD_ACTIVITIES)
    set(kcm_libs ${kcm_libs} Plasma::Activities)
endif()
target_link_libraries(NWinRulesObjects ${kcm_libs} ${nwin_kcm_rules_XCB_LIBS})

kcmutils_add_qml_kcm(kcm_nwinrules SOURCES kcmrules.cpp)
target_link_libraries(kcm_nwinrules PRIVATE NWinRulesObjects)
