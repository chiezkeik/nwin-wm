/*
    SPDX-FileCopyrightText: 2010 Fredrik Höglund <fredrik@kde.org>
    SPDX-FileCopyrightText: 2010 Alexandre Pereira <pereira.alex@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "glide_config.h"

#include "config-nwin.h"

// KConfigSkeleton
#include "glideconfig.h"

#include <KPluginFactory>
#include <nwineffects_interface.h>

K_PLUGIN_CLASS(NWin::GlideEffectConfig)

namespace NWin
{

GlideEffectConfig::GlideEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    ui.setupUi(widget());
    GlideConfig::instance(NWIN_CONFIG);
    addConfig(GlideConfig::self(), widget());
}

GlideEffectConfig::~GlideEffectConfig()
{
}

void GlideEffectConfig::save()
{
    KCModule::save();
    OrgKdeNWinEffectsInterface interface(QStringLiteral("org.kde.NWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("glide"));
}

} // namespace NWin

#include "glide_config.moc"

#include "moc_glide_config.cpp"
