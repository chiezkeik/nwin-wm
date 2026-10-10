/*
    SPDX-FileCopyrightText: 2024 Nicolas Fella <nicolas.fella@kdab.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "systembell.h"

namespace NWin
{

NWIN_EFFECT_FACTORY_SUPPORTED(SystemBellEffect,
                              "metadata.json.stripped",
                              return SystemBellEffect::supported();)

} // namespace NWin

#include "main.moc"
