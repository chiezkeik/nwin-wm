########### next target ###############

set(nwin_killer_helper_SRCS killer.cpp killdialog.cpp killdialog.h)

ki18n_wrap_ui(nwin_killer_helper_SRCS killdialog.ui)

add_executable(nwin_killer_helper ${nwin_killer_helper_SRCS})

target_link_libraries(nwin_killer_helper
    KF6::AuthCore
    KF6::GuiAddons
    KF6::I18n
    KF6::Service
    KF6::WidgetsAddons
    KF6::WindowSystem
    Qt::GuiPrivate
    Qt::Widgets
    Qt::Concurrent
)

ecm_qt_declare_logging_category(nwin_killer_helper
    HEADER debug.h
    IDENTIFIER NWIN_KILLER
    CATEGORY_NAME org.kde.nwin.killer
)

install(TARGETS nwin_killer_helper DESTINATION ${KDE_INSTALL_LIBEXECDIR})

configure_file(org.kde.nwin.killer.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.nwin.killer.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.nwin.killer.desktop DESTINATION ${KDE_INSTALL_APPDIR})
