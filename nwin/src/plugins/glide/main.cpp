/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "glide.h"

namespace NWin
{

NWIN_EFFECT_FACTORY_SUPPORTED(GlideEffect,
                              "metadata.json.stripped",
                              return GlideEffect::supported();)

} // namespace NWin

#include "main.moc"
