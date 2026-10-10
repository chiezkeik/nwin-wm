/*
    SPDX-FileCopyrightText: 2024 Nicolas Fella <nicolas.fella@kdab.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "plugin.h"

#include "keynotification.h"

class NWIN_EXPORT KeyNotificationFactory : public NWin::PluginFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginFactory_iid FILE "metadata.json")
    Q_INTERFACES(NWin::PluginFactory)

public:
    std::unique_ptr<NWin::Plugin> create() const override
    {
        return std::make_unique<NWin::KeyNotificationPlugin>();
    }
};

#include "main.moc"
