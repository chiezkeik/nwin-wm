/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2007 Christian Nitschkowski <christian.nitschkowski@kdemail.net>
    SPDX-FileCopyrightText: 2023 Andrew Shark <ashark at linuxcomp.ru>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "mousemark_config.h"

#include "config-nwin.h"

// KConfigSkeleton
#include "mousemarkconfig.h"
#include <nwineffects_interface.h>

#include <QAction>

#include <KActionCollection>
#include <KGlobalAccel>
#include <KLocalizedString>
#include <KPluginFactory>

#include <QDebug>
#include <QWidget>

K_PLUGIN_CLASS(NWin::MouseMarkEffectConfig)

namespace NWin
{

MouseMarkEffectConfig::MouseMarkEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    m_ui.setupUi(widget());

    MouseMarkConfig::instance(NWIN_CONFIG);
    addConfig(MouseMarkConfig::self(), widget());

    // Shortcut config. The shortcut belongs to the component "nwin"!
    m_actionCollection = new KActionCollection(this, QStringLiteral("nwin"));
    m_actionCollection->setComponentDisplayName(i18n("NWin"));

    QAction *a = m_actionCollection->addAction(QStringLiteral("ClearMouseMarks"));
    a->setText(i18n("Clear Mouse Marks"));
    a->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setGlobalShortcut(a, QKeySequence(Qt::SHIFT | Qt::META | Qt::Key_F11));

    a = m_actionCollection->addAction(QStringLiteral("ClearLastMouseMark"));
    a->setText(i18n("Clear Last Mouse Mark"));
    a->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setGlobalShortcut(a, QKeySequence(Qt::SHIFT | Qt::META | Qt::Key_F12));

    m_ui.editor->addCollection(m_actionCollection);
    connect(m_ui.editor, &KShortcutsEditor::keyChange, this, &MouseMarkEffectConfig::markAsChanged);

    connect(m_ui.kcfg_LineWidth, qOverload<int>(&QSpinBox::valueChanged), this, [this]() {
        updateSpinBoxSuffix();
    });
}

void MouseMarkEffectConfig::load()
{
    KCModule::load();

    updateSpinBoxSuffix();
}

void MouseMarkEffectConfig::save()
{
    qDebug() << "Saving config of MouseMark";
    KCModule::save();

    m_actionCollection->writeSettings();
    m_ui.editor->save(); // undo() will restore to this state from now on

    OrgKdeNWinEffectsInterface interface(QStringLiteral("org.kde.NWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("mousemark"));
}

void MouseMarkEffectConfig::updateSpinBoxSuffix()
{
    m_ui.kcfg_LineWidth->setSuffix(i18ncp("Suffix", " pixel", " pixels", m_ui.kcfg_LineWidth->value()));
}

} // namespace

#include "mousemark_config.moc"

#include "moc_mousemark_config.cpp"
