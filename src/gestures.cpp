/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2017 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "gestures.h"

#include <QDebug>

#include <cmath>
#include <functional>
#include <utility>

namespace KWin
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

void GestureRecognizer::registerSwipeGesture(KWin::SwipeGesture *gesture)
{
    Q_ASSERT(!m_swipeGestures.contains(gesture));
    auto connection = connect(gesture, &QObject::destroyed, this, std::bind(&GestureRecognizer::unregisterSwipeGesture, this, gesture));
    m_destroyConnections.insert(gesture, connection);
    m_swipeGestures << gesture;
}

void GestureRecognizer::unregisterSwipeGesture(KWin::SwipeGesture *gesture)
{
    auto it = m_destroyConnections.find(gesture);
    if (it != m_destroyConnections.end()) {
        disconnect(it.value());
        m_destroyConnections.erase(it);
    }
    m_swipeGestures.removeAll(gesture);
    m_activeSwipeGestures.removeAll(gesture);
}

void GestureRecognizer::registerPinchGesture(KWin::PinchGesture *gesture)
{
    Q_ASSERT(!m_pinchGestures.contains(gesture));
    auto connection = connect(gesture, &QObject::destroyed, this, std::bind(&GestureRecognizer::unregisterPinchGesture, this, gesture));
    m_destroyConnections.insert(gesture, connection);
    m_pinchGestures << gesture;
}

void GestureRecognizer::unregisterPinchGesture(KWin::PinchGesture *gesture)
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
    if ((m_swipeState & 7) != 0 || !m_activePinchGestures.isEmpty()) {
        return 0;
    }
    resetSwipeGesture();
    m_swipeState |= 1;
    for (SwipeGesture *gesture : std::as_const(m_swipeGestures)) {
        if (gesture->fingerCount() == fingerCount) {
            m_activeSwipeGestures.append(gesture);
        }
    }
    return m_activeSwipeGestures.size();
}

void GestureRecognizer::updateSwipeGesture(const QPointF &delta)
{
    if ((m_swipeState & 7) != 1) {
        return;
    }
    m_currentDelta += delta;
    // Small diagonal noise must not execute a command.
    if (QPointF::dotProduct(m_currentDelta, m_currentDelta) < 16 * 16) {
        return;
    }
    SwipeDirection direction;
    if (std::abs(m_currentDelta.x()) > std::abs(m_currentDelta.y())) {
        direction = m_currentDelta.x() < 0 ? SwipeDirection::Left : SwipeDirection::Right;
    } else {
        direction = m_currentDelta.y() < 0 ? SwipeDirection::Up : SwipeDirection::Down;
    }
    m_swipeState = (m_swipeState & ~uint32_t(7)) | (uint32_t(direction) + 1);

    // Snapshot eligibility before any command changes the state of another binding.
    // Keep the stroke latched, even if no binding matches this initial direction.
    m_activeSwipeGestures.removeIf([direction](const QPointer<SwipeGesture> &gesture) {
        return !gesture || gesture->direction() != direction || !gesture->isEnabled();
    });
    auto gestures = std::move(m_activeSwipeGestures);
    const auto generation = m_swipeState;
    const QPointer<GestureRecognizer> guard(this);
    for (const auto &gesture : gestures) {
        if (gesture && m_swipeGestures.contains(gesture.data())) {
            Q_EMIT gesture->triggered();
            // Commands may destroy the recognizer, unregister other bindings, or
            // end this stroke and start another. Never dispatch stale commands.
            if (!guard || m_swipeState != generation) {
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
    m_swipeState = (m_swipeState + 8) & ~uint32_t(7);
    m_activeSwipeGestures.clear();
    m_currentDelta = QPointF();
}

void GestureRecognizer::cancelActiveGestures()
{
    cancelSwipeGesture();
    for (auto gesture : std::as_const(m_activePinchGestures)) {
        Q_EMIT gesture->cancelled();
    }
    m_activePinchGestures.clear();
    m_currentScale = 0;
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
    m_currentFingerCount = fingerCount;
    if ((m_swipeState & 7) != 0 || !m_activePinchGestures.isEmpty()) {
        return 0;
    }
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

int GestureRecognizer::startSwipeGesture(uint fingerCount, const QPointF &)
{
    return startSwipeGesture(fingerCount);
}

}

#include "moc_gestures.cpp"
