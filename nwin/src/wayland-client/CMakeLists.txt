target_sources(nwin PRIVATE
    linuxdmabuf.cpp
    viewporter.cpp
)
ecm_qt_declare_logging_category(nwin
    HEADER waylandclientlogging.h
    IDENTIFIER NWIN_WAYLAND_CLIENT
    CATEGORY_NAME nwin_wayland_client
)
