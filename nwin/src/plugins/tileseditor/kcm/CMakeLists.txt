# SPDX-FileCopyrightText: 2022 Marco Martin <mart@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

set(nwin_tileseditor_config_SOURCES tileseditoreffectkcm.cpp)
ki18n_wrap_ui(nwin_tileseditor_config_SOURCES tileseditoreffectkcm.ui)

nwin_add_effect_config(nwin_tileseditor_config ${nwin_tileseditor_config_SOURCES})
target_link_libraries(nwin_tileseditor_config
    KF6::KCMUtils
    KF6::CoreAddons
    KF6::GlobalAccel
    KF6::I18n
    KF6::XmlGui
    NWinEffectsInterface
)
