/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "colorpicker.h"

namespace NWin
{

NWIN_EFFECT_FACTORY_SUPPORTED(ColorPickerEffect,
                              "metadata.json.stripped",
                              return ColorPickerEffect::supported();)

} // namespace NWin

#include "main.moc"
