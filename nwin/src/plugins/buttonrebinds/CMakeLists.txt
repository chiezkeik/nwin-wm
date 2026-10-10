# SPDX-FileCopyrightText: 2022 David Redondo <kde@david-redono.de>
# SPDX-License-Identifier: BSD-3-Clause

kcoreaddons_add_plugin(buttonsrebind INSTALL_NAMESPACE "nwin/plugins")

ecm_qt_declare_logging_category(buttonsrebind
    HEADER buttonrebinds_debug.h
    IDENTIFIER NWIN_BUTTONREBINDS
    CATEGORY_NAME nwin_buttonrebinds
    DEFAULT_SEVERITY Warning
)

target_sources(buttonsrebind PRIVATE
    main.cpp
    buttonrebindsfilter.cpp
)
target_link_libraries(buttonsrebind PRIVATE nwin Qt::GuiPrivate XKB::XKB)
