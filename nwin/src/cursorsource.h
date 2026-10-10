/*
    SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "utils/cursortheme.h"

#include <QImage>
#include <QObject>
#include <QPoint>
#include <QTimer>

namespace NWin
{

class SurfaceInterface;

/**
 * The CursorSource class represents the contents of the Cursor.
 */
class NWIN_EXPORT CursorSource : public QObject
{
    Q_OBJECT

public:
    explicit CursorSource(QObject *parent = nullptr);

    bool isBlank() const;
    QSizeF size() const;
    QPointF hotspot() const;

Q_SIGNALS:
    void changed();

protected:
    QSizeF m_size = QSizeF(0, 0);
    QPointF m_hotspot;
};

/**
 * The ShapeCursorSource class represents the contents of a shape in the cursor theme.
 */
class NWIN_EXPORT ShapeCursorSource : public CursorSource
{
    Q_OBJECT

public:
    explicit ShapeCursorSource(QObject *parent = nullptr);

    QImage image() const;

    QByteArray shape() const;
    void setShape(const QByteArray &shape);
    void setShape(Qt::CursorShape shape);

    CursorTheme theme() const;
    void setTheme(const CursorTheme &theme);

private:
    void refresh();
    void selectNextSprite();
    void selectSprite(int index);

    CursorTheme m_theme;
    QByteArray m_shape;
    QList<CursorSprite> m_sprites;
    QTimer m_delayTimer;
    QImage m_image;
    int m_currentSprite = -1;
};

/**
 * The SurfaceCursorSource class repsents the contents of a cursor backed by a wl_surface.
 */
class NWIN_EXPORT SurfaceCursorSource : public CursorSource
{
    Q_OBJECT

public:
    explicit SurfaceCursorSource(QObject *parent = nullptr);

    SurfaceInterface *surface() const;

public Q_SLOTS:
    void update(SurfaceInterface *surface, const QPointF &hotspot);

private:
    void refresh();
    void reset();

    SurfaceInterface *m_surface = nullptr;
};

} // namespace NWin
