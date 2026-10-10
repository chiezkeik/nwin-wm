/*
    SPDX-FileCopyrightText: 2019 Martin Flöser <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "plugin.h"
#include "windoweffects.h"
#include "windowshadow.h"
#include "windowsystem.h"

KWindowSystemNWinPlugin::KWindowSystemNWinPlugin(QObject *parent)
    : KWindowSystemPluginInterface(parent)
{
}

KWindowSystemNWinPlugin::~KWindowSystemNWinPlugin()
{
}

KWindowEffectsPrivate *KWindowSystemNWinPlugin::createEffects()
{
    return new NWin::WindowEffects();
}

KWindowSystemPrivate *KWindowSystemNWinPlugin::createWindowSystem()
{
    return new NWin::WindowSystem();
}

KWindowShadowTilePrivate *KWindowSystemNWinPlugin::createWindowShadowTile()
{
    return new NWin::WindowShadowTile();
}

KWindowShadowPrivate *KWindowSystemNWinPlugin::createWindowShadow()
{
    return new NWin::WindowShadow();
}

#include "moc_plugin.cpp"
