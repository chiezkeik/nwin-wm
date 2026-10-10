/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2020 <davidedmundson@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

/**
 * This tiny executable creates a socket, then starts nwin passing it the FD to the wayland socket
 * along with the name of the socket to use
 * On any non-zero nwin exit nwin gets restarted.
 *
 * After restart nwin is relaunched but now with the NWIN_RESTART_COUNT env set to an incrementing counter
 *
 * It's somewhat  akin to systemd socket activation, but we also need the lock file, finding the next free socket
 * and so on, hence our own binary.
 *
 * Usage nwin_wayland_wrapper [argForKwin] [argForKwin] ...
 */

#include "config-nwin.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDebug>
#include <QProcess>
#include <QTemporaryFile>

#include <KSignalHandler>
#include <KUpdateLaunchEnvironmentJob>

#include <signal.h>

#include "wl-socket.h"
#include "wrapper_logging.h"

#if NWIN_BUILD_X11
#include "xauthority.h"
#include "xwaylandsocket.h"
#endif

using namespace std::chrono_literals;

class NWinWrapper : public QObject
{
    Q_OBJECT

public:
    NWinWrapper(QObject *parent);
    ~NWinWrapper();

    void run();
    void restart();
    void terminate(std::chrono::milliseconds timeout);

private:
    wl_socket *m_socket;

    int m_crashCount = 0;
    QProcess *m_nwinProcess = nullptr;

    const std::chrono::microseconds m_watchdogInterval;
    bool m_watchdogIntervalOk;

#if NWIN_BUILD_X11
    std::unique_ptr<NWin::XwaylandSocket> m_xwlSocket;
    QTemporaryFile m_xauthorityFile;
#endif
};

NWinWrapper::NWinWrapper(QObject *parent)
    : QObject(parent)
    , m_nwinProcess(new QProcess(this))
    , m_watchdogInterval(std::chrono::microseconds(qEnvironmentVariableIntValue("WATCHDOG_USEC", &m_watchdogIntervalOk) / 2))
{
    m_socket = wl_socket_create();
    if (!m_socket) {
        qFatal("Could not create wayland socket");
    }

#if NWIN_BUILD_X11
    if (qApp->arguments().contains(QLatin1StringView("--xwayland"))) {
        m_xwlSocket = std::make_unique<NWin::XwaylandSocket>(NWin::XwaylandSocket::OperationMode::TransferFdsOnExec);
        if (!m_xwlSocket->isValid()) {
            qCWarning(NWIN_WRAPPER) << "Failed to create Xwayland connection sockets";
            m_xwlSocket.reset();
        }
        if (m_xwlSocket) {
            if (!qEnvironmentVariableIsSet("NWIN_WAYLAND_NO_XAUTHORITY")) {
                if (!generateXauthorityFile(m_xwlSocket->display(), &m_xauthorityFile)) {
                    qCWarning(NWIN_WRAPPER) << "Failed to create an Xauthority file";
                }
            }
        }
    }
#endif
}

NWinWrapper::~NWinWrapper()
{
    wl_socket_destroy(m_socket);
    terminate(30s);
}

void NWinWrapper::run()
{
    m_nwinProcess->setProgram("nwin_wayland");

    QStringList args;

    args << "--wayland-fd" << QString::number(wl_socket_get_fd(m_socket));
    args << "--socket" << QString::fromUtf8(wl_socket_get_display_name(m_socket));

#if NWIN_BUILD_X11
    if (m_xwlSocket) {
        const auto xwaylandFileDescriptors = m_xwlSocket->fileDescriptors();
        for (const int &fileDescriptor : xwaylandFileDescriptors) {
            args << "--xwayland-fd" << QString::number(fileDescriptor);
        }
        args << "--xwayland-display" << m_xwlSocket->name();
        if (m_xauthorityFile.open()) {
            args << "--xwayland-xauthority" << m_xauthorityFile.fileName();
        }
    }
#endif

    // attach our main process arguments
    // the first entry is dropped as it will be our program name
    args << qApp->arguments().mid(1);

    m_nwinProcess->setProcessChannelMode(QProcess::ForwardedChannels);
    m_nwinProcess->setArguments(args);

    connect(m_nwinProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode == 0) {
            qApp->quit();
            return;
        } else if (exitCode == 133) {
            m_crashCount = 0;
        } else {
            m_crashCount++;
        }

        if (m_crashCount > 10) {
            qApp->quit();
            return;
        }
        qputenv("NWIN_RESTART_COUNT", QByteArray::number(m_crashCount));
        // restart
        m_nwinProcess->start();
    });

    m_nwinProcess->start();

    QProcessEnvironment env;
    env.insert("WAYLAND_DISPLAY", QString::fromUtf8(wl_socket_get_display_name(m_socket)));
#if NWIN_BUILD_X11
    if (m_xwlSocket) {
        env.insert("DISPLAY", m_xwlSocket->name());
        if (m_xauthorityFile.open()) {
            env.insert("XAUTHORITY", m_xauthorityFile.fileName());
        }
    }
#endif
    auto envSyncJob = new KUpdateLaunchEnvironmentJob(env);
    connect(envSyncJob, &KUpdateLaunchEnvironmentJob::finished, this, []() {
        // The service name is merely there to indicate to the world that we're up and ready with all envs exported
        QDBusConnection::sessionBus().registerService(QStringLiteral("org.kde.NWinWrapper"));
    });
}

void NWinWrapper::terminate(std::chrono::milliseconds timeout)
{
    if (m_nwinProcess) {
        disconnect(m_nwinProcess, nullptr, this, nullptr);
        m_nwinProcess->terminate();
        m_nwinProcess->waitForFinished(timeout.count() / 2);
        if (m_nwinProcess->state() != QProcess::NotRunning) {
            m_nwinProcess->kill();
            m_nwinProcess->waitForFinished(timeout.count() / 2);
        }
    }
}

void NWinWrapper::restart()
{
    terminate(m_watchdogIntervalOk ? std::chrono::duration_cast<std::chrono::milliseconds>(m_watchdogInterval) : 30000ms);
    m_nwinProcess->start();
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    app.setQuitLockEnabled(false); // don't exit when the first KJob finishes

    KSignalHandler::self()->watchSignal(SIGTERM);
    KSignalHandler::self()->watchSignal(SIGHUP);

    NWinWrapper wrapper(&app);
    wrapper.run();

    QObject::connect(KSignalHandler::self(), &KSignalHandler::signalReceived, &app, [&app, &wrapper](int signal) {
        if (signal == SIGTERM) {
            app.quit();
        } else if (signal == SIGHUP) { // The systemd service will issue SIGHUP when it's locked up so that we can restarted
            wrapper.restart();
        }
    });

    return app.exec();
}

#include "nwin_wrapper.moc"
