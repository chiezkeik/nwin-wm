/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QHash>
#include <QTimer>
#include <private/kabstractidletimepoller_p.h>

namespace NWin
{

class IdleDetector;

class NWinIdleTimePoller : public KAbstractIdleTimePoller
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KAbstractIdleTimePoller_iid FILE "nwin.json")
    Q_INTERFACES(KAbstractIdleTimePoller)

public:
    NWinIdleTimePoller(QObject *parent = nullptr);

    bool isAvailable() override;
    bool setUpPoller() override;
    void unloadPoller() override;

public Q_SLOTS:
    void addTimeout(int nextTimeout) override;
    void removeTimeout(int nextTimeout) override;
    QList<int> timeouts() const override;
    int forcePollRequest() override;
    void catchIdleEvent() override;
    void stopCatchingIdleEvents() override;
    void simulateUserActivity() override;

private:
    IdleDetector *m_catchResumeTimeout = nullptr;
    QHash<int, IdleDetector *> m_timeouts;
};

}
