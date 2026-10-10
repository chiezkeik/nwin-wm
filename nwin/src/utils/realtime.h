/*
    SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "nwin_export.h"

namespace NWin
{

/**
 * Makes the calling thread to use realtime scheduling.
 */
NWIN_EXPORT void gainRealTime();

} // namespace NWin
