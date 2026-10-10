/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "wobblywindows.h"

namespace NWin
{

NWIN_EFFECT_FACTORY_SUPPORTED(WobblyWindowsEffect,
                              "metadata.json.stripped",
                              return WobblyWindowsEffect::supported();)

} // namespace NWin

#include "main.moc"
