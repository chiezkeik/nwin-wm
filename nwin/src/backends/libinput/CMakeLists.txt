qt_generate_dbus_interface(device.h org.kde.NWin.InputDevice.xml OPTIONS -A)

add_custom_target(
    NWinInputDBusInterfaces
    ALL
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/org.kde.NWin.InputDevice.xml
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/org.kde.NWin.InputDevice.xml
    DESTINATION
        ${KDE_INSTALL_DBUSINTERFACEDIR}
)

target_sources(nwin PRIVATE
    connection.cpp
    context.cpp
    device.cpp
    events.cpp
    libinput_logging.cpp
    libinputbackend.cpp
)
target_link_libraries(nwin PRIVATE Libinput::Libinput)
