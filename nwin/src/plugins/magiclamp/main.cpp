/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "magiclamp.h"

namespace NWin
{

NWIN_EFFECT_FACTORY_SUPPORTED(MagicLampEffect,
                              "metadata.json.stripped",
                              return MagicLampEffect::supported();)

} // namespace NWin

#include "main.moc"
