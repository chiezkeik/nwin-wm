/*
    mouse.h

    SPDX-FileCopyrightText: 1998 Matthias Ettrich <ettrich@kde.org>


    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

class KConfig;

#include <KCModule>
#include <KLocalizedString>

#include "ui_actions.h"
#include "ui_mouse.h"

class NWinOptionsSettings;

class NWinMouseConfigForm : public QWidget, public Ui::NWinMouseConfigForm
{
    Q_OBJECT

public:
    explicit NWinMouseConfigForm(QWidget *parent);
};

class NWinActionsConfigForm : public QWidget, public Ui::NWinActionsConfigForm
{
    Q_OBJECT

public:
    explicit NWinActionsConfigForm(QWidget *parent);
};

class KTitleBarActionsConfig : public KCModule
{
    Q_OBJECT

public:
    KTitleBarActionsConfig(NWinOptionsSettings *settings, QWidget *parent);

protected:
    void initialize(NWinOptionsSettings *settings);

private:
    NWinMouseConfigForm *m_ui;
    NWinOptionsSettings *m_settings;
};

class KWindowActionsConfig : public KCModule
{
    Q_OBJECT

public:
    KWindowActionsConfig(NWinOptionsSettings *settings, QWidget *parent);

    bool isDefaults() const;
    bool isSaveNeeded() const;

protected:
    void initialize(NWinOptionsSettings *settings);

private:
    NWinActionsConfigForm *m_ui;
    NWinOptionsSettings *m_settings;
};
