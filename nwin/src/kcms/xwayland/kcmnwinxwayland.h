/*
    SPDX-FileCopyrightText: 2020 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KQuickManagedConfigModule>
#include <QAbstractListModel>

#include <nwinxwaylandsettings.h>

class NWinXwaylandData;

class KcmXwayland : public KQuickManagedConfigModule
{
    Q_OBJECT
    Q_PROPERTY(NWinXwaylandSettings *settings READ settings CONSTANT)

public:
    explicit KcmXwayland(QObject *parent, const KPluginMetaData &metaData);
    ~KcmXwayland() override;

    NWinXwaylandSettings *settings() const
    {
        return m_settings;
    }

    Q_INVOKABLE void logout() const;

Q_SIGNALS:
    void showLogoutMessage();

private:
    void refresh();

    NWinXwaylandData *const m_data;
    NWinXwaylandSettings *const m_settings;
};
