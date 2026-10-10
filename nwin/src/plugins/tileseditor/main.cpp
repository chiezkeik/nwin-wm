/*
    SPDX-FileCopyrightText: 2022 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "tileseditoreffect.h"

namespace NWin
{

NWIN_EFFECT_FACTORY_SUPPORTED(TilesEditorEffect,
                              "metadata.json.stripped",
                              return TilesEditorEffect::supported();)

} // namespace NWin

#include "main.moc"
