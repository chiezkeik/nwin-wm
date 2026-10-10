/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "integration.h"
#include <qpa/qplatformintegrationplugin.h>

#include <QCoreApplication>

class NWinIntegrationPlugin : public QPlatformIntegrationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformIntegrationFactoryInterface_iid FILE "nwin.json")

public:
    using QPlatformIntegrationPlugin::create;
    QPlatformIntegration *create(const QString &system, const QStringList &paramList) override;
};

QPlatformIntegration *NWinIntegrationPlugin::create(const QString &system, const QStringList &paramList)
{
    if (!QCoreApplication::applicationFilePath().endsWith(QLatin1StringView("nwin_wayland")) && !qEnvironmentVariableIsSet("NWIN_FORCE_OWN_QPA")) {
        // Not NWin
        return nullptr;
    }
    if (system.compare(QLatin1StringView("wayland-org.kde.nwin.qpa"), Qt::CaseInsensitive) == 0) {
        // create our integration
        return new NWin::QPA::Integration;
    }
    return nullptr;
}

#include "main.moc"
