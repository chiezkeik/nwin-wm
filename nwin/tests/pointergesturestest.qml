/*
    SPDX-FileCopyrightText: 2016 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
import QtQuick
import org.kde.NWin.tests

Image {
    source: "../48-apps-nwin.png"
    scale: gesture.scale * gesture.progressScale

    PinchGesture {
        id: gesture
    }
}
