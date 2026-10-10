/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
    SPDX-FileCopyrightText: 2022 ivan tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "windowvieweffectkcm.h"

#include "config-nwin.h"

#include "windowviewconfig.h"

#include <nwineffects_interface.h>

#include <KActionCollection>
#include <KGlobalAccel>
#include <KLocalizedString>
#include <KPluginFactory>

#include <QAction>

K_PLUGIN_CLASS(NWin::WindowViewEffectConfig)

namespace NWin
{

WindowViewEffectConfig::WindowViewEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    ui.setupUi(widget());
    WindowViewConfig::instance(NWIN_CONFIG);
    addConfig(WindowViewConfig::self(), widget());

    auto actionCollection = new KActionCollection(widget(), QStringLiteral("nwin"));

    actionCollection->setComponentDisplayName(i18n("NWin"));
    actionCollection->setConfigGroup(QStringLiteral("windowview"));
    actionCollection->setConfigGlobal(true);

    QAction *toggleAction = actionCollection->addAction(QStringLiteral("Expose"));
    toggleAction->setText(i18n("Toggle Present Windows (Current desktop)"));
    toggleAction->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setGlobalShortcut(toggleAction, QKeySequence(Qt::CTRL | Qt::Key_F9));

    toggleAction = actionCollection->addAction(QStringLiteral("ExposeAll"));
    toggleAction->setText(i18n("Toggle Present Windows (All desktops)"));
    toggleAction->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setGlobalShortcut(toggleAction, QKeySequence(Qt::CTRL | Qt::Key_F10));

    toggleAction = actionCollection->addAction(QStringLiteral("ExposeClass"));
    toggleAction->setText(i18n("Toggle Present Windows (Window class)"));
    toggleAction->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setGlobalShortcut(toggleAction, QKeySequence(Qt::CTRL | Qt::Key_F7));

    toggleAction = actionCollection->addAction(QStringLiteral("ExposeClassCurrentDesktop"));
    toggleAction->setText(i18n("Toggle Present Windows (Window class on current desktop)"));
    toggleAction->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setGlobalShortcut(toggleAction, QList<QKeySequence>()); // no default shortcut

    ui.shortcutsEditor->addCollection(actionCollection);
    connect(ui.shortcutsEditor, &KShortcutsEditor::keyChange, this, &KCModule::markAsChanged);
}

void WindowViewEffectConfig::save()
{
    KCModule::save();
    ui.shortcutsEditor->save();

    OrgKdeNWinEffectsInterface interface(QStringLiteral("org.kde.NWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("windowview"));
}

void WindowViewEffectConfig::defaults()
{
    ui.shortcutsEditor->allDefault();
    KCModule::defaults();
}

} // namespace NWin

#include "windowvieweffectkcm.moc"

#include "moc_windowvieweffectkcm.cpp"
