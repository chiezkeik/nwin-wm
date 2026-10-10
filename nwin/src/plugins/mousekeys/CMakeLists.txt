# SPDX-FileCopyrightText: 2022 Nicolas Fella <nicolas.fella@gmx.de>
# SPDX-License-Identifier: BSD-3-Clause

kcoreaddons_add_plugin(MouseKeysPlugin INSTALL_NAMESPACE "nwin/plugins")

ecm_qt_declare_logging_category(MouseKeysPlugin
    HEADER mousekeys_debug.h
    IDENTIFIER NWIN_MOUSEKEYS
    CATEGORY_NAME nwin_mousekeys
    DEFAULT_SEVERITY Warning
)

target_sources(MouseKeysPlugin PRIVATE
    main.cpp
    mousekeys.cpp
)
target_link_libraries(MouseKeysPlugin PRIVATE nwin)

