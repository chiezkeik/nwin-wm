/*
    main.h

    SPDX-FileCopyrightText: 2001 Waldo Bastian <bastian@kde.org>

    Requires the Qt widget libraries, available at no cost at
    https://www.qt.io

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KCModule>
#include <QTabWidget>

class NWinOptionsSettings;
class NWinOptionsKDEGlobalsSettings;
class KFocusConfig;
class KTitleBarActionsConfig;
class KWindowActionsConfig;
class KAdvancedConfig;
class KMovingConfig;
class KPipConfig;

class NWinOptions : public KCModule
{
    Q_OBJECT

public:
    explicit NWinOptions(QObject *parent, const KPluginMetaData &data);

    void load() override;
    void save() override;
    void defaults() override;

protected Q_SLOTS:
    void updateUnmanagedState();

private:
    QTabWidget *tab;

    KFocusConfig *mFocus;
    KTitleBarActionsConfig *mTitleBarActions;
    KWindowActionsConfig *mWindowActions;
    KMovingConfig *mMoving;
    KAdvancedConfig *mAdvanced;
    KPipConfig *mPip = nullptr;

    NWinOptionsSettings *mSettings;
};

class KActionsOptions : public KCModule
{
    Q_OBJECT

public:
    KActionsOptions(QObject *parent, const KPluginMetaData &data);

    void load() override;
    void save() override;
    void defaults() override;

protected Q_SLOTS:

    void moduleChanged(bool state);

private:
    QTabWidget *tab;

    KTitleBarActionsConfig *mTitleBarActions;
    KWindowActionsConfig *mWindowActions;

    NWinOptionsSettings *mSettings;
};
