/*
    SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

*/

#pragma once

#include "config-nwin.h"

#include "effect/globals.h"
#include <KConfigWatcher>
#include <QObject>
#include <nwin_export.h>

namespace NWin
{

class NWIN_EXPORT TabletModeManager : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.NWin.TabletModeManager")
    // assuming such a switch is not pluggable for now
    Q_PROPERTY(bool tabletModeAvailable READ isTabletModeAvailable NOTIFY tabletModeAvailableChanged)
    Q_PROPERTY(bool tabletMode READ effectiveTabletMode NOTIFY tabletModeChanged)

public:
    enum class ConfiguredMode {
        Auto,
        Off,
        On,
    };

    explicit TabletModeManager();
    ~TabletModeManager() override = default;

    void setTabletModeAvailable(bool detecting);
    bool isTabletModeAvailable() const;

    bool effectiveTabletMode() const;
    bool isTablet() const;
    void setIsTablet(bool tablet);

    ConfiguredMode configuredMode() const;

Q_SIGNALS:
    void tabletModeAvailableChanged(bool available);
    void tabletModeChanged(bool tabletMode);

private:
    void hasTabletModeInputChanged(bool set);
    void refreshSettings();

    KConfigWatcher::Ptr m_settingsWatcher;
    bool m_isTabletMode = false;
    bool m_detecting = false;
    ConfiguredMode m_configuredMode = ConfiguredMode::Auto;
};

}
