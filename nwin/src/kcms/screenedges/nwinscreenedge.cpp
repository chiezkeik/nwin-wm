/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2009 Lucas Murray <lmurray@undefinedfire.com>
    SPDX-FileCopyrightText: 2020 Cyril Rossi <cyril.rossi@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "nwinscreenedge.h"

#include "monitor.h"

namespace NWin
{

NWinScreenEdge::NWinScreenEdge(QWidget *parent)
    : QWidget(parent)
{
    QMetaObject::invokeMethod(this, &NWinScreenEdge::createConnection, Qt::QueuedConnection);
}

NWinScreenEdge::~NWinScreenEdge()
{
}

void NWinScreenEdge::monitorHideEdge(ElectricBorder border, bool hidden)
{
    const int edge = NWinScreenEdge::electricBorderToMonitorEdge(border);
    monitor()->setEdgeHidden(edge, hidden);
}

void NWinScreenEdge::monitorEnableEdge(ElectricBorder border, bool enabled)
{
    const int edge = NWinScreenEdge::electricBorderToMonitorEdge(border);
    monitor()->setEdgeEnabled(edge, enabled);
}

void NWinScreenEdge::monitorAddItem(const QString &item)
{
    for (int i = 0; i < 8; i++) {
        monitor()->addEdgeItem(i, item);
    }
}

void NWinScreenEdge::monitorItemSetEnabled(int index, bool enabled)
{
    for (int i = 0; i < 8; i++) {
        monitor()->setEdgeItemEnabled(i, index, enabled);
    }
}

void NWinScreenEdge::monitorChangeEdge(const QList<int> &borderList, int index)
{
    for (int border : borderList) {
        monitorChangeEdge(static_cast<ElectricBorder>(border), index);
    }
}

void NWinScreenEdge::monitorChangeEdge(ElectricBorder border, int index)
{
    if (ELECTRIC_COUNT == border || ElectricNone == border) {
        return;
    }
    m_reference[border] = index;
    monitor()->selectEdgeItem(NWinScreenEdge::electricBorderToMonitorEdge(border), index);
}

QList<int> NWinScreenEdge::monitorCheckEffectHasEdge(int index) const
{
    QList<int> list;
    if (monitor()->selectedEdgeItem(Monitor::Top) == index) {
        list.append(ElectricTop);
    }
    if (monitor()->selectedEdgeItem(Monitor::TopRight) == index) {
        list.append(ElectricTopRight);
    }
    if (monitor()->selectedEdgeItem(Monitor::Right) == index) {
        list.append(ElectricRight);
    }
    if (monitor()->selectedEdgeItem(Monitor::BottomRight) == index) {
        list.append(ElectricBottomRight);
    }
    if (monitor()->selectedEdgeItem(Monitor::Bottom) == index) {
        list.append(ElectricBottom);
    }
    if (monitor()->selectedEdgeItem(Monitor::BottomLeft) == index) {
        list.append(ElectricBottomLeft);
    }
    if (monitor()->selectedEdgeItem(Monitor::Left) == index) {
        list.append(ElectricLeft);
    }
    if (monitor()->selectedEdgeItem(Monitor::TopLeft) == index) {
        list.append(ElectricTopLeft);
    }

    if (list.isEmpty()) {
        list.append(ElectricNone);
    }
    return list;
}

int NWinScreenEdge::selectedEdgeItem(ElectricBorder border) const
{
    return monitor()->selectedEdgeItem(NWinScreenEdge::electricBorderToMonitorEdge(border));
}

void NWinScreenEdge::monitorChangeDefaultEdge(ElectricBorder border, int index)
{
    if (ELECTRIC_COUNT == border || ElectricNone == border) {
        return;
    }
    m_default[border] = index;
}

void NWinScreenEdge::monitorChangeDefaultEdge(const QList<int> &borderList, int index)
{
    for (int border : borderList) {
        monitorChangeDefaultEdge(static_cast<ElectricBorder>(border), index);
    }
}

void NWinScreenEdge::reload()
{
    for (auto it = m_reference.cbegin(); it != m_reference.cend(); ++it) {
        monitor()->selectEdgeItem(NWinScreenEdge::electricBorderToMonitorEdge(it.key()), it.value());
    }
    onChanged();
}

void NWinScreenEdge::setDefaults()
{
    for (auto it = m_default.cbegin(); it != m_default.cend(); ++it) {
        monitor()->selectEdgeItem(NWinScreenEdge::electricBorderToMonitorEdge(it.key()), it.value());
    }
    onChanged();
}

int NWinScreenEdge::electricBorderToMonitorEdge(ElectricBorder border)
{
    switch (border) {
    case ElectricTop:
        return Monitor::Top;
    case ElectricTopRight:
        return Monitor::TopRight;
    case ElectricRight:
        return Monitor::Right;
    case ElectricBottomRight:
        return Monitor::BottomRight;
    case ElectricBottom:
        return Monitor::Bottom;
    case ElectricBottomLeft:
        return Monitor::BottomLeft;
    case ElectricLeft:
        return Monitor::Left;
    case ElectricTopLeft:
        return Monitor::TopLeft;
    default: // ELECTRIC_COUNT and ElectricNone
        return Monitor::None;
    }
}

ElectricBorder NWinScreenEdge::monitorEdgeToElectricBorder(int edge)
{
    const Monitor::Edges monitorEdge = static_cast<Monitor::Edges>(edge);
    switch (monitorEdge) {
    case Monitor::Left:
        return ElectricLeft;
    case Monitor::Right:
        return ElectricRight;
    case Monitor::Top:
        return ElectricTop;
    case Monitor::Bottom:
        return ElectricBottom;
    case Monitor::TopLeft:
        return ElectricTopLeft;
    case Monitor::TopRight:
        return ElectricTopRight;
    case Monitor::BottomLeft:
        return ElectricBottomLeft;
    case Monitor::BottomRight:
        return ElectricBottomRight;
    default:
        return ElectricNone;
    }
}

void NWinScreenEdge::onChanged()
{
    bool needSave = isSaveNeeded();
    for (auto it = m_reference.cbegin(); it != m_reference.cend(); ++it) {
        needSave |= it.value() != monitor()->selectedEdgeItem(NWinScreenEdge::electricBorderToMonitorEdge(it.key()));
    }
    Q_EMIT saveNeededChanged(needSave);

    bool defaults = isDefault();
    for (auto it = m_default.cbegin(); it != m_default.cend(); ++it) {
        defaults &= it.value() == monitor()->selectedEdgeItem(NWinScreenEdge::electricBorderToMonitorEdge(it.key()));
    }
    Q_EMIT defaultChanged(defaults);
}

void NWinScreenEdge::createConnection()
{
    connect(monitor(),
            &Monitor::changed,
            this,
            &NWinScreenEdge::onChanged);
}

bool NWinScreenEdge::isSaveNeeded() const
{
    return false;
}

bool NWinScreenEdge::isDefault() const
{
    return true;
}

} // namespace

#include "moc_nwinscreenedge.cpp"
