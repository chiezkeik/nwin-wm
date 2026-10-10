/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// own
#include "virtualdesktopsdbustypes.h"

// Marshall the DBusDesktopDataStruct data into a D-BUS argument
const QDBusArgument &operator<<(QDBusArgument &argument, const NWin::DBusDesktopDataStruct &desk)
{
    argument.beginStructure();
    argument << desk.position;
    argument << desk.id;
    argument << desk.name;
    argument.endStructure();
    return argument;
}
// Retrieve
const QDBusArgument &operator>>(const QDBusArgument &argument, NWin::DBusDesktopDataStruct &desk)
{
    argument.beginStructure();
    argument >> desk.position;
    argument >> desk.id;
    argument >> desk.name;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator<<(QDBusArgument &argument, const NWin::DBusDesktopDataVector &deskVector)
{
    argument.beginArray(qMetaTypeId<NWin::DBusDesktopDataStruct>());
    for (int i = 0; i < deskVector.size(); ++i) {
        argument << deskVector[i];
    }
    argument.endArray();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NWin::DBusDesktopDataVector &deskVector)
{
    argument.beginArray();
    deskVector.clear();

    while (!argument.atEnd()) {
        NWin::DBusDesktopDataStruct element;
        argument >> element;
        deskVector.append(element);
    }

    argument.endArray();

    return argument;
}
