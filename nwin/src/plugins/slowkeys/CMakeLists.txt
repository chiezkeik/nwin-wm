# SPDX-FileCopyrightText: 2025 Martin Riethmayer <ripper@freakmail.de>
# SPDX-License-Identifier: BSD-3-Clause

kcoreaddons_add_plugin(SlowKeysPlugin INSTALL_NAMESPACE "nwin/plugins")

ecm_qt_declare_logging_category(SlowKeysPlugin
    HEADER slowkeys_debug.h
    IDENTIFIER NWIN_SLOWKEYS
    CATEGORY_NAME nwin_slowkeys
    DEFAULT_SEVERITY Warning
)

target_sources(SlowKeysPlugin PRIVATE
    main.cpp
    slowkeys.cpp
)
target_link_libraries(SlowKeysPlugin PRIVATE nwin KF6::WindowSystem)

