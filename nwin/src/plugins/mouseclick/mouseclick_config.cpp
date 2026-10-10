/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2012 Filip Wieladek <wattos@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "mouseclick_config.h"

#include "config-nwin.h"

// KConfigSkeleton
#include "mouseclickconfig.h"
#include <nwineffects_interface.h>

#include <QAction>

#include <KActionCollection>
#include <KGlobalAccel>
#include <KLocalizedString>
#include <KPluginFactory>

#include <QWidget>

K_PLUGIN_CLASS(NWin::MouseClickEffectConfig)

namespace NWin
{

MouseClickEffectConfig::MouseClickEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    m_ui.setupUi(widget());

    connect(m_ui.editor, &KShortcutsEditor::keyChange, this, &KCModule::markAsChanged);

    // Shortcut config. The shortcut belongs to the component "nwin"!
    m_actionCollection = new KActionCollection(this, QStringLiteral("nwin"));
    m_actionCollection->setComponentDisplayName(i18n("NWin"));

    QAction *a = m_actionCollection->addAction(QStringLiteral("ToggleMouseClick"));
    a->setText(i18n("Toggle Mouse Click Animation"));
    a->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setGlobalShortcut(a, QKeySequence(Qt::META | Qt::Key_Asterisk));

    m_ui.editor->addCollection(m_actionCollection);

    MouseClickConfig::instance(NWIN_CONFIG);
    addConfig(MouseClickConfig::self(), widget());
}

void MouseClickEffectConfig::save()
{
    KCModule::save();
    m_ui.editor->save(); // undo() will restore to this state from now on
    OrgKdeNWinEffectsInterface interface(QStringLiteral("org.kde.NWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("mouseclick"));
}

} // namespace

#include "mouseclick_config.moc"

#include "moc_mouseclick_config.cpp"
