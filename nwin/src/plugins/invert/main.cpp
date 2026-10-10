/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "invert.h"

namespace NWin
{

NWIN_EFFECT_FACTORY_SUPPORTED(InvertEffect,
                              "metadata.json.stripped",
                              return InvertEffect::supported();)

} // namespace NWin

#include "main.moc"
