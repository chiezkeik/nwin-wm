/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2006 Lubos Lunak <l.lunak@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "config-nwin.h"

#if !NWIN_BUILD_X11
#error Do not include on non-X11 builds
#endif

#include <QGuiApplication>
#include <nwin_export.h>
#include <xcb/xcb.h>

namespace NWin
{

inline NWIN_EXPORT xcb_connection_t *connection()
{
    return reinterpret_cast<xcb_connection_t *>(qApp->property("x11Connection").value<void *>());
}

inline NWIN_EXPORT xcb_window_t rootWindow()
{
    return qApp->property("x11RootWindow").value<quint32>();
}

void NWIN_EXPORT grabXServer();
void NWIN_EXPORT ungrabXServer();

/**
 * Small helper class which performs grabXServer in the ctor and
 * ungrabXServer in the dtor. Use this class to ensure that grab and
 * ungrab are matched.
 */
class XServerGrabber
{
public:
    XServerGrabber()
    {
        grabXServer();
    }
    ~XServerGrabber()
    {
        ungrabXServer();
    }
};

}
