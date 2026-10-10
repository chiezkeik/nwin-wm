/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2020 Cyril Rossi <cyril.rossi@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

#include <KCModuleData>

namespace NWin
{
namespace TabBox
{

class TabBoxSettings;
class SwitchEffectSettings;
class ShortcutSettings;

class NWinTabboxData : public KCModuleData
{
    Q_OBJECT

public:
    explicit NWinTabboxData(QObject *parent);

    TabBoxSettings *tabBoxConfig() const;
    TabBoxSettings *tabBoxAlternativeConfig() const;
    ShortcutSettings *shortcutConfig() const;

private:
    TabBoxSettings *m_tabBoxConfig;
    TabBoxSettings *m_tabBoxAlternativeConfig;
    ShortcutSettings *m_shortcutConfig;
};

}

}
