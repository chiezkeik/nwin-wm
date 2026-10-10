add_subdirectory(lib)

add_library(NWinXwaylandServerModule OBJECT
    clipboard.cpp
    databridge.cpp
    datasource.cpp
    dnd.cpp
    drag.cpp
    drag_wl.cpp
    drag_x.cpp
    primary.cpp
    selection.cpp
    transfer.cpp
    xwayland.cpp
    xwldrophandler.cpp
    xwaylandlauncher.cpp
)
target_link_libraries(NWinXwaylandServerModule PUBLIC nwin Qt::Concurrent KF6::I18n NWinXwaylandCommon XCB::XCB XCB::RANDR XCB::XFIXES XCB::COMPOSITE XKB::XKB)
if(TARGET KF6::Notifications)
    target_link_libraries(NWinXwaylandServerModule PUBLIC KF6::Notifications)
endif()
