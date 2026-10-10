/*
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "waylandshellintegration.h"

namespace NWin
{

WaylandShellIntegration::WaylandShellIntegration(QObject *parent)
    : QObject(parent)
{
}

} // namespace NWin

#include "moc_waylandshellintegration.cpp"
