import QtQuick
import org.kde.nwin

ScreenEdgeHandler {
    edge: ScreenEdgeHandler.LeftEdge
    mode: ScreenEdgeHandler.Touch
    onActivated: {
        Workspace.slotToggleShowDesktop();
    }
}
