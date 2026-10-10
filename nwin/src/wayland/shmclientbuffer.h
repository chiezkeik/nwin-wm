/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "nwin_export.h"

#include <QObject>

namespace NWin
{

class Display;
class ShmClientBufferIntegrationPrivate;

/**
 * The ShmClientBufferIntegration class provides support for shared memory client buffers.
 */
class NWIN_EXPORT ShmClientBufferIntegration : public QObject
{
    Q_OBJECT

public:
    explicit ShmClientBufferIntegration(Display *display);
    ~ShmClientBufferIntegration() override;

private:
    friend class ShmClientBufferIntegrationPrivate;
    std::unique_ptr<ShmClientBufferIntegrationPrivate> d;
};

} // namespace NWin
