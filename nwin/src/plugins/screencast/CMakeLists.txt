kcoreaddons_add_plugin(screencast INSTALL_NAMESPACE "nwin/plugins")
target_sources(screencast PRIVATE
    filteredsceneview.cpp
    main.cpp
    outputscreencastsource.cpp
    pipewirecore.cpp
    regionscreencastsource.cpp
    screencastbuffer.cpp
    screencastlayer.cpp
    screencastmanager.cpp
    screencastsource.cpp
    screencaststream.cpp
    windowscreencastsource.cpp
)

ecm_qt_declare_logging_category(screencast
    HEADER nwinscreencast_logging.h
    IDENTIFIER NWIN_SCREENCAST
    CATEGORY_NAME nwin_screencast
    DEFAULT_SEVERITY Warning
)

target_link_libraries(screencast nwin KF6::I18n PkgConfig::PipeWire Libdrm::Libdrm)
