/*
    SPDX-FileCopyrightText: 2020 Cyril Rossi <cyril.rossi@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "nwinscriptsdata.h"

#include "config-nwin.h"

#include <KConfigGroup>
#include <KPackage/Package>
#include <KPackage/PackageLoader>
#include <KPackage/PackageStructure>
#include <KPluginFactory>

NWinScriptsData::NWinScriptsData(QObject *parent)
    : KCModuleData(parent)
    , m_nwinConfig(KSharedConfig::openConfig("nwinrc"))
{
}

QList<KPluginMetaData> NWinScriptsData::pluginMetaDataList() const
{
    return KPackage::PackageLoader::self()->findPackages(QStringLiteral("NWin/Script"), QStringLiteral("nwin-wayland/scripts/"))
        + KPackage::PackageLoader::self()->findPackages(QStringLiteral("NWin/Script"), QStringLiteral("nwin/scripts/"));
}

bool NWinScriptsData::isDefaults() const
{
    QList<KPluginMetaData> plugins = pluginMetaDataList();
    KConfigGroup cfgGroup(m_nwinConfig, QStringLiteral("Plugins"));
    for (auto &plugin : plugins) {
        if (cfgGroup.readEntry(plugin.pluginId() + QLatin1StringView("Enabled"), plugin.isEnabledByDefault()) != plugin.isEnabledByDefault()) {
            return false;
        }
    }

    return true;
}

#include "moc_nwinscriptsdata.cpp"
