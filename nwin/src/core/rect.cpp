/*
    SPDX-FileCopyrightText: 2024 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "core/rect.h"

#include <QDebugStateSaver>

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

QDataStream &operator<<(QDataStream &stream, const NWin::Rect &rect)
{
    stream << int(rect.left()) << int(rect.top()) << int(rect.right()) << int(rect.bottom());
    return stream;
}

QDataStream &operator>>(QDataStream &stream, NWin::Rect &rect)
{
    int left;
    int top;
    int right;
    int bottom;
    stream >> left >> top >> right >> bottom;
    rect.setCoords(left, top, right, bottom);
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const NWin::RectF &rect)
{
    stream << double(rect.left()) << double(rect.top()) << double(rect.right()) << double(rect.bottom());
    return stream;
}

QDataStream &operator>>(QDataStream &stream, NWin::RectF &rect)
{
    double left;
    double top;
    double right;
    double bottom;
    stream >> left >> top >> right >> bottom;
    rect.setCoords(left, top, right, bottom);
    return stream;
}
