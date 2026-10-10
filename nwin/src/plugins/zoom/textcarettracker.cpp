/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "textcarettracker.h"
#include "inputmethod.h"
#include "main.h"
#include "window.h"

#if NWIN_BUILD_X11
#include "x11window.h"
#endif

namespace NWin
{

TextCaretTracker::TextCaretTracker()
{
    connect(nwinApp()->inputMethod(), &InputMethod::activeWindowChanged, this, &TextCaretTracker::tryUpdate);
    connect(nwinApp()->inputMethod(), &InputMethod::cursorRectangleChanged, this, &TextCaretTracker::tryUpdate);
    tryUpdate();
}

static bool supportsOnlyLegacyTextCaretMoved(const Window *window)
{
#if NWIN_BUILD_X11
    return qobject_cast<const X11Window *>(window);
#else
    return false;
#endif
}

void TextCaretTracker::tryUpdate()
{
    const Window *followedWindow = nwinApp()->inputMethod()->activeWindow();
    if (!followedWindow) {
        m_cursorRectangle.reset();
        return;
    }

    if (supportsOnlyLegacyTextCaretMoved(followedWindow)) {
        subscribeLegacyTextCaretMoved();
    } else {
        unsubscribeLegacyTextCaretMoved();

        const RectF cursorRectangle = nwinApp()->inputMethod()->cursorRectangle();
        if (cursorRectangle.isEmpty()) {
            m_cursorRectangle.reset();
            return;
        }

        if (m_cursorRectangle != cursorRectangle) {
            m_cursorRectangle = cursorRectangle;
            if (!followedWindow->isInteractiveMove() && !followedWindow->isInteractiveResize()) {
                Q_EMIT moved(m_cursorRectangle->center());
            }
        }
    }
}

void TextCaretTracker::subscribeLegacyTextCaretMoved()
{
#if NWIN_BUILD_QACCESSIBILITYCLIENT
    // Dbus-based text caret tracking is disabled on wayland because libqaccessibilityclient has
    // blocking dbus calls, which can result in nwin_wayland lockups.

    static bool forceCaretTracking = qEnvironmentVariableIntValue("NWIN_WAYLAND_ZOOM_FORCE_LEGACY_TEXT_CARET_TRACKING");
    if (!forceCaretTracking) {
        return;
    }

    if (!m_registry) {
        m_registry = new QAccessibleClient::Registry(this);
        connect(m_registry, &QAccessibleClient::Registry::textCaretMoved, this, [this](const QAccessibleClient::AccessibleObject &object) {
            Q_EMIT moved(object.focusPoint());
        });
    }

    m_registry->subscribeEventListeners(QAccessibleClient::Registry::TextCaretMoved);
#endif
}

void TextCaretTracker::unsubscribeLegacyTextCaretMoved()
{
#if NWIN_BUILD_QACCESSIBILITYCLIENT
    if (m_registry) {
        m_registry->subscribeEventListeners(QAccessibleClient::Registry::NoEventListeners);
    }
#endif
}

} // namespace NWin

#include "moc_textcarettracker.cpp"
