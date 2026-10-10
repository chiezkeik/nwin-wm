/*
    SPDX-FileCopyrightText: 2020 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kcmnwinxwayland.h"

#include <qqml.h>

#include <KConfigGroup>
#include <KDesktopFile>
#include <KLocalizedString>
#include <KPluginFactory>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QKeySequence>

#include <nwinxwaylanddata.h>

K_PLUGIN_FACTORY_WITH_JSON(KcmXwaylandFactory, "kcm_nwinxwayland.json", registerPlugin<KcmXwayland>(); registerPlugin<NWinXwaylandData>();)

KcmXwayland::KcmXwayland(QObject *parent, const KPluginMetaData &metaData)
    : KQuickManagedConfigModule(parent, metaData)
    , m_data(new NWinXwaylandData(this))
    , m_settings(new NWinXwaylandSettings(m_data))
{
    registerSettings(m_settings);
    qmlRegisterAnonymousType<NWinXwaylandSettings>("org.kde.nwin.nwinxwaylandsettings", 1);
}

void KcmXwayland::logout() const
{
    auto method = QDBusMessage::createMethodCall(QStringLiteral("org.kde.LogoutPrompt"),
                                                 QStringLiteral("/LogoutPrompt"),
                                                 QStringLiteral("org.kde.LogoutPrompt"),
                                                 QStringLiteral("promptLogout"));
    QDBusConnection::sessionBus().asyncCall(method);
}

KcmXwayland::~KcmXwayland() = default;

#include "kcmnwinxwayland.moc"

#include "moc_kcmnwinxwayland.cpp"
