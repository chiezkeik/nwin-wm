/*
    SPDX-FileCopyrightText: 2022 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "tileseditoreffectkcm.h"

#include "config-nwin.h"

#include <nwineffects_interface.h>

#include <KActionCollection>
#include <KGlobalAccel>
#include <KLocalizedString>
#include <KPluginFactory>

#include <QAction>

K_PLUGIN_CLASS(NWin::TilesEditorEffectConfig)

namespace NWin
{

TilesEditorEffectConfig::TilesEditorEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    ui.setupUi(widget());

    auto actionCollection = new KActionCollection(this, QStringLiteral("nwin"));

    actionCollection->setComponentDisplayName(i18n("NWin"));
    actionCollection->setConfigGroup(QStringLiteral("tileseditor"));
    actionCollection->setConfigGlobal(true);

    QAction *toggleAction = actionCollection->addAction(QStringLiteral("Edit Tiles"));
    toggleAction->setText(i18n("Toggle Tiles Editor"));
    toggleAction->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setGlobalShortcut(toggleAction, QKeySequence(Qt::META | Qt::Key_T));

    ui.shortcutsEditor->addCollection(actionCollection);
    connect(ui.shortcutsEditor, &KShortcutsEditor::keyChange, this, &KCModule::markAsChanged);
}

void TilesEditorEffectConfig::save()
{
    KCModule::save();
    ui.shortcutsEditor->save();

    OrgKdeNWinEffectsInterface interface(QStringLiteral("org.kde.NWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("tileseditor"));
}

void TilesEditorEffectConfig::defaults()
{
    ui.shortcutsEditor->allDefault();
    KCModule::defaults();
}

} // namespace NWin

#include "tileseditoreffectkcm.moc"

#include "moc_tileseditoreffectkcm.cpp"
