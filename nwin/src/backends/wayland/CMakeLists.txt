target_sources(nwin PRIVATE
    color_manager.cpp
    wayland_backend.cpp
    wayland_display.cpp
    wayland_egl_backend.cpp
    wayland_layer.cpp
    wayland_logging.cpp
    wayland_output.cpp
    wayland_qpainter_backend.cpp
)

target_link_libraries(nwin PRIVATE Plasma::KWaylandClient Wayland::Client gbm::gbm)
