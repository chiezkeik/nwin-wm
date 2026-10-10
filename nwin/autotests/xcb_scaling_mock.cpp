/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2022 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <utils/xcbutils.h>

namespace NWin
{

uint32_t Xcb::toXNative(qreal value)
{
    return value;
}

Rect Xcb::toXNative(const RectF &rect)
{
    return rect.toRect();
}

qreal Xcb::fromXNative(int value)
{
    return value;
}

QSizeF Xcb::fromXNative(const QSize &value)
{
    return value;
}

}

QDebug operator<<(QDebug dbg, const NWin::Rect &rect)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace();
    dbg << "NWin::Rect(" << rect.x() << "," << rect.y() << " " << rect.width() << "x" << rect.height() << ")";
    return dbg;
}

QDebug operator<<(QDebug dbg, const NWin::RectF &rect)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace();
    dbg << "NWin::RectF(" << rect.x() << "," << rect.y() << " " << rect.width() << "x" << rect.height() << ")";
    return dbg;
}
