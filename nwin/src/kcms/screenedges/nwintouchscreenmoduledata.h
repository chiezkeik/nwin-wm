/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2026 Alexander Wilms <f.alexander.wilms@outlook.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KCModuleData>

class OrgKdeNWinInputDeviceManagerInterface;

namespace NWin
{

class NWinTouchScreenModuleData : public KCModuleData
{
    Q_OBJECT

public:
    explicit NWinTouchScreenModuleData(QObject *parent = nullptr);

private Q_SLOTS:
    void onDeviceAdded();
    void onDeviceRemoved();

private:
    void updateRelevance();
    void checkDevices();

    OrgKdeNWinInputDeviceManagerInterface *m_deviceManager = nullptr;
    QStringList m_touchDevices;
};

}
