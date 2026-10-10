add_executable(nwin_wayland_wrapper)
target_sources(nwin_wayland_wrapper PRIVATE
    nwin_wrapper.cpp
    wl-socket.c
)

ecm_qt_declare_logging_category(nwin_wayland_wrapper
    HEADER
        wrapper_logging.h
    IDENTIFIER
        NWIN_WRAPPER
    CATEGORY_NAME
        nwin_wayland_wrapper
    DEFAULT_SEVERITY
        Warning
)

target_link_libraries(nwin_wayland_wrapper Qt::Core Qt::DBus KF6::DBusAddons KF6::CoreAddons)
if (NWIN_BUILD_X11)
    target_link_libraries(nwin_wayland_wrapper NWinXwaylandCommon)
endif()
set_property(TARGET nwin_wayland_wrapper PROPERTY C_STANDARD 11)
install(TARGETS nwin_wayland_wrapper ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
