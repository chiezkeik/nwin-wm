/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "overvieweffectkcm.h"

#include "config-nwin.h"

#include "overviewconfig.h"

#include <nwineffects_interface.h>

#include <KActionCollection>
#include <KGlobalAccel>
#include <KLocalizedString>
#include <KPluginFactory>

#include <QAction>

K_PLUGIN_CLASS(NWin::OverviewEffectConfig)

namespace NWin
{

OverviewEffectConfig::OverviewEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    ui.setupUi(widget());
    OverviewConfig::instance(NWIN_CONFIG);
    addConfig(OverviewConfig::self(), widget());

    auto actionCollection = new KActionCollection(this, QStringLiteral("nwin"));

    actionCollection->setComponentDisplayName(i18n("NWin"));
    actionCollection->setConfigGroup(QStringLiteral("Overview"));
    actionCollection->setConfigGlobal(true);

    QAction *cycleAction = actionCollection->addAction(QStringLiteral("Cycle Overview"));
    cycleAction->setText(i18nc("@action Overview and Grid View are the name of NWin effects", "Cycle through Overview and Grid View"));
    cycleAction->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setGlobalShortcut(cycleAction, QList<QKeySequence>());

    QAction *reverseCycleAction = actionCollection->addAction(QStringLiteral("Cycle Overview Opposite"));
    reverseCycleAction->setText(i18nc("@action Grid View and Overview are the name of NWin effects", "Cycle through Grid View and Overview"));
    reverseCycleAction->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setGlobalShortcut(reverseCycleAction, QList<QKeySequence>());

    QAction *overviewAction = actionCollection->addAction(QStringLiteral("Overview"));
    overviewAction->setText(i18nc("@action Overview is the name of a NWin effect", "Toggle Overview"));
    overviewAction->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setGlobalShortcut(overviewAction, QKeySequence(Qt::META | Qt::Key_W));

    QAction *gridAction = actionCollection->addAction(QStringLiteral("Grid View"));
    gridAction->setText(i18nc("@action Grid View is the name of a NWin effect", "Toggle Grid View"));
    gridAction->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setGlobalShortcut(gridAction, QKeySequence(Qt::META | Qt::Key_G));

    ui.shortcutsEditor->addCollection(actionCollection);
    connect(ui.shortcutsEditor, &KShortcutsEditor::keyChange, this, &KCModule::markAsChanged);
}

void OverviewEffectConfig::save()
{
    KCModule::save();
    ui.shortcutsEditor->save();

    OrgKdeNWinEffectsInterface interface(QStringLiteral("org.kde.NWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("overview"));
}

void OverviewEffectConfig::defaults()
{
    ui.shortcutsEditor->allDefault();
    KCModule::defaults();
}

} // namespace NWin

#include "overvieweffectkcm.moc"

#include "moc_overvieweffectkcm.cpp"
