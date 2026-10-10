/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "overvieweffect.h"

namespace NWin
{

NWIN_EFFECT_FACTORY_SUPPORTED(OverviewEffect,
                              "metadata.json.stripped",
                              return OverviewEffect::supported();)

} // namespace NWin

#include "main.moc"
