/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2017, 2018 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "slide_config.h"

#include "config-nwin.h"

// KConfigSkeleton
#include "slideconfig.h"

#include <nwineffects_interface.h>

#include <KPluginFactory>

K_PLUGIN_CLASS(NWin::SlideEffectConfig)

namespace NWin
{

SlideEffectConfig::SlideEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    m_ui.setupUi(widget());
    SlideConfig::instance(NWIN_CONFIG);
    addConfig(SlideConfig::self(), widget());
}

SlideEffectConfig::~SlideEffectConfig()
{
}

void SlideEffectConfig::save()
{
    KCModule::save();

    OrgKdeNWinEffectsInterface interface(QStringLiteral("org.kde.NWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("slide"));
}

} // namespace NWin

#include "slide_config.moc"

#include "moc_slide_config.cpp"
