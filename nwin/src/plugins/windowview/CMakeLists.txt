# SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

if (NWIN_BUILD_KCMS)
    add_subdirectory(kcm)
endif()

set(windowview_SOURCES
    main.cpp
    windowvieweffect.cpp
)

kconfig_add_kcfg_files(windowview_SOURCES
    windowviewconfig.kcfgc
)

qt_add_dbus_adaptor(windowview_SOURCES org.kde.NWin.Effect.WindowView1.xml windowvieweffect.h NWin::WindowViewEffect)

nwin_add_builtin_effect(windowview ${windowview_SOURCES})

ecm_add_qml_module(windowview
    URI "org.kde.nwin.windowview"
    QML_FILES
        qml/Main.qml
    QT_NO_PLUGIN
)

target_link_libraries(windowview PRIVATE
    nwin

    KF6::ConfigGui
    KF6::GlobalAccel
    KF6::I18n

    Qt::Quick
    )
