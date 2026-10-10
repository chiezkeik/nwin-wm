/*
    SPDX-FileCopyrightText: 2022 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import org.kde.nwin as NWinComponents
import org.kde.nwin.private.effects
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents

Item {
    id: delegate
    required property NWinComponents.Tile tile

    x: Math.round(tile.absoluteGeometryInScreen.x)
    y: Math.round(tile.absoluteGeometryInScreen.y)
    z: focus ? 1000 : 0
    //onZChanged: print(delegate + " "+z)
    width: Math.round(tile.absoluteGeometryInScreen.width)
    height: Math.round(tile.absoluteGeometryInScreen.height)
    Connections {
        target: tile
        function onTilesChanged() {
            if (tile.layoutDirection === NWinComponents.Tile.Floating && tile.tiles.length === 0) {
                tile.layoutDirection = tile.parent.layoutDirection;
            }
        }
    }
    ResizeHandle {
        anchors {
            horizontalCenter: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        tile: delegate.tile
        edge: Qt.LeftEdge
    }
    ResizeHandle {
        anchors {
            verticalCenter: parent.top
            left: parent.left
            right: parent.right
        }
        tile: delegate.tile
        edge: Qt.TopEdge
    }
    ResizeHandle {
        anchors {
            horizontalCenter: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        tile: delegate.tile
        edge: Qt.RightEdge
        visible: tile.layoutDirection === NWinComponents.Tile.Floating
    }
    ResizeHandle {
        anchors {
            verticalCenter: parent.bottom
            left: parent.left
            right: parent.right
        }
        tile: delegate.tile
        edge: Qt.BottomEdge
        visible: tile.layoutDirection === NWinComponents.Tile.Floating
    }

    ResizeCorner {
        anchors {
            top: parent.top
            left: parent.left
        }
        tile: delegate.tile
        corner: Qt.TopLeftCorner
    }
    ResizeCorner {
        anchors {
            top: parent.top
            right: parent.right
        }
        tile: delegate.tile
        corner: Qt.TopRightCorner
    }
    ResizeCorner {
        anchors {
            left: parent.left
            bottom: parent.bottom
        }
        tile: delegate.tile
        corner: Qt.BottomLeftCorner
    }
    ResizeCorner {
        anchors {
            right: parent.right
            bottom: parent.bottom
        }
        tile: delegate.tile
        corner: Qt.BottomRightCorner
    }

    Item {
        anchors.fill: parent

        Rectangle {
            anchors {
                fill: parent
                margins: Kirigami.Units.smallSpacing
            }
            visible: tile.tiles.length === 0
            radius: Kirigami.Units.cornerRadius
            opacity: tile.layoutDirection === NWinComponents.Tile.Floating ? 0.6 : 0.3
            color: tile.layoutDirection === NWinComponents.Tile.Floating ? Kirigami.Theme.backgroundColor : "transparent"
            border.color: Kirigami.Theme.textColor
            Rectangle {
                anchors {
                    fill: parent
                    margins: 1
                }
                radius: Kirigami.Units.cornerRadius
                color: "transparent"
                border.color: Kirigami.Theme.backgroundColor
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                propagateComposedEvents: true
                property point lastPos
                onClicked: {
                    delegate.focus = true;
                    if (tile.layoutDirection !== NWinComponents.Tile.Floating) {
                        mouse.accepted = false;
                    }
                }
                onPressed: {
                    lastPos = mapToItem(null, mouse.x, mouse.y);
                }
                onPositionChanged: {
                    if (!pressed || tile.layoutDirection !== NWinComponents.Tile.Floating) {
                        return;
                    }
                    let pos = mapToItem(null, mouse.x, mouse.y);
                    let deltaPoint = Qt.point(pos.x - lastPos.x, pos.y - lastPos.y);
                    deltaPoint.x = Math.max(deltaPoint.x, tile.parent.absoluteGeometryInScreen.x - tile.absoluteGeometryInScreen.x);
                    deltaPoint.x = Math.min(deltaPoint.x, (tile.parent.absoluteGeometryInScreen.x + tile.parent.absoluteGeometryInScreen.width) - (tile.absoluteGeometryInScreen.x + tile.absoluteGeometryInScreen.width));

                    deltaPoint.y = Math.max(deltaPoint.y, tile.parent.absoluteGeometryInScreen.y - tile.absoluteGeometryInScreen.y);
                    deltaPoint.y = Math.min(deltaPoint.y, (tile.parent.absoluteGeometryInScreen.y + tile.parent.absoluteGeometryInScreen.height) - (tile.absoluteGeometryInScreen.y + tile.absoluteGeometryInScreen.height));

                    tile.moveByPixels(deltaPoint);
                    lastPos = pos;
                }
            }
        }
        GridLayout {
            anchors.centerIn: parent
            visible: tile.tiles.length === 0
            readonly property bool compact: delegate.width < Kirigami.Units.gridUnit * 10 || delegate.height < splitButton.implicitHeight * visibleChildren.length + rowSpacing * visibleChildren.length + Kirigami.Units.gridUnit
            rows: compact ? 1 : -1
            columns: compact ? -1 : 1
            PlasmaComponents.Button {
                id: splitButton
                Layout.fillWidth: true
                icon.name: "view-split-left-right"
                text: i18nd("nwin","Split Left/Right")
                display: parent.compact ? PlasmaComponents.Button.IconOnly : PlasmaComponents.Button.TextBesideIcon
                onClicked: tile.split(NWinComponents.Tile.Horizontal)
            }
            PlasmaComponents.Button {
                Layout.fillWidth: true
                icon.name: "view-split-top-bottom"
                text: i18nd("nwin","Split Top/Bottom")
                display: parent.compact ? PlasmaComponents.Button.IconOnly : PlasmaComponents.Button.TextBesideIcon
                onClicked: tile.split(NWinComponents.Tile.Vertical)
            }
            PlasmaComponents.Button {
                Layout.fillWidth: true
                visible: tile.layoutDirection !== NWinComponents.Tile.Floating
                icon.name: "window-duplicate"
                text: i18nd("nwin","Add Floating Tile")
                display: parent.compact ? PlasmaComponents.Button.IconOnly : PlasmaComponents.Button.TextBesideIcon
                onClicked: tile.split(NWinComponents.Tile.Floating)
            }
            PlasmaComponents.Button {
                id: deleteButton
                visible: tile.canBeRemoved
                Layout.fillWidth: true
                icon.name: "edit-delete"
                text: i18nd("nwin","Delete")
                display: parent.compact ? PlasmaComponents.Button.IconOnly : PlasmaComponents.Button.TextBesideIcon
                onClicked: {
                    tile.remove();
                }
            }
        }
    }
    TapHandler {
        enabled: tile.layoutDirection === NWinComponents.Tile.Floating && tile.isLayout
        onTapped: effect.deactivate(effect.animationDuration);
    }
    PlasmaComponents.Button {
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: Kirigami.Units.smallSpacing
        }
        visible: tile.layoutDirection === NWinComponents.Tile.Floating && tile.isLayout
        icon.name: "window-duplicate"
        text: i18nd("nwin","Add Floating Tile")
        onClicked: tile.split(NWinComponents.Tile.Floating)
    }
}
