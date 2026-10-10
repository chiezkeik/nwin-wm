# SPDX-FileCopyrightText: None
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required(VERSION 3.20)
project(quick-effect)

set(KF6_MIN_VERSION "6.0.0")

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt6 CONFIG REQUIRED COMPONENTS
    Core
    Widgets
)

find_package(NWin REQUIRED COMPONENTS
    nwin
)

kcoreaddons_add_plugin(eventlistener INSTALL_NAMESPACE "nwin/plugins")
target_sources(eventlistener PRIVATE
    main.cpp
    eventlistener.cpp
)

target_link_libraries(eventlistener PRIVATE
    NWin::nwin
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
