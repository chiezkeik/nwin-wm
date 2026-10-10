/*
    SPDX-FileCopyrightText: 2024 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "scene/item.h"

namespace NWin
{

/**
 * The RootItem type represents the root item in the scene.
 */
class NWIN_EXPORT RootItem : public Item
{
    Q_OBJECT

public:
    explicit RootItem(Scene *scene);
};

} // namespace NWin
