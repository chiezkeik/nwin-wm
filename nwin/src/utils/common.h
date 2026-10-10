/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 1999, 2000 Matthias Ettrich <ettrich@kde.org>
    SPDX-FileCopyrightText: 2003 Lubos Lunak <l.lunak@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

// cmake stuff
#include "config-nwin.h"
// nwin
#include "core/rect.h"
#include "effect/globals.h"
#include "utils/version.h"
// Qt
#include <QList>
#include <QLoggingCategory>
#include <QMatrix4x4>
#include <QPoint>
// system
#include <climits>
Q_DECLARE_LOGGING_CATEGORY(NWIN_CORE)
Q_DECLARE_LOGGING_CATEGORY(NWIN_OPENGL)
Q_DECLARE_LOGGING_CATEGORY(NWIN_QPAINTER)
Q_DECLARE_LOGGING_CATEGORY(NWIN_VIRTUALKEYBOARD)
namespace NWin
{

const QPoint invalidPoint(INT_MIN, INT_MIN);

enum StrutArea {
    StrutAreaInvalid = 0, // Null
    StrutAreaTop = 1 << 0,
    StrutAreaRight = 1 << 1,
    StrutAreaBottom = 1 << 2,
    StrutAreaLeft = 1 << 3,
    StrutAreaAll = StrutAreaTop | StrutAreaRight | StrutAreaBottom | StrutAreaLeft,
};
Q_DECLARE_FLAGS(StrutAreas, StrutArea)

class NWIN_EXPORT StrutRect : public RectF
{
public:
    explicit StrutRect(RectF rect = RectF(), StrutArea area = StrutAreaInvalid);
    StrutRect(const QPointF &topLeft, const QPointF &bottomRight, StrutArea area = StrutAreaInvalid);
    StrutRect(qreal x, qreal y, qreal width, qreal height, StrutArea area = StrutAreaInvalid);
    StrutRect(const StrutRect &other);
    StrutRect &operator=(const StrutRect &other);
    inline StrutArea area() const
    {
        return m_area;
    }

private:
    StrutArea m_area;
};
typedef QList<StrutRect> StrutRects;

} // namespace

// Must be outside namespace
Q_DECLARE_OPERATORS_FOR_FLAGS(NWin::StrutAreas)
Q_DECLARE_OPERATORS_FOR_FLAGS(NWin::QuickTileMode)
