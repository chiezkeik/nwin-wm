/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2009 Lucas Murray <lmurray@undefinedfire.com>
    SPDX-FileCopyrightText: 2020 Cyril Rossi <cyril.rossi@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "nwintouchscreenedgeconfigform.h"
#include "ui_touch.h"

namespace NWin
{

NWinTouchScreenEdgeConfigForm::NWinTouchScreenEdgeConfigForm(QWidget *parent)
    : NWinScreenEdge(parent)
    , ui(std::make_unique<Ui::NWinTouchScreenConfigUi>())
{
    ui->setupUi(this);
}

NWinTouchScreenEdgeConfigForm::~NWinTouchScreenEdgeConfigForm() = default;

Monitor *NWinTouchScreenEdgeConfigForm::monitor() const
{
    return ui->monitor;
}

} // namespace

#include "moc_nwintouchscreenedgeconfigform.cpp"
