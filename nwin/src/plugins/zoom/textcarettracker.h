/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "config-nwin.h"

#include "core/rect.h"

#include <QObject>
#include <QPointF>
#include <QPointer>

#if NWIN_BUILD_QACCESSIBILITYCLIENT
#include <qaccessibilityclient/registry.h>
#endif

namespace NWin
{

class Window;

/**
 * The TextCaretTracker helper provides a way to track the position of the text caret
 * in the active window.
 */
class TextCaretTracker : public QObject
{
    Q_OBJECT

public:
    TextCaretTracker();

Q_SIGNALS:
    void moved(const QPointF &focus);

private:
    void subscribeLegacyTextCaretMoved();
    void unsubscribeLegacyTextCaretMoved();

    void tryUpdate();

#if NWIN_BUILD_QACCESSIBILITYCLIENT
    QAccessibleClient::Registry *m_registry = nullptr;
#endif
    std::optional<RectF> m_cursorRectangle;
};

} // namespace NWin
