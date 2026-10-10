/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2020 Cyril Rossi <cyril.rossi@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "nwintabboxdata.h"

#include "nwinswitcheffectsettings.h"
#include "nwintabboxsettings.h"
#include "shortcutsettings.h"

namespace NWin
{
namespace TabBox
{

NWinTabboxData::NWinTabboxData(QObject *parent)
    : KCModuleData(parent)
    , m_tabBoxConfig(new TabBoxSettings(QStringLiteral("TabBox"), this))
    , m_tabBoxAlternativeConfig(new TabBoxSettings(QStringLiteral("TabBoxAlternative"), this))
    , m_shortcutConfig(new ShortcutSettings(this))
{
    registerSkeleton(m_tabBoxConfig);
    registerSkeleton(m_tabBoxAlternativeConfig);
    registerSkeleton(m_shortcutConfig);
}

TabBoxSettings *NWinTabboxData::tabBoxConfig() const
{
    return m_tabBoxConfig;
}

TabBoxSettings *NWinTabboxData::tabBoxAlternativeConfig() const
{
    return m_tabBoxAlternativeConfig;
}

ShortcutSettings *NWinTabboxData::shortcutConfig() const
{
    return m_shortcutConfig;
}

}
}

#include "moc_nwintabboxdata.cpp"
