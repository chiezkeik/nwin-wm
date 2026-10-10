/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2018 Martin Flöser <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include "input_event_spy.h"

namespace NWin
{

class HideCursorSpy : public InputEventSpy
{
public:
    void pointerMotion(NWin::PointerMotionEvent *event) override;
    void pointerButton(NWin::PointerButtonEvent *event) override;
    void pointerAxis(NWin::PointerAxisEvent *event) override;
    void touchDown(TouchDownEvent *event) override;
    void tabletToolProximityEvent(NWin::TabletToolProximityEvent *event) override;

private:
    void showCursor();
    void hideCursor();

    bool m_cursorHidden = false;
};

}
