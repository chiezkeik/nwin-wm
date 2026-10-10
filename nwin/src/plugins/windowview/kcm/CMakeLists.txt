# SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

set(nwin_windowview_config_SOURCES windowvieweffectkcm.cpp)
ki18n_wrap_ui(nwin_windowview_config_SOURCES windowvieweffectkcm.ui)
kconfig_add_kcfg_files(nwin_windowview_config_SOURCES ../windowviewconfig.kcfgc)

nwin_add_effect_config(nwin_windowview_config ${nwin_windowview_config_SOURCES})
target_link_libraries(nwin_windowview_config
    KF6::KCMUtils
    KF6::CoreAddons
    KF6::GlobalAccel
    KF6::I18n
    KF6::XmlGui
    NWinEffectsInterface
)
