/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2025 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.plasma.workspace.keyboardlayout as Keyboards
import org.kde.layershell as LayerShell

ApplicationWindow
{
    visible: true
    width: 400
    height: 300
    title: "NWin Virtual Keyboard Control"

    LayerShell.Window.anchors: LayerShell.Window.AnchorTop
    LayerShell.Window.layer: LayerShell.Window.LayerTop


    ColumnLayout {
        anchors {
            fill: parent
            margins: 20
        }

        Button {
            text: "Force Activate"
            onClicked: Keyboards.NWinVirtualKeyboard.forceActivate()
        }

        Button {
            text: Keyboards.NWinVirtualKeyboard.enabled ? "Disable Keyboard" : "Enable Keyboard"
            onClicked: Keyboards.NWinVirtualKeyboard.enabled = !Keyboards.NWinVirtualKeyboard.enabled
        }

        Button {
            enabled: Keyboards.NWinVirtualKeyboard.visible
            icon.source: "go-down-symbolic"
            text: "Hide"

            onClicked: {
                Keyboards.NWinVirtualKeyboard.active = false
            }
        }

        Repeater {
            model: [
                "available",
                "active",
                "visible",
                "activeClientSupportsTextInput"
            ]

            delegate: Label {
                required property string modelData
                text: modelData + ": " + Keyboards.NWinVirtualKeyboard[modelData]
            }
        }
    }
}
