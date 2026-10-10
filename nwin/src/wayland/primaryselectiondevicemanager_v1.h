/*
    SPDX-FileCopyrightText: 2020 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#pragma once

#include "nwin_export.h"

#include <QObject>
#include <memory>

namespace NWin
{

class Display;
class PrimarySelectionSourceV1Interface;
class PrimarySelectionDeviceManagerV1InterfacePrivate;
class PrimarySelectionDeviceV1Interface;

/**
 * @brief Represents the Global for zwp_primary_selection_manager_v1 interface.
 *
 */
class NWIN_EXPORT PrimarySelectionDeviceManagerV1Interface : public QObject
{
    Q_OBJECT

public:
    explicit PrimarySelectionDeviceManagerV1Interface(Display *display, QObject *parent = nullptr);
    ~PrimarySelectionDeviceManagerV1Interface();

Q_SIGNALS:
    void dataSourceCreated(NWin::PrimarySelectionSourceV1Interface *dataSource);
    void dataDeviceCreated(NWin::PrimarySelectionDeviceV1Interface *dataDevice);

private:
    std::unique_ptr<PrimarySelectionDeviceManagerV1InterfacePrivate> d;
};

}
