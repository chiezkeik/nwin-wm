/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QDBusArgument>

namespace NWin
{

struct DBusDesktopDataStruct
{
    uint position;
    QString id;
    QString name;
};
typedef QList<DBusDesktopDataStruct> DBusDesktopDataVector;

}

const QDBusArgument &operator<<(QDBusArgument &argument, const NWin::DBusDesktopDataStruct &desk);
const QDBusArgument &operator>>(const QDBusArgument &argument, NWin::DBusDesktopDataStruct &desk);

Q_DECLARE_METATYPE(NWin::DBusDesktopDataStruct)

const QDBusArgument &operator<<(QDBusArgument &argument, const NWin::DBusDesktopDataVector &deskVector);
const QDBusArgument &operator>>(const QDBusArgument &argument, NWin::DBusDesktopDataVector &deskVector);

Q_DECLARE_METATYPE(NWin::DBusDesktopDataVector)
