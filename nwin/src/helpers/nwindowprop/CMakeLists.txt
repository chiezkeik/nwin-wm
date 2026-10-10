# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2025 David Redondo <kde@david-redondo.de>

add_executable(nwindowprop main.cpp)
target_link_libraries(nwindowprop Qt::Core Qt::DBus)
install(TARGETS nwindowprop ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

