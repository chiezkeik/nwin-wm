add_library(NWinXwaylandCommon STATIC
    xwaylandsocket.cpp
    xauthority.cpp
)

ecm_qt_declare_logging_category(NWinXwaylandCommon
    HEADER
        xwayland_logging.h
    IDENTIFIER
        NWIN_XWL
    CATEGORY_NAME
        nwin_xwl
    DEFAULT_SEVERITY
        Warning
)

target_include_directories(NWinXwaylandCommon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(NWinXwaylandCommon Qt::Core)
