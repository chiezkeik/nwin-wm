#SPDX-FileCopyrightText: 2020 Aleix Pol Gonzalzez <aleixpol@kde.org>
#SPDX-License-Identifier: BSD-3-Clause

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_nwinxwayland\")

kcmutils_generate_module_data(
    kcm_nwinxwayland_PART_SRCS
    MODULE_DATA_HEADER nwinxwaylanddata.h
    MODULE_DATA_CLASS_NAME NWinXwaylandData
    SETTINGS_HEADERS nwinxwaylandsettings.h
    SETTINGS_CLASSES NWinXwaylandSettings
)

kconfig_add_kcfg_files(kcm_nwinxwayland_PART_SRCS nwinxwaylandsettings.kcfgc GENERATE_MOC)
kcmutils_add_qml_kcm(kcm_nwinxwayland SOURCES kcmnwinxwayland.cpp ${kcm_nwinxwayland_PART_SRCS})

target_link_libraries(kcm_nwinxwayland PRIVATE
    KF6::I18n
    KF6::KCMUtils
    KF6::KCMUtilsQuick
    Qt::DBus
    Qt::Qml
    Wayland::Client
)
