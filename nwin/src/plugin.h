/*
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "config-nwin.h"

#include <nwin_export.h>

#include <QObject>
#include <memory>

namespace NWin
{

#define PluginFactory_iid "org.kde.NWin.PluginFactoryInterface" NWIN_PLUGIN_VERSION_STRING

/**
 * The Plugin class is the baseclass for all binary compositor extensions.
 *
 * Note that a binary extension must be recompiled with every new NWin release.
 */
class NWIN_EXPORT Plugin : public QObject
{
    Q_OBJECT

public:
    explicit Plugin();
};

/**
 * The PluginFactory class creates binary compositor extensions.
 */
class NWIN_EXPORT PluginFactory : public QObject
{
    Q_OBJECT

public:
    explicit PluginFactory();

    virtual std::unique_ptr<Plugin> create() const = 0;
};

} // namespace NWin

Q_DECLARE_INTERFACE(NWin::PluginFactory, PluginFactory_iid)
