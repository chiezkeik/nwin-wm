/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2026 Alexander Wilms <f.alexander.wilms@outlook.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "nwintouchscreenmoduledata.h"

#include "inputdevicemanager_interface.h"

#include <QDBusConnection>

namespace NWin
{

NWinTouchScreenModuleData::NWinTouchScreenModuleData(QObject *parent)
    : KCModuleData(parent)
{
    m_deviceManager = new OrgKdeNWinInputDeviceManagerInterface(
        QStringLiteral("org.kde.NWin"),
        QStringLiteral("/org/kde/NWin/InputDevice"),
        QDBusConnection::sessionBus(),
        this);

    connect(m_deviceManager, &OrgKdeNWinInputDeviceManagerInterface::deviceAdded,
            this, &NWinTouchScreenModuleData::onDeviceAdded);
    connect(m_deviceManager, &OrgKdeNWinInputDeviceManagerInterface::deviceRemoved,
            this, &NWinTouchScreenModuleData::onDeviceRemoved);

    checkDevices();
}

void NWinTouchScreenModuleData::checkDevices()
{
    m_touchDevices = m_deviceManager->ListTouch();
    updateRelevance();
}

void NWinTouchScreenModuleData::onDeviceAdded()
{
    checkDevices();
}

void NWinTouchScreenModuleData::onDeviceRemoved()
{
    checkDevices();
}

void NWinTouchScreenModuleData::updateRelevance()
{
    setRelevant(!m_touchDevices.isEmpty());
}

}

#include "moc_nwintouchscreenmoduledata.cpp"
