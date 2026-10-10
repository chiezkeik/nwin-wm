/*
    SPDX-FileCopyrightText: 1998 Matthias Ettrich <ettrich@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mouse.h"
#include "nwinoptions_settings.h"

NWinMouseConfigForm::NWinMouseConfigForm(QWidget *parent)
    : QWidget(parent)
{
    setupUi(parent);
}

NWinActionsConfigForm::NWinActionsConfigForm(QWidget *parent)
    : QWidget(parent)
{
    setupUi(parent);
}

KTitleBarActionsConfig::KTitleBarActionsConfig(NWinOptionsSettings *settings, QWidget *parent)
    : KCModule(parent, KPluginMetaData())
    , m_ui(new NWinMouseConfigForm(widget()))
{
    if (settings) {
        initialize(settings);
    }
}

void KTitleBarActionsConfig::initialize(NWinOptionsSettings *settings)
{
    m_settings = settings;
    addConfig(m_settings, widget());
}

KWindowActionsConfig::KWindowActionsConfig(NWinOptionsSettings *settings, QWidget *parent)
    : KCModule(parent, KPluginMetaData())
    , m_ui(new NWinActionsConfigForm(widget()))
{
    if (settings) {
        initialize(settings);
    }
}

void KWindowActionsConfig::initialize(NWinOptionsSettings *settings)
{
    m_settings = settings;
    addConfig(m_settings, widget());
}

#include "moc_mouse.cpp"
