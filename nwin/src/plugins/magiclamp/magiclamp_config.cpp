/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2009 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "magiclamp_config.h"

#include "config-nwin.h"

// KConfigSkeleton
#include "magiclampconfig.h"

#include <nwineffects_interface.h>

#include <KPluginFactory>
#include <kconfiggroup.h>

#include <QVBoxLayout>

K_PLUGIN_CLASS(NWin::MagicLampEffectConfig)

namespace NWin
{

MagicLampEffectConfig::MagicLampEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    m_ui.setupUi(widget());

    MagicLampConfig::instance(NWIN_CONFIG);
    addConfig(MagicLampConfig::self(), widget());
}

void MagicLampEffectConfig::save()
{
    KCModule::save();
    OrgKdeNWinEffectsInterface interface(QStringLiteral("org.kde.NWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("magiclamp"));
}

} // namespace

#include "magiclamp_config.moc"

#include "moc_magiclamp_config.cpp"
