/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2007 Christian Nitschkowski <christian.nitschkowski@kdemail.net>
    SPDX-FileCopyrightText: 2018 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "diminactive_config.h"

#include "config-nwin.h"

// KConfigSkeleton
#include "diminactiveconfig.h"

#include <nwineffects_interface.h>

#include <KPluginFactory>

K_PLUGIN_CLASS(NWin::DimInactiveEffectConfig)

namespace NWin
{

DimInactiveEffectConfig::DimInactiveEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    m_ui.setupUi(widget());
    DimInactiveConfig::instance(NWIN_CONFIG);
    addConfig(DimInactiveConfig::self(), widget());
}

DimInactiveEffectConfig::~DimInactiveEffectConfig()
{
}

void DimInactiveEffectConfig::save()
{
    KCModule::save();

    OrgKdeNWinEffectsInterface interface(QStringLiteral("org.kde.NWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("diminactive"));
}

} // namespace NWin

#include "diminactive_config.moc"

#include "moc_diminactive_config.cpp"
