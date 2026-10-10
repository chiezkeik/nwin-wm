/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import org.kde.nwin as NWinComponents

Item {
    id: desktopView

    required property QtObject windowModel
    required property QtObject desktop

    Repeater {
        model: NWinComponents.WindowFilterModel {
            activity: NWinComponents.Workspace.currentActivity
            desktop: desktopView.desktop
            screenName: targetScreen.name
            windowModel: desktopView.windowModel
        }

        NWinComponents.WindowThumbnail {
            wId: model.window.internalId
            x: model.window.x - targetScreen.geometry.x
            y: model.window.y - targetScreen.geometry.y
            width: model.window.width
            height: model.window.height
            z: model.window.stackingOrder
            visible: !model.window.minimized
        }
    }
}
