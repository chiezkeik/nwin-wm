/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2009 Lucas Murray <lmurray@undefinedfire.com>
    SPDX-FileCopyrightText: 2020 Cyril Rossi <cyril.rossi@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "nwinscreenedge.h"

namespace Ui
{

class NWinTouchScreenConfigUi;

}

namespace NWin
{

class NWinTouchScreenEdgeConfigForm : public NWinScreenEdge
{
    Q_OBJECT

public:
    NWinTouchScreenEdgeConfigForm(QWidget *parent = nullptr);
    ~NWinTouchScreenEdgeConfigForm() override;

protected:
    Monitor *monitor() const override;

private:
    std::unique_ptr<Ui::NWinTouchScreenConfigUi> ui;
};

} // namespace
