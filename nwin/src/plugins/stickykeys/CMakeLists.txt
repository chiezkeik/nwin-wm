# SPDX-FileCopyrightText: 2022 Nicolas Fella <nicolas.fella@gmx.de>
# SPDX-License-Identifier: BSD-3-Clause

kcoreaddons_add_plugin(StickyKeysPlugin INSTALL_NAMESPACE "nwin/plugins")

ecm_qt_declare_logging_category(StickyKeysPlugin
    HEADER stickykeys_debug.h
    IDENTIFIER NWIN_STICKYKEYS
    CATEGORY_NAME nwin_stickykeys
    DEFAULT_SEVERITY Warning
)

target_sources(StickyKeysPlugin PRIVATE
    main.cpp
    stickykeys.cpp
)
target_link_libraries(StickyKeysPlugin PRIVATE nwin KF6::WindowSystem KF6::I18n XKB::XKB)

if (NWIN_BUILD_NOTIFICATIONS)
    target_link_libraries(StickyKeysPlugin PRIVATE KF6::Notifications)
endif()
