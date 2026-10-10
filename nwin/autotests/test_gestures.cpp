/*
    SPDX-FileCopyrightText: 2017 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "gestures.h"

#include <QSignalSpy>
#include <QTest>

#include <memory>

using namespace NWin;
Q_DECLARE_METATYPE(SwipeDirection);

class GestureTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testImmediateCommand();
    void testDirection_data();
    void testDirection();
    void testCancellation();
    void testFingerCountAndLifecycle();
    void testEligibilitySnapshot();
    void testRegistrationSnapshot();
    void testReentrantReset();
    void testCommandDestruction_data();
    void testCommandDestruction();
    void testRecognizerDestruction();
    void testMinimumScaleDelta();
    void testUnregisterPinchCancels();
    void testPinchDirection();
};

void GestureTest::testImmediateCommand()
{
    GestureRecognizer recognizer;
    SwipeGesture up(4), down(4);
    up.setDirection(SwipeDirection::Up);
    down.setDirection(SwipeDirection::Down);
    recognizer.registerSwipeGesture(&up);
    recognizer.registerSwipeGesture(&down);
    QSignalSpy triggered(&up, &SwipeGesture::triggered);
    QSignalSpy opposite(&down, &SwipeGesture::triggered);
    QSignalSpy cancelled(&up, &SwipeGesture::cancelled);
    QSignalSpy started(&up, &SwipeGesture::started);

    QCOMPARE(recognizer.startSwipeGesture(4), 2);
    recognizer.updateSwipeGesture(QPointF(5, -5));
    QCOMPARE(triggered.count(), 0);
    recognizer.updateSwipeGesture(QPointF(-5, -10.5));
    QCOMPARE(triggered.count(), 0);
    recognizer.updateSwipeGesture(QPointF(0, -0.5));
    QCOMPARE(triggered.count(), 1); // Synchronous, before lift.
    recognizer.updateSwipeGesture(QPointF(0, -900));
    recognizer.updateSwipeGesture(QPointF(0, 2000));
    recognizer.updateSwipeGesture(QPointF(900, 0));
    QCOMPARE(triggered.count(), 1);
    QCOMPARE(opposite.count(), 0);
    recognizer.endSwipeGesture();
    QCOMPARE(triggered.count(), 1);
    QCOMPARE(cancelled.count(), 0);
    QCOMPARE(started.count(), 0); // No interactive swipe lifecycle.
}

void GestureTest::testDirection_data()
{
    QTest::addColumn<QPointF>("delta");
    QTest::addColumn<SwipeDirection>("direction");
    QTest::newRow("up") << QPointF(0, -16) << SwipeDirection::Up;
    QTest::newRow("down") << QPointF(0, 16) << SwipeDirection::Down;
    QTest::newRow("left") << QPointF(-16, 0) << SwipeDirection::Left;
    QTest::newRow("right") << QPointF(16, 0) << SwipeDirection::Right;
    QTest::newRow("horizontal-major") << QPointF(-13, 12) << SwipeDirection::Left;
    QTest::newRow("vertical-major") << QPointF(12, -13) << SwipeDirection::Up;
    QTest::newRow("diagonal-tie-up") << QPointF(-12, -12) << SwipeDirection::Up;
    QTest::newRow("diagonal-tie-down") << QPointF(12, 12) << SwipeDirection::Down;
}

void GestureTest::testDirection()
{
    QFETCH(QPointF, delta);
    QFETCH(SwipeDirection, direction);
    GestureRecognizer recognizer;
    SwipeGesture up(3), down(3), left(3), right(3);
    up.setDirection(SwipeDirection::Up);
    down.setDirection(SwipeDirection::Down);
    left.setDirection(SwipeDirection::Left);
    right.setDirection(SwipeDirection::Right);
    SwipeDirection fired = SwipeDirection::Invalid;
    int count = 0;
    for (auto gesture : {&up, &down, &left, &right}) {
        recognizer.registerSwipeGesture(gesture);
        connect(gesture, &SwipeGesture::triggered, &recognizer, [&, gesture]() {
            fired = gesture->direction();
            ++count;
        });
    }
    recognizer.startSwipeGesture(3);
    recognizer.updateSwipeGesture(delta);
    QCOMPARE(fired, direction);
    QCOMPARE(count, 1);
    recognizer.updateSwipeGesture(-10 * delta);
    recognizer.endSwipeGesture();
    QCOMPARE(count, 1);
}

void GestureTest::testCancellation()
{
    GestureRecognizer recognizer;
    SwipeGesture gesture(3);
    gesture.setDirection(SwipeDirection::Left);
    recognizer.registerSwipeGesture(&gesture);
    QSignalSpy triggered(&gesture, &SwipeGesture::triggered);
    QSignalSpy cancelled(&gesture, &SwipeGesture::cancelled);
    recognizer.startSwipeGesture(3);
    recognizer.updateSwipeGesture(QPointF(-15, 0));
    recognizer.cancelSwipeGesture();
    recognizer.updateSwipeGesture(QPointF(-1000, 0));
    recognizer.endSwipeGesture();
    QCOMPARE(triggered.count(), 0);
    QCOMPARE(cancelled.count(), 0);
    recognizer.startSwipeGesture(3);
    recognizer.updateSwipeGesture(QPointF(-16, 0));
    QCOMPARE(triggered.count(), 1);
    recognizer.cancelSwipeGesture();
    recognizer.endSwipeGesture();
    QCOMPARE(triggered.count(), 1);
    QCOMPARE(cancelled.count(), 0);
}

void GestureTest::testFingerCountAndLifecycle()
{
    GestureRecognizer recognizer;
    SwipeGesture gesture(4);
    gesture.setDirection(SwipeDirection::Up);
    recognizer.registerSwipeGesture(&gesture);
    QSignalSpy triggered(&gesture, &SwipeGesture::triggered);
    recognizer.updateSwipeGesture(QPointF(0, -16));
    QCOMPARE(triggered.count(), 0);
    QCOMPARE(recognizer.startSwipeGesture(3), 0);
    QCOMPARE(recognizer.startSwipeGesture(4), 0); // Ignore duplicate begin.
    recognizer.updateSwipeGesture(QPointF(0, -16));
    recognizer.endSwipeGesture();
    QCOMPARE(triggered.count(), 0);
    QCOMPARE(recognizer.startSwipeGesture(4), 1);
    recognizer.updateSwipeGesture(QPointF(16, 0)); // Unbound direction still latches.
    recognizer.updateSwipeGesture(QPointF(0, -1000));
    recognizer.endSwipeGesture();
    QCOMPARE(triggered.count(), 0);
    recognizer.startSwipeGesture(4);
    recognizer.updateSwipeGesture(QPointF(0, -16));
    QCOMPARE(triggered.count(), 1);
    recognizer.endSwipeGesture();
    recognizer.startSwipeGesture(4);
    recognizer.updateSwipeGesture(QPointF(0, -16));
    QCOMPARE(triggered.count(), 2);
}

void GestureTest::testEligibilitySnapshot()
{
    class ConditionalSwipe : public SwipeGesture
    {
    public:
        ConditionalSwipe(const int &state, int source)
            : SwipeGesture(4), state(state), source(source)
        {
            setDirection(SwipeDirection::Up);
        }
        bool isEnabled() const override { return state == source; }
        const int &state;
        const int source;
    };
    int state = 0;
    ConditionalSwipe overview(state, 0), grid(state, 1);
    GestureRecognizer recognizer;
    recognizer.registerSwipeGesture(&overview);
    recognizer.registerSwipeGesture(&grid);
    connect(&overview, &SwipeGesture::triggered, &recognizer, [&]() { state = 1; });
    connect(&grid, &SwipeGesture::triggered, &recognizer, [&]() { state = 2; });
    recognizer.startSwipeGesture(4);
    recognizer.updateSwipeGesture(QPointF(0, -16));
    QCOMPARE(state, 1);
    recognizer.updateSwipeGesture(QPointF(0, -1000));
    recognizer.endSwipeGesture();
    QCOMPARE(state, 1);
    recognizer.startSwipeGesture(4);
    recognizer.updateSwipeGesture(QPointF(0, -16));
    QCOMPARE(state, 2);
}

void GestureTest::testRegistrationSnapshot()
{
    GestureRecognizer recognizer;
    SwipeGesture first(3), later(3);
    first.setDirection(SwipeDirection::Up);
    later.setDirection(SwipeDirection::Up);
    recognizer.registerSwipeGesture(&first);
    QSignalSpy triggered(&later, &SwipeGesture::triggered);
    connect(&first, &SwipeGesture::triggered, &recognizer, [&]() {
        recognizer.registerSwipeGesture(&later);
        recognizer.unregisterSwipeGesture(&first);
    });
    recognizer.startSwipeGesture(3);
    recognizer.updateSwipeGesture(QPointF(0, -16));
    QCOMPARE(triggered.count(), 0);
    recognizer.endSwipeGesture();
    recognizer.startSwipeGesture(3);
    recognizer.updateSwipeGesture(QPointF(0, -16));
    QCOMPARE(triggered.count(), 1);
}

void GestureTest::testReentrantReset()
{
    GestureRecognizer recognizer;
    SwipeGesture first(3), second(3);
    first.setDirection(SwipeDirection::Up);
    second.setDirection(SwipeDirection::Up);
    recognizer.registerSwipeGesture(&first);
    recognizer.registerSwipeGesture(&second);
    QSignalSpy triggered(&second, &SwipeGesture::triggered);
    const auto connection = connect(&first, &SwipeGesture::triggered, &recognizer, [&]() {
        recognizer.cancelSwipeGesture();
        recognizer.startSwipeGesture(3);
    });
    recognizer.startSwipeGesture(3);
    recognizer.updateSwipeGesture(QPointF(0, -16));
    QCOMPARE(triggered.count(), 0); // Old dispatch must not enter the new stroke.
    disconnect(connection);
    recognizer.updateSwipeGesture(QPointF(0, -16));
    QCOMPARE(triggered.count(), 1);
}

void GestureTest::testCommandDestruction_data()
{
    QTest::addColumn<bool>("destroy");
    QTest::newRow("unregister") << false;
    QTest::newRow("destroy") << true;
}

void GestureTest::testCommandDestruction()
{
    QFETCH(bool, destroy);
    GestureRecognizer recognizer;
    SwipeGesture first(3);
    auto second = std::make_unique<SwipeGesture>(3);
    first.setDirection(SwipeDirection::Up);
    second->setDirection(SwipeDirection::Up);
    recognizer.registerSwipeGesture(&first);
    recognizer.registerSwipeGesture(second.get());
    int count = 0;
    connect(second.get(), &SwipeGesture::triggered, &recognizer, [&]() { ++count; });
    connect(&first, &SwipeGesture::triggered, &recognizer, [&]() {
        if (destroy) {
            second.reset();
        } else {
            recognizer.unregisterSwipeGesture(second.get());
        }
    });
    recognizer.startSwipeGesture(3);
    recognizer.updateSwipeGesture(QPointF(0, -16));
    QCOMPARE(count, 0);
    recognizer.endSwipeGesture();
}

void GestureTest::testRecognizerDestruction()
{
    auto recognizer = std::make_unique<GestureRecognizer>();
    SwipeGesture first(3), second(3);
    first.setDirection(SwipeDirection::Up);
    second.setDirection(SwipeDirection::Up);
    recognizer->registerSwipeGesture(&first);
    recognizer->registerSwipeGesture(&second);
    QSignalSpy triggered(&second, &SwipeGesture::triggered);
    connect(&first, &SwipeGesture::triggered, &first, [&]() { recognizer.reset(); });
    recognizer->startSwipeGesture(3);
    recognizer->updateSwipeGesture(QPointF(0, -16));
    QVERIFY(!recognizer);
    QCOMPARE(triggered.count(), 0);
}

void GestureTest::testMinimumScaleDelta()
{
    PinchGesture gesture(4);
    gesture.setDirection(PinchDirection::Contracting);
    QCOMPARE(gesture.minimumScaleDeltaReached(1.1), false);
    QCOMPARE(gesture.minimumScaleDeltaReached(1.3), true);
    GestureRecognizer recognizer;
    recognizer.registerPinchGesture(&gesture);
    QSignalSpy triggered(&gesture, &PinchGesture::triggered);
    recognizer.startPinchGesture(4);
    recognizer.updatePinchGesture(0.5, 0, QPointF());
    recognizer.endPinchGesture();
    QCOMPARE(triggered.count(), 1);
}

void GestureTest::testUnregisterPinchCancels()
{
    GestureRecognizer recognizer;
    auto gesture = std::make_unique<PinchGesture>(1);
    QSignalSpy cancelled(gesture.get(), &PinchGesture::cancelled);
    recognizer.registerPinchGesture(gesture.get());
    recognizer.startPinchGesture(1);
    recognizer.unregisterPinchGesture(gesture.get());
    QCOMPARE(cancelled.count(), 1);
    gesture.reset();
    QCOMPARE(cancelled.count(), 1);
}

void GestureTest::testPinchDirection()
{
    GestureRecognizer recognizer;
    PinchGesture expand(4), contract(4);
    expand.setDirection(PinchDirection::Expanding);
    contract.setDirection(PinchDirection::Contracting);
    recognizer.registerPinchGesture(&expand);
    recognizer.registerPinchGesture(&contract);
    QSignalSpy expandProgress(&expand, &PinchGesture::progress);
    QSignalSpy contractProgress(&contract, &PinchGesture::progress);
    recognizer.startPinchGesture(4);
    QCOMPARE(expandProgress.count(), 0);
    QCOMPARE(contractProgress.count(), 0);
    recognizer.updatePinchGesture(0.5, 0, QPointF());
    QCOMPARE(expandProgress.count(), 0);
    QCOMPARE(contractProgress.count(), 1);
    recognizer.updatePinchGesture(1.5, 0, QPointF());
    QCOMPARE(expandProgress.count(), 1);
    QCOMPARE(contractProgress.count(), 1);
}

QTEST_MAIN(GestureTest)
#include "test_gestures.moc"
