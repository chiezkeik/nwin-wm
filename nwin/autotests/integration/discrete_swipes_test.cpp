/*
    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "nwin_wayland_test.h"

#include "compositor.h"
#include "effect/effecthandler.h"
#include "effect/effectloader.h"
#include "input.h"
#include "pointer_input.h"
#include "virtualdesktops.h"
#include "wayland_server.h"
#include "workspace.h"

#include <KConfigGroup>
#include <KWayland/Client/surface.h>

using namespace NWin;
using namespace std::chrono_literals;

class DiscreteSwipesTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void init();
    void cleanup();
    void desktopCommandsBeforeRelease();
    void overviewDoesNotChainTransitions();
};

void DiscreteSwipesTest::initTestCase()
{
    qRegisterMetaType<NWin::Effect *>();
    QVERIFY(waylandServer()->init(qAppName()));
    auto config = KSharedConfig::openConfig(QString(), KConfig::SimpleConfig);
    KConfigGroup plugins(config, QStringLiteral("Plugins"));
    for (const auto &name : EffectLoader().listOfKnownEffects()) {
        plugins.writeEntry(name + QStringLiteral("Enabled"), false);
    }
    config->sync();
    nwinApp()->setConfig(config);
    // The default integration renderer is QPainter; Overview requires OpenGL.
    qputenv("NWIN_COMPOSE", QByteArrayLiteral("O2"));
    nwinApp()->start();
    QVERIFY(Compositor::self());
    Test::setOutputConfig({Rect(0, 0, 1280, 1024)});
}

void DiscreteSwipesTest::init()
{
    QVERIFY(Test::setupWaylandConnection());
    workspace()->setActiveOutput(QPoint(640, 512));
    input()->pointer()->warp(QPoint(640, 512));
    auto desktops = VirtualDesktopManager::self();
    desktops->setPerOutputVirtualDesktops(false);
    desktops->setCount(4);
    desktops->setRows(1);
    desktops->setNavigationWrappingAround(false);
    desktops->setCurrent(2);
}

void DiscreteSwipesTest::cleanup()
{
    input()->pointer()->processSwipeGestureCancelled(10s);
    if (effects->isEffectLoaded(QStringLiteral("overview"))) {
        effects->unloadEffect(QStringLiteral("overview"));
    }
    Test::destroyWaylandConnection();
}

void DiscreteSwipesTest::desktopCommandsBeforeRelease()
{
    auto desktops = VirtualDesktopManager::self();
    auto pointer = input()->pointer();
    QSignalSpy changed(desktops, &VirtualDesktopManager::currentChanged);
    QSignalSpy preview(desktops, &VirtualDesktopManager::currentChanging);
    pointer->processSwipeGestureBegin(3, 0ms);
    pointer->processSwipeGestureUpdate(QPointF(-15, 0), 10ms);
    QCOMPARE(desktops->current(), 2u);
    pointer->processSwipeGestureUpdate(QPointF(-1, 0), 20ms);
    QCOMPARE(desktops->current(), 3u);
    QCOMPARE(changed.count(), 1);
    QCOMPARE(preview.count(), 0);
    pointer->processSwipeGestureUpdate(QPointF(-900, 0), 200ms);
    pointer->processSwipeGestureUpdate(QPointF(2000, 0), 400ms);
    QCOMPARE(desktops->current(), 3u);
    pointer->processSwipeGestureEnd(600ms);
    QCOMPARE(changed.count(), 1);

    pointer->processSwipeGestureBegin(3, 1000ms);
    pointer->processSwipeGestureUpdate(QPointF(-16, 0), 1020ms);
    QCOMPARE(desktops->current(), 4u);
    pointer->processSwipeGestureCancelled(1021ms);
    QCOMPARE(desktops->current(), 4u);
    QCOMPARE(changed.count(), 2);

    // Preserve the desktop grid and wrapping policy, not a linear carousel.
    pointer->processSwipeGestureBegin(3, 2000ms);
    pointer->processSwipeGestureUpdate(QPointF(-16, 0), 2020ms);
    QCOMPARE(desktops->current(), 4u);
    pointer->processSwipeGestureEnd(2040ms);
    desktops->setNavigationWrappingAround(true);
    pointer->processSwipeGestureBegin(4, 3000ms);
    pointer->processSwipeGestureUpdate(QPointF(-16, 0), 3020ms);
    QCOMPARE(desktops->current(), 1u);
    pointer->processSwipeGestureEnd(3040ms);

    desktops->setRows(2);
    desktops->setCurrent(1);
    pointer->processSwipeGestureBegin(3, 4000ms);
    pointer->processSwipeGestureUpdate(QPointF(0, -16), 4020ms);
    QCOMPARE(desktops->current(), 3u);
    pointer->processSwipeGestureEnd(4040ms);
    QCOMPARE(preview.count(), 0);
}

void DiscreteSwipesTest::overviewDoesNotChainTransitions()
{
    std::unique_ptr<KWayland::Client::Surface> surface(Test::createSurface());
    std::unique_ptr<Test::XdgToplevel> shellSurface(Test::createXdgToplevelSurface(surface.get()));
    QVERIFY(Test::renderAndWaitForShown(surface.get(), QSize(300, 200), Qt::blue));
    auto loader = effects->findChild<AbstractEffectLoader *>();
    QVERIFY(loader);
    QSignalSpy loaded(loader, &AbstractEffectLoader::effectLoaded);
    QVERIFY(effects->loadEffect(QStringLiteral("overview")));
    QCOMPARE(loaded.count(), 1);
    auto overview = loaded.first().first().value<Effect *>();
    QVERIFY(overview);
    auto pointer = input()->pointer();

    pointer->processSwipeGestureBegin(4, 0ms);
    pointer->processSwipeGestureUpdate(QPointF(0, -15), 10ms);
    QCOMPARE(overview->property("overviewPartialActivationFactor").toReal(), 0.0);
    pointer->processSwipeGestureUpdate(QPointF(0, -1), 20ms);
    QCOMPARE(overview->property("overviewPartialActivationFactor").toReal(), 1.0);
    QCOMPARE(overview->property("gridPartialActivationFactor").toReal(), 0.0);
    QVERIFY(!overview->property("overviewGestureInProgress").toBool());
    QVERIFY(!overview->property("transitionGestureInProgress").toBool());
    pointer->processSwipeGestureUpdate(QPointF(0, -900), 200ms);
    pointer->processSwipeGestureUpdate(QPointF(0, 2000), 400ms);
    QCOMPARE(overview->property("overviewPartialActivationFactor").toReal(), 1.0);
    QCOMPARE(overview->property("gridPartialActivationFactor").toReal(), 0.0);
    pointer->processSwipeGestureCancelled(600ms);
    QCOMPARE(overview->property("overviewPartialActivationFactor").toReal(), 1.0);

    // A separate physical stroke can move Overview into Grid immediately.
    pointer->processSwipeGestureBegin(4, 1000ms);
    pointer->processSwipeGestureUpdate(QPointF(0, -16), 1020ms);
    QCOMPARE(overview->property("gridPartialActivationFactor").toReal(), 1.0);
    QVERIFY(!overview->property("transitionGestureInProgress").toBool());
    QVERIFY(!overview->property("gridGestureInProgress").toBool());
    pointer->processSwipeGestureUpdate(QPointF(0, 900), 1040ms);
    pointer->processSwipeGestureEnd(1060ms);
    QCOMPARE(overview->property("gridPartialActivationFactor").toReal(), 1.0);

    // Grid -> Overview is likewise a command, not a reversible preview.
    pointer->processSwipeGestureBegin(4, 2000ms);
    pointer->processSwipeGestureUpdate(QPointF(0, 16), 2020ms);
    QCOMPARE(overview->property("gridPartialActivationFactor").toReal(), 0.0);
    QCOMPARE(overview->property("overviewPartialActivationFactor").toReal(), 1.0);
    pointer->processSwipeGestureCancelled(2040ms);
    QCOMPARE(overview->property("overviewPartialActivationFactor").toReal(), 1.0);

    pointer->processSwipeGestureBegin(4, 3000ms);
    pointer->processSwipeGestureUpdate(QPointF(0, 16), 3020ms);
    QCOMPARE(overview->property("overviewPartialActivationFactor").toReal(), 0.0);
    pointer->processSwipeGestureEnd(3040ms);
    QTRY_VERIFY(!overview->isActive());
}

WAYLANDTEST_MAIN(DiscreteSwipesTest)
#include "discrete_swipes_test.moc"
