/*
    SPDX-FileCopyrightText: 2019 Martin Flöser <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#pragma once

#include <private/kwindowsystemplugininterface_p.h>

class KWindowSystemNWinPlugin : public KWindowSystemPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KWindowSystemPluginInterface_iid FILE "nwindowsystem.json")
    Q_INTERFACES(KWindowSystemPluginInterface)

public:
    explicit KWindowSystemNWinPlugin(QObject *parent = nullptr);
    ~KWindowSystemNWinPlugin() override;

    KWindowEffectsPrivate *createEffects() override;
    KWindowSystemPrivate *createWindowSystem() override;
    KWindowShadowTilePrivate *createWindowShadowTile() override;
    KWindowShadowPrivate *createWindowShadow() override;
};
