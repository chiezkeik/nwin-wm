/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "magnifier.h"

namespace NWin
{

NWIN_EFFECT_FACTORY_SUPPORTED(MagnifierEffect,
                              "metadata.json.stripped",
                              return MagnifierEffect::supported();)

} // namespace NWin

#include "main.moc"
