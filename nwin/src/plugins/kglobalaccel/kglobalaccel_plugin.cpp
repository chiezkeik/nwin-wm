/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "kglobalaccel_plugin.h"

#include "input.h"

#include <QDebug>

KGlobalAccelImpl::KGlobalAccelImpl(QObject *parent)
    : KGlobalAccelInterface(parent)
{
}

KGlobalAccelImpl::~KGlobalAccelImpl() = default;

bool KGlobalAccelImpl::grabKey(int key, bool grab)
{
    return true;
}

bool KGlobalAccelImpl::checkKeyPressed(int keyQt, NWin::KeyboardKeyState state)
{
    switch (state) {
    case NWin::KeyboardKeyState::Pressed:
        return keyEvent(keyQt, ShortcutKeyState::Pressed);
    case NWin::KeyboardKeyState::Repeated:
        return keyEvent(keyQt, ShortcutKeyState::Repeated);
    case NWin::KeyboardKeyState::Released:
        return keyEvent(keyQt, ShortcutKeyState::Released);
    }

    return false;
}

bool KGlobalAccelImpl::checkPointerPressed(Qt::MouseButtons buttons)
{
    return pointerPressed(buttons);
}

bool KGlobalAccelImpl::checkAxisTriggered(int axis)
{
    return axisTriggered(axis);
}

void KGlobalAccelImpl::cancelModiferOnlySequence()
{
    resetModifierOnlyState();
}

#include "moc_kglobalaccel_plugin.cpp"
