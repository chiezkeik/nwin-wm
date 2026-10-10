# SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
# SPDX-License-Identifier: BSD-3-Clause

add_executable(nwin_dialog_helper dialoghelper.cpp)

target_link_libraries(nwin_dialog_helper
    KF6::ConfigCore
    KF6::GuiAddons
    KF6::GlobalAccel
    KF6::I18n
    KF6::WidgetsAddons
    KF6::WindowSystem
    Qt::GuiPrivate
    Qt::Widgets
)

install(TARGETS nwin_dialog_helper DESTINATION ${KDE_INSTALL_LIBEXECDIR})

configure_file(org.kde.nwin.dialoghelper.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.nwin.dialoghelper.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.nwin.dialoghelper.desktop DESTINATION ${KDE_INSTALL_APPDIR})
