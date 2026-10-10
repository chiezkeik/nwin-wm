/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2007 Christian Nitschkowski <christian.nitschkowski@kdemail.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "thumbnailaside_config.h"

#include "config-nwin.h"

// KConfigSkeleton
#include "thumbnailasideconfig.h"
#include <nwineffects_interface.h>

#include <QAction>

#include <KActionCollection>
#include <KGlobalAccel>
#include <KLocalizedString>
#include <KPluginFactory>
#include <kconfiggroup.h>

#include <QVBoxLayout>
#include <QWidget>

K_PLUGIN_CLASS(NWin::ThumbnailAsideEffectConfig)

namespace NWin
{

ThumbnailAsideEffectConfig::ThumbnailAsideEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    m_ui.setupUi(widget());

    connect(m_ui.editor, &KShortcutsEditor::keyChange, this, &KCModule::markAsChanged);

    ThumbnailAsideConfig::instance(NWIN_CONFIG);
    addConfig(ThumbnailAsideConfig::self(), widget());

    // Shortcut config. The shortcut belongs to the component "nwin"!
    m_actionCollection = new KActionCollection(this, QStringLiteral("nwin"));

    m_actionCollection->setComponentDisplayName(i18n("NWin"));
    m_actionCollection->setConfigGroup(QStringLiteral("ThumbnailAside"));
    m_actionCollection->setConfigGlobal(true);

    QAction *a = m_actionCollection->addAction(QStringLiteral("ToggleCurrentThumbnail"));
    a->setText(i18n("Toggle Thumbnail for Current Window"));
    a->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setGlobalShortcut(a, QKeySequence(Qt::META | Qt::CTRL | Qt::Key_T));

    m_ui.editor->addCollection(m_actionCollection);
}

void ThumbnailAsideEffectConfig::save()
{
    KCModule::save();
    m_ui.editor->save();
    OrgKdeNWinEffectsInterface interface(QStringLiteral("org.kde.NWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("thumbnailaside"));
}

} // namespace

#include "thumbnailaside_config.moc"

#include "moc_thumbnailaside_config.cpp"
