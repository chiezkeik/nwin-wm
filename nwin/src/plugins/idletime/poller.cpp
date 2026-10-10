/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "poller.h"
#include "idledetector.h"
#include "input.h"

namespace NWin
{

NWinIdleTimePoller::NWinIdleTimePoller(QObject *parent)
    : KAbstractIdleTimePoller(parent)
{
}

bool NWinIdleTimePoller::isAvailable()
{
    return true;
}

bool NWinIdleTimePoller::setUpPoller()
{
    return true;
}

void NWinIdleTimePoller::unloadPoller()
{
}

void NWinIdleTimePoller::addTimeout(int nextTimeout)
{
    if (m_timeouts.contains(nextTimeout)) {
        return;
    }

    auto detector = new IdleDetector(std::chrono::milliseconds(nextTimeout), IdleDetector::OperatingMode::FollowsInhibitors, this);
    m_timeouts.insert(nextTimeout, detector);
    connect(detector, &IdleDetector::idle, this, [this, nextTimeout] {
        Q_EMIT timeoutReached(nextTimeout);
    });
    connect(detector, &IdleDetector::resumed, this, &NWinIdleTimePoller::resumingFromIdle);
}

void NWinIdleTimePoller::removeTimeout(int nextTimeout)
{
    delete m_timeouts.take(nextTimeout);
}

QList<int> NWinIdleTimePoller::timeouts() const
{
    return m_timeouts.keys();
}

void NWinIdleTimePoller::catchIdleEvent()
{
    if (m_catchResumeTimeout) {
        // already setup
        return;
    }
    m_catchResumeTimeout = new IdleDetector(std::chrono::milliseconds::zero(), IdleDetector::OperatingMode::FollowsInhibitors, this);
    connect(m_catchResumeTimeout, &IdleDetector::resumed, this, [this]() {
        m_catchResumeTimeout->deleteLater();
        m_catchResumeTimeout = nullptr;
        Q_EMIT resumingFromIdle();
    });
}

void NWinIdleTimePoller::stopCatchingIdleEvents()
{
    delete m_catchResumeTimeout;
    m_catchResumeTimeout = nullptr;
}

int NWinIdleTimePoller::forcePollRequest()
{
    return 0;
}

void NWinIdleTimePoller::simulateUserActivity()
{
    input()->simulateUserActivity();
}

} // namespace NWin

#include "moc_poller.cpp"
