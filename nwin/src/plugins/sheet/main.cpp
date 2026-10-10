/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sheet.h"

namespace NWin
{

NWIN_EFFECT_FACTORY_SUPPORTED(SheetEffect,
                              "metadata.json.stripped",
                              return SheetEffect::supported();)

} // namespace NWin

#include "main.moc"
