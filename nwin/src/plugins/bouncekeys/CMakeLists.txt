# SPDX-FileCopyrightText: 2023 Nicolas Fella <nicolas.fella@gmx.de>
# SPDX-License-Identifier: BSD-3-Clause

kcoreaddons_add_plugin(BounceKeysPlugin INSTALL_NAMESPACE "nwin/plugins")

ecm_qt_declare_logging_category(BounceKeysPlugin
    HEADER bouncekeys_debug.h
    IDENTIFIER NWIN_BOUNCEKEYS
    CATEGORY_NAME nwin_bouncekeys
    DEFAULT_SEVERITY Warning
)

target_sources(BounceKeysPlugin PRIVATE
    main.cpp
    bouncekeys.cpp
)
target_link_libraries(BounceKeysPlugin PRIVATE nwin KF6::WindowSystem)

