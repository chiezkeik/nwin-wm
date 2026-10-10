/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "screentransform.h"

namespace NWin
{

NWIN_EFFECT_FACTORY_SUPPORTED(ScreenTransformEffect,
                              "metadata.json.stripped",
                              return ScreenTransformEffect::supported();)

} // namespace NWin

#include "main.moc"
