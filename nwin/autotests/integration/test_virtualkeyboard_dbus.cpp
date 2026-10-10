/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2017 Martin Flöser <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "nwin_wayland_test.h"

#include "main.h"
#include "virtualkeyboard_dbus.h"
#include "wayland_server.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingReply>
#include <QSignalSpy>
#include <QTest>

#include <virtualkeyboardinterface.h>

using NWin::VirtualKeyboardDBus;
using namespace NWin;
using namespace NWin::Test;

class VirtualKeyboardDBusTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testEnabled();
    void testRequestMode_data();
    void testRequestMode();
    void init();
    void cleanup();
};

void VirtualKeyboardDBusTest::initTestCase()
{
    QDBusConnection::sessionBus().registerService(QStringLiteral("org.kde.nwin.testvirtualkeyboard"));
    QVERIFY(waylandServer()->init(qAppName()));

    static_cast<WaylandTestApplication *>(nwinApp())->setInputMethodServerToStart("internal");
    nwinApp()->start();
    Test::setOutputConfig({
        Rect(0, 0, 1280, 1024),
        Rect(1280, 0, 1280, 1024),
    });

    QVERIFY(setupWaylandConnection(AdditionalWaylandInterface::Seat | AdditionalWaylandInterface::InputMethodV1 | AdditionalWaylandInterface::TextInputManagerV2 | AdditionalWaylandInterface::TextInputManagerV3));
}

void VirtualKeyboardDBusTest::init()
{
    nwinApp()->inputMethod()->setMode(InputMethod::VirtualKeyboardVisibility::Never);
}

void VirtualKeyboardDBusTest::cleanup()
{
    Test::destroyWaylandConnection();
}

void VirtualKeyboardDBusTest::testEnabled()
{
    VirtualKeyboardDBus dbus(NWin::nwinApp()->inputMethod());
    OrgKdeNWinVirtualKeyboardInterface iface(QStringLiteral("org.kde.nwin.testvirtualkeyboard"), QStringLiteral("/VirtualKeyboard"), QDBusConnection::sessionBus());
    QSignalSpy helperChangedSpy(&iface, &OrgKdeNWinVirtualKeyboardInterface::modeChanged);

    QCOMPARE(dbus.isEnabled(), false);
    QCOMPARE(dbus.property("mode"), 0);
    QSignalSpy modeChangedSpy(&dbus, &VirtualKeyboardDBus::modeChanged);

    QVERIFY(iface.isValid());
    QCOMPARE(iface.mode(), 0);

    dbus.setMode(1);
    QCOMPARE(modeChangedSpy.count(), 1);
    QVERIFY(helperChangedSpy.wait());
    QCOMPARE(helperChangedSpy.count(), 1);
    QCOMPARE(dbus.isEnabled(), true);
    QCOMPARE(dbus.property("mode"), 1);
    QCOMPARE(iface.mode(), 1);

    // setting again to enabled should not change anything
    dbus.setMode(1);
    QCOMPARE(modeChangedSpy.count(), 1);

    // back to false
    dbus.setMode(0);
    QCOMPARE(modeChangedSpy.count(), 2);
    QVERIFY(helperChangedSpy.wait());
    QCOMPARE(helperChangedSpy.count(), 2);
    QCOMPARE(dbus.isEnabled(), false);
    QCOMPARE(dbus.property("mode"), 0);
    QCOMPARE(iface.mode(), 0);
}

void VirtualKeyboardDBusTest::testRequestMode_data()
{
    QTest::addColumn<QString>("method");
    QTest::addColumn<int>("expectedResult");

    QTest::newRow("Never show") << QStringLiteral("setMode") << 0;
    QTest::newRow("Show with non-mouse") << QStringLiteral("setMode") << 1;
    QTest::newRow("Always show") << QStringLiteral("setMode") << 2;
}

void VirtualKeyboardDBusTest::testRequestMode()
{
    QFETCH(QString, method);
    QFETCH(int, expectedResult);

    VirtualKeyboardDBus dbus(NWin::nwinApp()->inputMethod());
    OrgKdeNWinVirtualKeyboardInterface iface(QStringLiteral("org.kde.nwin.testvirtualkeyboard"), QStringLiteral("/VirtualKeyboard"), QDBusConnection::sessionBus());

    iface.setMode(expectedResult);
    QTRY_COMPARE(iface.mode(), expectedResult);
}

WAYLANDTEST_MAIN(VirtualKeyboardDBusTest)
#include "test_virtualkeyboard_dbus.moc"
