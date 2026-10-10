/*
    SPDX-FileCopyrightText: 2022 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "blendchanges.h"

namespace NWin
{

NWIN_EFFECT_FACTORY_SUPPORTED(BlendChanges,
                              "metadata.json.stripped",
                              return BlendChanges::supported();)

} // namespace NWin

#include "main.moc"
