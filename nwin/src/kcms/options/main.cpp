/*
    SPDX-FileCopyrightText: 2001 Waldo Bastian <bastian@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "main.h"

#include <QLayout>
// Added by qt3to4:
#include <QVBoxLayout>

#include <QtDBus>

#include <KLocalizedString>
#include <KPluginFactory>
#include <kconfig.h>

#include "nwinoptions_kdeglobals_settings.h"
#include "nwinoptions_settings.h"
#include "nwinoptionsdata.h"
#include "mouse.h"
#include "windows.h"

K_PLUGIN_CLASS_WITH_JSON(NWinOptions, "kcm_nwinoptions.json")

NWinOptions::NWinOptions(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    mSettings = new NWinOptionsSettings(this);

    QVBoxLayout *layout = new QVBoxLayout(widget());
    layout->setContentsMargins(0, 0, 0, 0);
    tab = new QTabWidget(widget());
    tab->setDocumentMode(true);
    tab->tabBar()->setExpanding(true);
    layout->addWidget(tab);

    const auto connectKCM = [this](KCModule *mod) {
        connect(mod, &KCModule::needsSaveChanged, this, &NWinOptions::updateUnmanagedState);
        connect(this, &KCModule::defaultsIndicatorsVisibleChanged, mod, [mod, this]() {
            mod->setDefaultsIndicatorsVisible(defaultsIndicatorsVisible());
        });
    };

    mFocus = new KFocusConfig(mSettings, widget());
    mFocus->setObjectName(QLatin1StringView("NWin Focus Config"));
    tab->addTab(mFocus->widget(), i18n("&Focus"));
    connectKCM(mFocus);

    mTitleBarActions = new KTitleBarActionsConfig(mSettings, widget());
    mTitleBarActions->setObjectName(QLatin1StringView("NWin TitleBar Actions"));
    tab->addTab(mTitleBarActions->widget(), i18n("Titlebar A&ctions"));
    connectKCM(mTitleBarActions);

    mWindowActions = new KWindowActionsConfig(mSettings, widget());
    mWindowActions->setObjectName(QLatin1StringView("NWin Window Actions"));
    tab->addTab(mWindowActions->widget(), i18n("W&indow Actions"));
    connectKCM(mWindowActions);

    mMoving = new KMovingConfig(mSettings, widget());
    mMoving->setObjectName(QLatin1StringView("NWin Moving"));
    tab->addTab(mMoving->widget(), i18n("Mo&vement"));
    connectKCM(mMoving);

    mAdvanced = new KAdvancedConfig(mSettings, new NWinOptionsKDEGlobalsSettings(this), widget());
    mAdvanced->setObjectName(QLatin1StringView("NWin Advanced"));
    tab->addTab(mAdvanced->widget(), i18n("Adva&nced"));
    connectKCM(mAdvanced);

    if (qEnvironmentVariableIntValue("NWIN_WAYLAND_SUPPORT_XX_PIP_V1")) {
        mPip = new KPipConfig(mSettings, widget());
        mPip->setObjectName(QLatin1StringView("NWin Picture-in-Picture"));
        tab->addTab(mPip->widget(), i18n("&Picture-in-picture"));
        connectKCM(mPip);
    }
}

void NWinOptions::load()
{
    KCModule::load();

    mTitleBarActions->load();
    mWindowActions->load();
    mMoving->load();
    mAdvanced->load();
    // mFocus is last because it may send unmanagedWidgetStateChanged
    // that need to have the final word
    mFocus->load();
    if (mPip) {
        mPip->load();
    }
}

void NWinOptions::save()
{
    KCModule::save();

    mFocus->save();
    mTitleBarActions->save();
    mWindowActions->save();
    mMoving->save();
    mAdvanced->save();
    if (mPip) {
        mPip->save();
    }

    // Send signal to all nwin instances
    QDBusMessage message =
        QDBusMessage::createSignal("/NWin", "org.kde.NWin", "reloadConfig");
    QDBusConnection::sessionBus().send(message);
}

void NWinOptions::defaults()
{
    KCModule::defaults();

    mTitleBarActions->defaults();
    mWindowActions->defaults();
    mMoving->defaults();
    mAdvanced->defaults();
    // mFocus is last because it may send unmanagedWidgetDefaulted
    // that need to have the final word
    mFocus->defaults();
    if (mPip) {
        mPip->defaults();
    }
}

void NWinOptions::updateUnmanagedState()
{
    bool changed = false;
    changed |= mFocus->needsSave();
    changed |= mTitleBarActions->needsSave();
    changed |= mWindowActions->needsSave();
    changed |= mMoving->needsSave();
    changed |= mAdvanced->needsSave();
    changed |= mPip ? mPip->needsSave() : false;

    unmanagedWidgetChangeState(changed);

    bool isDefault = true;
    isDefault &= mFocus->representsDefaults();
    isDefault &= mTitleBarActions->representsDefaults();
    isDefault &= mWindowActions->representsDefaults();
    isDefault &= mMoving->representsDefaults();
    isDefault &= mAdvanced->representsDefaults();
    isDefault &= mPip ? mPip->representsDefaults() : true;

    unmanagedWidgetDefaultState(isDefault);
}

KActionsOptions::KActionsOptions(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    mSettings = new NWinOptionsSettings(this);

    QVBoxLayout *layout = new QVBoxLayout(widget());
    layout->setContentsMargins(0, 0, 0, 0);
    tab = new QTabWidget(widget());
    layout->addWidget(tab);

    mTitleBarActions = new KTitleBarActionsConfig(mSettings, widget());
    mTitleBarActions->setObjectName(QLatin1StringView("NWin TitleBar Actions"));
    tab->addTab(mTitleBarActions->widget(), i18n("&Titlebar Actions"));
    connect(mTitleBarActions, &KCModule::needsSaveChanged, this, [this]() {
        setNeedsSave(mTitleBarActions->needsSave());
    });
    connect(mTitleBarActions, &KCModule::representsDefaultsChanged, this, [this]() {
        setRepresentsDefaults(mTitleBarActions->representsDefaults());
    });

    mWindowActions = new KWindowActionsConfig(mSettings, widget());
    mWindowActions->setObjectName(QLatin1StringView("NWin Window Actions"));
    tab->addTab(mWindowActions->widget(), i18n("Window Actio&ns"));
    connect(mWindowActions, &KCModule::needsSaveChanged, this, [this]() {
        setNeedsSave(mWindowActions->needsSave());
    });
    connect(mWindowActions, &KCModule::representsDefaultsChanged, this, [this]() {
        setRepresentsDefaults(mWindowActions->representsDefaults());
    });
}

void KActionsOptions::load()
{
    mTitleBarActions->load();
    mWindowActions->load();
}

void KActionsOptions::save()
{
    mTitleBarActions->save();
    mWindowActions->save();

    setNeedsSave(false);
    // Send signal to all nwin instances
    QDBusMessage message = QDBusMessage::createSignal("/NWin", "org.kde.NWin", "reloadConfig");
    QDBusConnection::sessionBus().send(message);
}

void KActionsOptions::defaults()
{
    mTitleBarActions->defaults();
    mWindowActions->defaults();
}

void KActionsOptions::moduleChanged(bool state)
{
    setNeedsSave(state);
}

#include "main.moc"

#include "moc_main.cpp"
