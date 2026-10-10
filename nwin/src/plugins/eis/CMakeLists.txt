# SPDX-FileCopyrightText: 2024 David Redondo <kde@david-redono.de>
# SPDX-License-Identifier: BSD-3-Clause

kcoreaddons_add_plugin(eis INSTALL_NAMESPACE "nwin/plugins")

ecm_qt_declare_logging_category(eis
    HEADER libeis_logging.h
    IDENTIFIER NWIN_EIS
    CATEGORY_NAME nwin_libeis
    DEFAULT_SEVERITY Warning
)

ecm_qt_declare_logging_category(eis
    HEADER inputcapture_logging.h
    IDENTIFIER NWIN_INPUTCAPTURE
    CATEGORY_NAME nwin_inputcapture
    DEFAULT_SEVERITY Warning
)

target_sources(eis PRIVATE
    main.cpp
    eisdevice.cpp
    eisbackend.cpp
    eiscontext.cpp
    eisplugin.cpp
    eisinputcapture.cpp
    eisinputcapturemanager.cpp
    eisinputcapturefilter.cpp
)

target_link_libraries(eis PRIVATE nwin KF6::I18n KF6::GlobalAccel KF6::ConfigGui Libeis::Libeis XKB::XKB)

if(NWIN_BUILD_X11)
    target_sources(eis PRIVATE xwaylandeiscontext.cpp)

    add_executable(nwin_eis_prompter eisprompter.cpp)
    kconfig_target_kcfg_file(nwin_eis_prompter
        FILE ${PROJECT_SOURCE_DIR}/src/kcms/xwayland/nwinxwaylandsettings.kcfg
        CLASS_NAME XwaylandSettings
        MUTATORS
        NOTIFIERS
    )
    target_link_libraries(nwin_eis_prompter PRIVATE  KF6::I18n  KF6::ConfigGui KF6::WidgetsAddons KF6::Service)
    install(TARGETS nwin_eis_prompter DESTINATION ${KDE_INSTALL_LIBEXECDIR})

    set(PROMPTER_BIN ${KDE_INSTALL_FULL_LIBEXECDIR}/nwin_eis_prompter)
endif()

if (Libeis_VERSION VERSION_GREATER_EQUAL "1.5")
    set(EIS_HAVE_GET_CLIENT_PID 1)
endif()
configure_file(config-eis.h.in config-eis.h)
