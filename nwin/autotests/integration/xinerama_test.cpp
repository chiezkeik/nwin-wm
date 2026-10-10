/*
    SPDX-FileCopyrightText: 2024 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "nwin_wayland_test.h"

#include "core/output.h"
#include "core/outputconfiguration.h"
#include "wayland_server.h"
#include "workspace.h"

namespace NWin
{

class XineramaTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void indexToOutput();
};

void XineramaTest::initTestCase()
{
    QVERIFY(waylandServer()->init(qAppName()));
    nwinApp()->start();
}

void XineramaTest::indexToOutput()
{
    Test::setOutputConfig({
        Test::OutputInfo{
            .geometry = Rect(0, 0, 1280, 1024),
            .scale = 1.5,
        },
        Test::OutputInfo{
            .geometry = Rect(1280, 0, 1280, 1024),
            .scale = 1.5,
        },
    });
    nwinApp()->setXwaylandScale(1.5);

    // Start Xwayland
    Test::XcbConnectionPtr c = Test::createX11Connection();
    QVERIFY(!xcb_connection_has_error(c.get()));

    const auto outputs = workspace()->outputs();

    OutputConfiguration config;
    config.changeSet(outputs[0]->backendOutput())->priority = 0;
    config.changeSet(outputs[1]->backendOutput())->priority = 1;
    QCOMPARE(workspace()->applyOutputConfiguration(config), OutputConfigurationError::None);
    QCOMPARE(workspace()->xineramaIndexToOutput(0), outputs.at(0));
    QCOMPARE(workspace()->xineramaIndexToOutput(1), outputs.at(1));

    config.changeSet(outputs[0]->backendOutput())->priority = 1;
    config.changeSet(outputs[1]->backendOutput())->priority = 0;
    QCOMPARE(workspace()->applyOutputConfiguration(config), OutputConfigurationError::None);
    QCOMPARE(workspace()->xineramaIndexToOutput(0), outputs.at(1));
    QCOMPARE(workspace()->xineramaIndexToOutput(1), outputs.at(0));
}

} // namespace NWin

WAYLANDTEST_MAIN(NWin::XineramaTest)
#include "xinerama_test.moc"
