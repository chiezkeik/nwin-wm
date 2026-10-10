# SPDX-FileCopyrightText: 2024 Nicolas Fella <nicolas.fella@kdab.com>
# SPDX-License-Identifier: BSD-3-Clause

kcoreaddons_add_plugin(KeyNotificationPlugin INSTALL_NAMESPACE "nwin/plugins")

target_compile_definitions(KeyNotificationPlugin PRIVATE
    -DTRANSLATION_DOMAIN=\"nwin\"
)

target_sources(KeyNotificationPlugin PRIVATE
    main.cpp
    keynotification.cpp
)
target_link_libraries(KeyNotificationPlugin PRIVATE nwin KF6::Notifications KF6::I18n XKB::XKB)

