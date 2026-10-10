/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2017 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "gestures.h"

#include <QDebug>

#include <cmath>
#include <functional>
#include <utility>

namespace NWin
{

Gesture::Gesture()
{
}

Gesture::~Gesture() = default;

SwipeGesture::SwipeGesture(uint32_t fingerCount)
    : m_fingerCount(fingerCount)
{
}

SwipeGesture::~SwipeGesture() = default;

bool SwipeGesture::isEnabled() const
{
    return true;
}


PinchGesture::PinchGesture(uint32_t fingerCount)
    : m_fingerCount(fingerCount)
{
}

PinchGesture::~PinchGesture() = default;

qreal PinchGesture::scaleDeltaToProgress(const qreal &scaleDelta) const
{
    return std::abs(scaleDelta - 1) / s_minimumScaleDelta;
}

bool PinchGesture::minimumScaleDeltaReached(const qreal &scaleDelta) const
{
    return scaleDeltaToProgress(scaleDelta) >= 1.0;
}

GestureRecognizer::GestureRecognizer(QObject *parent)
    : QObject(parent)
{
}

GestureRecognizer::~GestureRecognizer() = default;

void GestureRecognizer::registerSwipeGesture(NWin::SwipeGesture *gesture)
{
    Q_ASSERT(!m_swipeGestures.contains(gesture));
    auto connection = connect(gesture, &QObject::destroyed, this, std::bind(&GestureRecognizer::unregisterSwipeGesture, this, gesture));
    m_destroyConnections.insert(gesture, connection);
    m_swipeGestures << gesture;
}

void GestureRecognizer::unregisterSwipeGesture(NWin::SwipeGesture *gesture)
{
    auto it = m_destroyConnections.find(gesture);
    if (it != m_destroyConnections.end()) {
        disconnect(it.value());
        m_destroyConnections.erase(it);
    }
    m_swipeGestures.removeAll(gesture);
    m_activeSwipeGestures.removeAll(gesture);
}

void GestureRecognizer::registerPinchGesture(NWin::PinchGesture *gesture)
{
    Q_ASSERT(!m_pinchGestures.contains(gesture));
    auto connection = connect(gesture, &QObject::destroyed, this, std::bind(&GestureRecognizer::unregisterPinchGesture, this, gesture));
    m_destroyConnections.insert(gesture, connection);
    m_pinchGestures << gesture;
}

void GestureRecognizer::unregisterPinchGesture(NWin::PinchGesture *gesture)
{
    auto it = m_destroyConnections.find(gesture);
    if (it != m_destroyConnections.end()) {
        disconnect(it.value());
        m_destroyConnections.erase(it);
    }
    m_pinchGestures.removeAll(gesture);
    if (m_activePinchGestures.removeOne(gesture)) {
        Q_EMIT gesture->cancelled();
    }
}

int GestureRecognizer::startSwipeGesture(uint fingerCount)
{
    if (m_swipeInProgress || !m_activePinchGestures.isEmpty()) {
        return 0;
    }
    resetSwipeGesture();
    m_swipeInProgress = true;
    for (SwipeGesture *gesture : std::as_const(m_swipeGestures)) {
        if (gesture->fingerCount() == fingerCount) {
            m_activeSwipeGestures.append(gesture);
        }
    }
    return m_activeSwipeGestures.size();
}

void GestureRecognizer::updateSwipeGesture(const QPointF &delta)
{
    if (!m_swipeInProgress || m_currentSwipeDirection != SwipeDirection::Invalid) {
        return;
    }
    m_currentDelta += delta;
    // Small diagonal noise must not execute a command.
    if (QPointF::dotProduct(m_currentDelta, m_currentDelta) < 16 * 16) {
        return;
    }
    if (std::abs(m_currentDelta.x()) > std::abs(m_currentDelta.y())) {
        m_currentSwipeDirection = m_currentDelta.x() < 0 ? SwipeDirection::Left : SwipeDirection::Right;
    } else {
        m_currentSwipeDirection = m_currentDelta.y() < 0 ? SwipeDirection::Up : SwipeDirection::Down;
    }

    // Snapshot eligibility before any command changes the state of another binding.
    // Keep the stroke latched, even if no binding matches this initial direction.
    m_activeSwipeGestures.removeIf([this](const QPointer<SwipeGesture> &gesture) {
        return !gesture || gesture->direction() != m_currentSwipeDirection || !gesture->isEnabled();
    });
    auto gestures = std::move(m_activeSwipeGestures);
    const auto generation = m_swipeGeneration;
    const QPointer<GestureRecognizer> guard(this);
    for (const auto &gesture : gestures) {
        if (gesture && m_swipeGestures.contains(gesture.data())) {
            Q_EMIT gesture->triggered();
            // Commands may destroy the recognizer, unregister other bindings, or
            // end this stroke and start another. Never dispatch stale commands.
            if (!guard || m_swipeGeneration != generation) {
                return;
            }
        }
    }
    // Reuse the candidate storage on the next stroke; reentrant strokes returned above.
    gestures.clear();
    m_activeSwipeGestures = std::move(gestures);
}

void GestureRecognizer::resetSwipeGesture()
{
    ++m_swipeGeneration;
    m_swipeInProgress = false;
    m_activeSwipeGestures.clear();
    m_currentDelta = QPointF();
    m_currentSwipeDirection = SwipeDirection::Invalid;
}

void GestureRecognizer::cancelActiveGestures()
{
    cancelSwipeGesture();
    const auto gestures = std::move(m_activePinchGestures);
    m_currentScale = 1;
    m_currentFingerCount = 0;
    for (PinchGesture *gesture : gestures) {
        Q_EMIT gesture->cancelled();
    }
}

void GestureRecognizer::cancelSwipeGesture()
{
    resetSwipeGesture();
}

void GestureRecognizer::endSwipeGesture()
{
    resetSwipeGesture();
}

int GestureRecognizer::startPinchGesture(uint fingerCount)
{
    if (m_swipeInProgress || !m_activePinchGestures.isEmpty()) {
        return 0;
    }
    m_currentFingerCount = fingerCount;
    int count = 0;
    for (PinchGesture *gesture : std::as_const(m_pinchGestures)) {
        if (gesture->fingerCount() != fingerCount) {
            continue;
        }

        // direction doesn't matter yet
        m_activePinchGestures << gesture;
        count++;
        Q_EMIT gesture->started();
    }
    return count;
}

void GestureRecognizer::updatePinchGesture(qreal scale, qreal angleDelta, const QPointF &posDelta)
{
    m_currentScale = scale;

    // Determine the direction of the swipe
    PinchDirection direction;
    if (scale < 1) {
        direction = PinchDirection::Contracting;
    } else {
        direction = PinchDirection::Expanding;
    }

    // Eliminate wrong gestures (takes two iterations)
    for (int i = 0; i < 2; i++) {
        if (m_activePinchGestures.isEmpty()) {
            startPinchGesture(m_currentFingerCount);
        }

        for (auto it = m_activePinchGestures.begin(); it != m_activePinchGestures.end();) {
            auto g = static_cast<PinchGesture *>(*it);

            if (g->direction() != direction) {
                Q_EMIT g->cancelled();
                it = m_activePinchGestures.erase(it);
                continue;
            }
            it++;
        }
    }

    for (PinchGesture *g : std::as_const(m_activePinchGestures)) {
        Q_EMIT g->progress(g->scaleDeltaToProgress(scale));
    }
}

void GestureRecognizer::cancelPinchGesture()
{
    cancelActiveGestures();
    m_currentScale = 1;
    m_currentFingerCount = 0;
}

void GestureRecognizer::endPinchGesture() // because fingers up
{
    for (auto g : std::as_const(m_activePinchGestures)) {
        if (g->minimumScaleDeltaReached(m_currentScale)) {
            Q_EMIT g->triggered();
        } else {
            Q_EMIT g->cancelled();
        }
    }
    m_activeSwipeGestures.clear();
    m_activePinchGestures.clear();
    m_currentScale = 1;
    m_currentFingerCount = 0;
}

uint32_t SwipeGesture::fingerCount() const
{
    return m_fingerCount;
}

SwipeDirection SwipeGesture::direction() const
{
    return m_direction;
}

void SwipeGesture::setDirection(SwipeDirection direction)
{
    m_direction = direction;
}

uint32_t PinchGesture::fingerCount() const
{
    return m_fingerCount;
}

PinchDirection PinchGesture::direction() const
{
    return m_direction;
}

void PinchGesture::setDirection(PinchDirection direction)
{
    m_direction = direction;
}

}

#include "moc_gestures.cpp"
