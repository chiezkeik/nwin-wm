/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "useractionprompt.h"

#include "wayland/xdgforeign_v2.h"
#include "wayland_server.h"
#include "xdgshellwindow.h"
#if NWIN_BUILD_X11
#include "x11window.h"
#endif

#include <QDir>
#include <QFileInfo>
#include <QString>

namespace NWin
{

UserActionPrompt::UserActionPrompt(Window *window, Prompt prompt, QObject *parent)
    : QObject(parent)
    , m_window(window)
    , m_prompt(prompt)
{
    m_process.setProcessChannelMode(QProcess::ForwardedChannels);
    connect(&m_process, &QProcess::finished, this, &UserActionPrompt::onProcessFinished);
}

UserActionPrompt::~UserActionPrompt()
{
    disconnect(&m_process, &QProcess::finished, this, &UserActionPrompt::onProcessFinished);
}

UserActionPrompt::Prompt UserActionPrompt::prompt() const
{
    return m_prompt;
}

bool UserActionPrompt::isRunning() const
{
    return m_process.state() == QProcess::Running;
}

bool UserActionPrompt::shouldShow()
{
    KConfig cfg(QStringLiteral("nwin_dialogsrc"));
    KConfigGroup dontAgainGroup(&cfg, QStringLiteral("Notification Messages"));
    const QString dontAgainKey = QStringLiteral("altf3warning");
    return !dontAgainGroup.readEntry(dontAgainKey, false);
}

void UserActionPrompt::start()
{
    if (isRunning()) {
        return;
    }

    const QFileInfo binaryInfo(NWIN_DIALOG_BIN);
    const QFileInfo buildDirBinary{QDir{QCoreApplication::applicationDirPath()}, binaryInfo.fileName()};

    if (buildDirBinary.exists()) {
        m_process.setProgram(buildDirBinary.absoluteFilePath());
    } else {
        m_process.setProgram(NWIN_DIALOG_BIN);
    }

    QProcessEnvironment env = nwinApp()->processStartupEnvironment();

    QString wid;
    QString timestampString;
    QString platform;

#if NWIN_BUILD_X11
    if (auto *x11Window = qobject_cast<X11Window *>(m_window)) {
        platform = QStringLiteral("xcb");
        wid = QString::number(x11Window->window());
        timestampString = QString::number(nwinApp()->x11Time());
    } else
#endif
        if (auto *xdgToplevel = qobject_cast<XdgToplevelWindow *>(m_window)) {
        platform = QStringLiteral("wayland");
        auto *exported = waylandServer()->exportAsForeign(xdgToplevel->surface());
        wid = exported->handle();

        env.remove(QStringLiteral("QT_WAYLAND_RECONNECT"));
    }

    QStringList args{
        QStringLiteral("-platform"),
        platform,
        QStringLiteral("--wid"),
        wid,
        QStringLiteral("--timestamp"),
        timestampString,
    };

    args.append(QStringLiteral("--message"));
    switch (m_prompt) {
    case Prompt::NoBorder:
        args.append(QStringLiteral("noborder"));
        break;
    case Prompt::FullScreen:
        args.append(QStringLiteral("fullscreen"));
        break;
    }

    m_process.setArguments(args);
    m_process.setProcessEnvironment(env);
    m_process.start();
}

void UserActionPrompt::quit()
{
    m_process.terminate();
}

void UserActionPrompt::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    if (status == QProcess::NormalExit && exitCode == 2) {
        Q_EMIT undoRequested();
    }
    Q_EMIT finished();
}

} // namespace NWin
