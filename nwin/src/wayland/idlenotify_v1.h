/*
    SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "nwin_export.h"

#include <QObject>

#include <memory>

struct wl_resource;

namespace NWin
{

class Display;
class IdleNotifyV1InterfacePrivate;

class NWIN_EXPORT IdleNotifyV1Interface : public QObject
{
    Q_OBJECT

public:
    explicit IdleNotifyV1Interface(Display *display, QObject *parent = nullptr);
    ~IdleNotifyV1Interface() override;

private:
    std::unique_ptr<IdleNotifyV1InterfacePrivate> d;
};

}
