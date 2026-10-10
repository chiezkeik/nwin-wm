# SPDX-FileCopyrightText: 2023 Niccolò Venerandi <niccolo.venerandi@kde.org>
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

add_executable(nwin-6.0-delete-desktop-switching-shortcuts)
target_sources(nwin-6.0-delete-desktop-switching-shortcuts PRIVATE nwin-6.0-delete-desktop-switching-shortcuts.cpp)
target_link_libraries(nwin-6.0-delete-desktop-switching-shortcuts PRIVATE KF6::GlobalAccel)
install(TARGETS nwin-6.0-delete-desktop-switching-shortcuts DESTINATION ${KDE_INSTALL_LIBDIR}/kconf_update_bin/)

add_executable(nwin-6.0-reset-active-mouse-screen)
target_sources(nwin-6.0-reset-active-mouse-screen PRIVATE nwin-6.0-reset-active-mouse-screen.cpp)
target_link_libraries(nwin-6.0-reset-active-mouse-screen PRIVATE KF6::ConfigCore)
install(TARGETS nwin-6.0-reset-active-mouse-screen DESTINATION ${KDE_INSTALL_LIBDIR}/kconf_update_bin/)

add_executable(nwin-6.0-remove-breeze-tabbox-default)
target_sources(nwin-6.0-remove-breeze-tabbox-default PRIVATE nwin-6.0-remove-breeze-tabbox-default.cpp)
target_link_libraries(nwin-6.0-remove-breeze-tabbox-default PRIVATE KF6::ConfigCore)
install(TARGETS nwin-6.0-remove-breeze-tabbox-default DESTINATION ${KDE_INSTALL_LIBDIR}/kconf_update_bin/)

add_executable(nwin-6.1-remove-gridview-expose-shortcuts)
target_sources(nwin-6.1-remove-gridview-expose-shortcuts PRIVATE nwin-6.1-remove-gridview-expose-shortcuts.cpp)
target_link_libraries(nwin-6.1-remove-gridview-expose-shortcuts PRIVATE KF6::GlobalAccel)
install(TARGETS nwin-6.1-remove-gridview-expose-shortcuts DESTINATION ${KDE_INSTALL_LIBDIR}/kconf_update_bin/)

add_executable(nwin-6.5-showpaint-changes)
target_sources(nwin-6.5-showpaint-changes PRIVATE nwin-6.5-showpaint-changes.cpp)
target_link_libraries(nwin-6.5-showpaint-changes PRIVATE KF6::ConfigCore KF6::GlobalAccel)
install(TARGETS nwin-6.5-showpaint-changes DESTINATION ${KDE_INSTALL_LIBDIR}/kconf_update_bin/)

install(FILES nwin.upd
        DESTINATION ${KDE_INSTALL_KCONFUPDATEDIR})
