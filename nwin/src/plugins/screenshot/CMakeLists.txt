set(screenshot_SOURCES
    main.cpp
    screenshot.cpp
    screenshotdbusinterface2.cpp
    screenshotlayer.cpp
)

qt_add_dbus_adaptor(screenshot_SOURCES org.kde.NWin.ScreenShot2.xml screenshotdbusinterface2.h NWin::ScreenShotDBusInterface2)

kcoreaddons_add_plugin(screenshot SOURCES ${screenshot_SOURCES} INSTALL_NAMESPACE "nwin/plugins")
target_link_libraries(screenshot PRIVATE
    nwin

    KF6::Service
    KF6::I18n

    Qt::DBus
)

ecm_qt_declare_logging_category(screenshot
    HEADER screenshotlogging.h
    IDENTIFIER NWIN_SCREENSHOT
    CATEGORY_NAME nwin_screenshot
    DEFAULT_SEVERITY Warning
)
