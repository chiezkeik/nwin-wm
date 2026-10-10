/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "fallapart.h"

namespace NWin
{

NWIN_EFFECT_FACTORY_SUPPORTED(FallApartEffect,
                              "metadata.json.stripped",
                              return FallApartEffect::supported();)

} // namespace NWin

#include "main.moc"
