/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2020 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#pragma once

#include "nwin_export.h"

#include <QObject>
#include <memory>

#include "datadevice.h"

namespace NWin
{

class Display;
class DataSourceInterface;
class DataDeviceManagerInterfacePrivate;

/**
 * @brief Represents the Global for wl_data_device_manager interface.
 *
 */
class NWIN_EXPORT DataDeviceManagerInterface : public QObject
{
    Q_OBJECT

public:
    explicit DataDeviceManagerInterface(Display *display, QObject *parent = nullptr);
    ~DataDeviceManagerInterface() override;

Q_SIGNALS:
    void dataSourceCreated(NWin::DataSourceInterface *);
    void dataDeviceCreated(NWin::DataDeviceInterface *);

private:
    std::unique_ptr<DataDeviceManagerInterfacePrivate> d;
};

}
