/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2008 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2009 Lucas Murray <lmurray@undefinedfire.com>
    SPDX-FileCopyrightText: 2020 Cyril Rossi <cyril.rossi@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "touch.h"
#include <nwin_effects_interface.h>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KPackage/Package>
#include <KPackage/PackageLoader>
#include <KPluginFactory>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QVBoxLayout>

#include "nwintouchscreendata.h"
#include "nwintouchscreenedgeconfigform.h"
#include "nwintouchscreenedgeeffectsettings.h"
#include "nwintouchscreenmoduledata.h"
#include "nwintouchscreenscriptsettings.h"
#include "nwintouchscreensettings.h"

K_PLUGIN_FACTORY_WITH_JSON(NWinScreenEdgesConfigFactory, "kcm_nwintouchscreen.json", registerPlugin<NWin::NWinScreenEdgesConfig>(); registerPlugin<NWin::NWinTouchScreenModuleData>();)

namespace NWin
{

NWinScreenEdgesConfig::NWinScreenEdgesConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
    , m_form(new NWinTouchScreenEdgeConfigForm(widget()))
    , m_config(KSharedConfig::openConfig("nwinrc"))
    , m_data(new NWinTouchScreenData(this))
{
    QVBoxLayout *layout = new QVBoxLayout(widget());
    layout->addWidget(m_form);

    monitorInit();

    connect(m_form, &NWinTouchScreenEdgeConfigForm::saveNeededChanged, this, &NWinScreenEdgesConfig::unmanagedWidgetChangeState);
    connect(m_form, &NWinTouchScreenEdgeConfigForm::defaultChanged, this, &NWinScreenEdgesConfig::unmanagedWidgetDefaultState);
}

NWinScreenEdgesConfig::~NWinScreenEdgesConfig()
{
}

void NWinScreenEdgesConfig::load()
{
    KCModule::load();
    m_data->settings()->load();
    for (NWinTouchScreenScriptSettings *setting : std::as_const(m_scriptSettings)) {
        setting->load();
    }
    for (NWinTouchScreenEdgeEffectSettings *setting : std::as_const(m_effectSettings)) {
        setting->load();
    }

    monitorLoadSettings();
    monitorLoadDefaultSettings();
    m_form->reload();
}

void NWinScreenEdgesConfig::save()
{
    monitorSaveSettings();
    m_data->settings()->save();
    for (NWinTouchScreenScriptSettings *setting : std::as_const(m_scriptSettings)) {
        setting->save();
    }
    for (NWinTouchScreenEdgeEffectSettings *setting : std::as_const(m_effectSettings)) {
        setting->save();
    }

    // Reload saved settings to ScreenEdge UI
    monitorLoadSettings();
    m_form->reload();

    // Reload NWin.
    QDBusMessage message = QDBusMessage::createSignal("/NWin", "org.kde.NWin", "reloadConfig");
    QDBusConnection::sessionBus().send(message);
    // and reconfigure the effects
    OrgKdeNWinEffectsInterface interface(QStringLiteral("org.kde.NWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("windowview"));
    interface.reconfigureEffect(QStringLiteral("overview"));
    for (const auto &effectId : std::as_const(m_effects)) {
        interface.reconfigureEffect(effectId);
    }

    KCModule::save();
}

void NWinScreenEdgesConfig::defaults()
{
    m_form->setDefaults();

    KCModule::defaults();
}

//-----------------------------------------------------------------------------
// Monitor

void NWinScreenEdgesConfig::monitorInit()
{
    m_form->monitorHideEdge(ElectricTopLeft, true);
    m_form->monitorHideEdge(ElectricTopRight, true);
    m_form->monitorHideEdge(ElectricBottomRight, true);
    m_form->monitorHideEdge(ElectricBottomLeft, true);

    m_form->monitorAddItem(i18n("No Action"));
    m_form->monitorAddItem(i18n("Peek at Desktop"));
    m_form->monitorAddItem(i18n("Lock Screen"));
    m_form->monitorAddItem(i18n("Show KRunner"));
    m_form->monitorAddItem(i18n("Activity Manager"));
    m_form->monitorAddItem(i18n("Application Launcher"));

    // TODO: Find a better way to get the display name of the present windows,
    // Maybe install metadata.json files?
    const QString presentWindowsName = i18n("Present Windows");
    m_form->monitorAddItem(i18n("%1 - All Desktops", presentWindowsName));
    m_form->monitorAddItem(i18n("%1 - Current Desktop", presentWindowsName));
    m_form->monitorAddItem(i18n("%1 - Current Application", presentWindowsName));

    m_form->monitorAddItem(i18n("Overview"));
    m_form->monitorAddItem(i18n("Grid"));

    m_form->monitorAddItem(i18n("Toggle window switching"));
    m_form->monitorAddItem(i18n("Toggle alternative window switching"));

    KConfigGroup config(m_config, QStringLiteral("Plugins"));
    const auto effects = KPackage::PackageLoader::self()->listPackages(QStringLiteral("NWin/Script"), QStringLiteral("nwin-wayland/builtin-effects/"))
        + KPackage::PackageLoader::self()->listPackages(QStringLiteral("NWin/Script"), QStringLiteral("nwin-wayland/effects/"))
        + KPackage::PackageLoader::self()->listPackages(QStringLiteral("NWin/Script"), QStringLiteral("nwin/effects/"));

    for (const KPluginMetaData &effect : effects) {
        if (!effect.value(QStringLiteral("X-NWin-Border-Activate"), false)) {
            continue;
        }

        if (!config.readEntry(effect.pluginId() + QStringLiteral("Enabled"), effect.isEnabledByDefault())) {
            continue;
        }
        m_effects << effect.pluginId();
        m_form->monitorAddItem(effect.name());
        m_effectSettings[effect.pluginId()] = new NWinTouchScreenEdgeEffectSettings(effect.pluginId(), this);
    }

    const auto scripts = KPackage::PackageLoader::self()->listPackages(QStringLiteral("NWin/Script"), QStringLiteral("nwin-wayland/scripts/"))
        + KPackage::PackageLoader::self()->listPackages(QStringLiteral("NWin/Script"), QStringLiteral("nwin/scripts/"));

    for (const KPluginMetaData &script : scripts) {
        if (script.value(QStringLiteral("X-NWin-Border-Activate"), false) != true) {
            continue;
        }

        if (!config.readEntry(script.pluginId() + QStringLiteral("Enabled"), script.isEnabledByDefault())) {
            continue;
        }
        m_scripts << script.pluginId();
        m_form->monitorAddItem(script.name());
        m_scriptSettings[script.pluginId()] = new NWinTouchScreenScriptSettings(script.pluginId(), this);
    }

    monitorShowEvent();
}

void NWinScreenEdgesConfig::monitorLoadSettings()
{
    // Load ElectricBorderActions
    m_form->monitorChangeEdge(ElectricTop, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->top()));
    m_form->monitorChangeEdge(ElectricRight, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->right()));
    m_form->monitorChangeEdge(ElectricBottom, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->bottom()));
    m_form->monitorChangeEdge(ElectricLeft, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->left()));

    // Load effect-specific actions:

    // Present Windows BorderActivateAll
    m_form->monitorChangeEdge(m_data->settings()->touchBorderActivateAll(), PresentWindowsAll);
    // PresentWindows BorderActivate
    m_form->monitorChangeEdge(m_data->settings()->touchBorderActivatePresentWindows(), PresentWindowsCurrent);
    // PresentWindows BorderActivateClass
    m_form->monitorChangeEdge(m_data->settings()->touchBorderActivateClass(), PresentWindowsClass);

    // Overview
    m_form->monitorChangeEdge(m_data->settings()->touchBorderActivateOverview(), Overview);
    m_form->monitorChangeEdge(m_data->settings()->touchBorderActivateGrid(), Grid);

    // TabBox BorderActivate
    m_form->monitorChangeEdge(m_data->settings()->touchBorderActivateTabBox(), TabBox);
    // Alternative TabBox
    m_form->monitorChangeEdge(m_data->settings()->touchBorderAlternativeActivate(), TabBoxAlternative);

    // Dynamically loaded effects
    int lastIndex = EffectCount;
    for (int i = 0; i < m_effects.size(); i++) {
        m_form->monitorChangeEdge(m_effectSettings[m_effects[i]]->touchBorderActivate(), lastIndex);
        ++lastIndex;
    }

    // Scripts
    for (int i = 0; i < m_scripts.size(); i++) {
        m_form->monitorChangeEdge(m_scriptSettings[m_scripts[i]]->touchBorderActivate(), lastIndex);
        ++lastIndex;
    }
}

void NWinScreenEdgesConfig::monitorLoadDefaultSettings()
{
    m_form->monitorChangeDefaultEdge(ElectricTop, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->defaultTopValue()));
    m_form->monitorChangeDefaultEdge(ElectricRight, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->defaultRightValue()));
    m_form->monitorChangeDefaultEdge(ElectricBottom, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->defaultBottomValue()));
    m_form->monitorChangeDefaultEdge(ElectricLeft, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->defaultLeftValue()));

    // Present Windows BorderActivateAll
    m_form->monitorChangeDefaultEdge(m_data->settings()->defaultTouchBorderActivateAllValue(), PresentWindowsAll);
    // PresentWindows BorderActivate
    m_form->monitorChangeDefaultEdge(m_data->settings()->defaultTouchBorderActivatePresentWindowsValue(), PresentWindowsCurrent);
    // PresentWindows BorderActivateClass
    m_form->monitorChangeDefaultEdge(m_data->settings()->defaultTouchBorderActivateClassValue(), PresentWindowsClass);

    // Overview
    m_form->monitorChangeDefaultEdge(m_data->settings()->defaultTouchBorderActivateOverviewValue(), Overview);
    m_form->monitorChangeDefaultEdge(m_data->settings()->defaultTouchBorderActivateGridValue(), Grid);

    // TabBox BorderActivate
    m_form->monitorChangeDefaultEdge(m_data->settings()->defaultTouchBorderActivateTabBoxValue(), TabBox);
    // Alternative TabBox
    m_form->monitorChangeDefaultEdge(m_data->settings()->defaultTouchBorderAlternativeActivateValue(), TabBoxAlternative);
}

void NWinScreenEdgesConfig::monitorSaveSettings()
{
    // Save ElectricBorderActions
    m_data->settings()->setTop(NWinScreenEdgesConfig::electricBorderActionToString(m_form->selectedEdgeItem(ElectricTop)));
    m_data->settings()->setRight(NWinScreenEdgesConfig::electricBorderActionToString(m_form->selectedEdgeItem(ElectricRight)));
    m_data->settings()->setBottom(NWinScreenEdgesConfig::electricBorderActionToString(m_form->selectedEdgeItem(ElectricBottom)));
    m_data->settings()->setLeft(NWinScreenEdgesConfig::electricBorderActionToString(m_form->selectedEdgeItem(ElectricLeft)));

    // Save effect-specific actions:

    // Present Windows
    m_data->settings()->setTouchBorderActivateAll(m_form->monitorCheckEffectHasEdge(PresentWindowsAll));
    m_data->settings()->setTouchBorderActivatePresentWindows(m_form->monitorCheckEffectHasEdge(PresentWindowsCurrent));
    m_data->settings()->setTouchBorderActivateClass(m_form->monitorCheckEffectHasEdge(PresentWindowsClass));

    // Overview
    m_data->settings()->setTouchBorderActivateOverview(m_form->monitorCheckEffectHasEdge(Overview));
    m_data->settings()->setTouchBorderActivateGrid(m_form->monitorCheckEffectHasEdge(Grid));

    // TabBox
    m_data->settings()->setTouchBorderActivateTabBox(m_form->monitorCheckEffectHasEdge(TabBox));
    m_data->settings()->setTouchBorderAlternativeActivate(m_form->monitorCheckEffectHasEdge(TabBoxAlternative));

    // Dynamically loaded effects
    int lastIndex = EffectCount;
    for (int i = 0; i < m_effects.size(); i++) {
        m_effectSettings[m_effects[i]]->setTouchBorderActivate(m_form->monitorCheckEffectHasEdge(lastIndex));
        ++lastIndex;
    }

    // Scripts
    for (int i = 0; i < m_scripts.size(); i++) {
        m_scriptSettings[m_scripts[i]]->setTouchBorderActivate(m_form->monitorCheckEffectHasEdge(lastIndex));
        ++lastIndex;
    }
}

void NWinScreenEdgesConfig::monitorShowEvent()
{
    // Check if they are enabled
    KConfigGroup config(m_config, QStringLiteral("Plugins"));

    // Present Windows
    bool enabled = config.readEntry("windowviewEnabled", true);
    m_form->monitorItemSetEnabled(PresentWindowsCurrent, enabled);
    m_form->monitorItemSetEnabled(PresentWindowsAll, enabled);

    // Overview
    const bool overviewEnabled = config.readEntry("overviewEnabled", true);
    m_form->monitorItemSetEnabled(Overview, overviewEnabled);
    m_form->monitorItemSetEnabled(Grid, overviewEnabled);

    // tabbox, depends on reasonable focus policy.
    KConfigGroup config2(m_config, QStringLiteral("Windows"));
    QString focusPolicy = config2.readEntry("FocusPolicy", QString());
    bool reasonable = focusPolicy != "FocusStrictlyUnderMouse" && focusPolicy != "FocusUnderMouse";
    m_form->monitorItemSetEnabled(TabBox, reasonable);
    m_form->monitorItemSetEnabled(TabBoxAlternative, reasonable);

    // Disable Edge if TouchEdges group entries are immutable
    m_form->monitorEnableEdge(ElectricTop, !m_data->settings()->isTopImmutable());
    m_form->monitorEnableEdge(ElectricRight, !m_data->settings()->isRightImmutable());
    m_form->monitorEnableEdge(ElectricBottom, !m_data->settings()->isBottomImmutable());
    m_form->monitorEnableEdge(ElectricLeft, !m_data->settings()->isLeftImmutable());
}

ElectricBorderAction NWinScreenEdgesConfig::electricBorderActionFromString(const QString &string)
{
    QString lowerName = string.toLower();
    if (lowerName == QLatin1StringView("showdesktop")) {
        return ElectricActionShowDesktop;
    }
    if (lowerName == QLatin1StringView("lockscreen")) {
        return ElectricActionLockScreen;
    }
    if (lowerName == QLatin1StringView("krunner")) {
        return ElectricActionKRunner;
    }
    if (lowerName == QLatin1StringView("activitymanager")) {
        return ElectricActionActivityManager;
    }
    if (lowerName == QLatin1StringView("applicationlauncher")) {
        return ElectricActionApplicationLauncher;
    }
    return ElectricActionNone;
}

QString NWinScreenEdgesConfig::electricBorderActionToString(int action)
{
    switch (action) {
    case 1:
        return QStringLiteral("ShowDesktop");
    case 2:
        return QStringLiteral("LockScreen");
    case 3:
        return QStringLiteral("KRunner");
    case 4:
        return QStringLiteral("ActivityManager");
    case 5:
        return QStringLiteral("ApplicationLauncher");
    default:
        return QStringLiteral("None");
    }
}

} // namespace

#include "touch.moc"

#include "moc_touch.cpp"
