add_library(NWinQpaPlugin OBJECT)
target_sources(NWinQpaPlugin PRIVATE
    backingstore.cpp
    clipboard.cpp
    eglhelpers.cpp
    eglplatformcontext.cpp
    integration.cpp
    main.cpp
    offscreensurface.cpp
    platformcursor.cpp
    screen.cpp
    swapchain.cpp
    window.cpp
)

ecm_qt_declare_logging_category(NWinQpaPlugin
    HEADER logging.h
    IDENTIFIER NWIN_QPA
    CATEGORY_NAME nwin_qpa_plugin
    DEFAULT_SEVERITY Critical
)

target_compile_definitions(NWinQpaPlugin PRIVATE QT_STATICPLUGIN)

target_link_libraries(NWinQpaPlugin PRIVATE
    Qt::Concurrent
    Qt::CorePrivate
    Qt::GuiPrivate
    nwin
)
