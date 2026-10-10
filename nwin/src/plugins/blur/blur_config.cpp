/*
    SPDX-FileCopyrightText: 2010 Fredrik Höglund <fredrik@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "blur_config.h"

#include "config-nwin.h"

// KConfigSkeleton
#include "blurconfig.h"

#include <KPluginFactory>
#include <nwineffects_interface.h>

K_PLUGIN_CLASS(NWin::BlurEffectConfig)

namespace NWin
{

BlurEffectConfig::BlurEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    ui.setupUi(widget());
    BlurConfig::instance(NWIN_CONFIG);
    addConfig(BlurConfig::self(), widget());
}

BlurEffectConfig::~BlurEffectConfig()
{
}

void BlurEffectConfig::save()
{
    KCModule::save();

    OrgKdeNWinEffectsInterface interface(QStringLiteral("org.kde.NWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("blur"));
}

} // namespace NWin

#include "blur_config.moc"

#include "moc_blur_config.cpp"
