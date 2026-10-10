/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2008 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2009 Lucas Murray <lmurray@undefinedfire.com>
    SPDX-FileCopyrightText: 2020 Cyril Rossi <cyril.rossi@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "main.h"
#include <nwin_effects_interface.h>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KPackage/Package>
#include <KPackage/PackageLoader>
#include <KPluginFactory>
#include <QDBusConnection>
#include <QVBoxLayout>

#include "nwinscreenedgeconfigform.h"
#include "nwinscreenedgedata.h"
#include "nwinscreenedgeeffectsettings.h"
#include "nwinscreenedgescriptsettings.h"
#include "nwinscreenedgesettings.h"

K_PLUGIN_FACTORY_WITH_JSON(NWinScreenEdgesConfigFactory, "kcm_nwinscreenedges.json", registerPlugin<NWin::NWinScreenEdgesConfig>(); registerPlugin<NWin::NWinScreenEdgeData>();)

namespace NWin
{

NWinScreenEdgesConfig::NWinScreenEdgesConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
    , m_form(new NWinScreenEdgesConfigForm(widget()))
    , m_config(KSharedConfig::openConfig("nwinrc"))
    , m_data(new NWinScreenEdgeData(this))
{
    QVBoxLayout *layout = new QVBoxLayout(widget());
    layout->addWidget(m_form);

    addConfig(m_data->settings(), m_form);

    monitorInit();

    connect(this, &NWinScreenEdgesConfig::defaultsIndicatorsVisibleChanged, m_form, [this]() {
        m_form->setDefaultsIndicatorsVisible(defaultsIndicatorsVisible());
    });
    connect(m_form, &NWinScreenEdgesConfigForm::saveNeededChanged, this, &NWinScreenEdgesConfig::unmanagedWidgetChangeState);
    connect(m_form, &NWinScreenEdgesConfigForm::defaultChanged, this, &NWinScreenEdgesConfig::unmanagedWidgetDefaultState);
}

NWinScreenEdgesConfig::~NWinScreenEdgesConfig()
{
}

void NWinScreenEdgesConfig::load()
{
    KCModule::load();
    m_data->settings()->load();
    for (NWinScreenEdgeScriptSettings *setting : std::as_const(m_scriptSettings)) {
        setting->load();
    }
    for (NWinScreenEdgeEffectSettings *setting : std::as_const(m_effectSettings)) {
        setting->load();
    }

    monitorLoadSettings();
    monitorLoadDefaultSettings();
    m_form->setRemainActiveOnFullscreen(m_data->settings()->remainActiveOnFullscreen());
    m_form->setElectricBorderCornerRatio(m_data->settings()->electricBorderCornerRatio());
    m_form->setDefaultElectricBorderCornerRatio(m_data->settings()->defaultElectricBorderCornerRatioValue());
    m_form->reload();
}

void NWinScreenEdgesConfig::save()
{
    monitorSaveSettings();
    m_data->settings()->setRemainActiveOnFullscreen(m_form->remainActiveOnFullscreen());
    m_data->settings()->setElectricBorderCornerRatio(m_form->electricBorderCornerRatio());
    m_data->settings()->save();
    for (NWinScreenEdgeScriptSettings *setting : std::as_const(m_scriptSettings)) {
        setting->save();
    }
    for (NWinScreenEdgeEffectSettings *setting : std::as_const(m_effectSettings)) {
        setting->save();
    }

    // Reload saved settings to ScreenEdge UI
    monitorLoadSettings();
    m_form->setElectricBorderCornerRatio(m_data->settings()->electricBorderCornerRatio());
    m_form->setRemainActiveOnFullscreen(m_data->settings()->remainActiveOnFullscreen());
    m_form->reload();

    // Reload NWin.
    QDBusMessage message = QDBusMessage::createSignal("/NWin", "org.kde.NWin", "reloadConfig");
    QDBusConnection::sessionBus().send(message);
    // and reconfigure the effects
    OrgKdeNWinEffectsInterface interface(QStringLiteral("org.kde.NWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());

    interface.reconfigureEffect(QStringLiteral("overview"));
    interface.reconfigureEffect(QStringLiteral("windowview"));

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

static QList<KPluginMetaData> listBuiltinEffects()
{
    const QString rootDirectory = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                         QStringLiteral("nwin-wayland/builtin-effects"),
                                                         QStandardPaths::LocateDirectory);

    QList<KPluginMetaData> ret;

    const QStringList nameFilters{QStringLiteral("*.json")};
    QDirIterator it(rootDirectory, nameFilters, QDir::Files);
    while (it.hasNext()) {
        it.next();
        if (const KPluginMetaData metaData = KPluginMetaData::fromJsonFile(it.filePath()); metaData.isValid()) {
            ret.append(metaData);
        }
    }

    return ret;
}

static QList<KPluginMetaData> listScriptedEffects()
{
    return KPackage::PackageLoader::self()->listPackages(QStringLiteral("NWin/Effect"), QStringLiteral("nwin-wayland/effects/"))
        + KPackage::PackageLoader::self()->listPackages(QStringLiteral("NWin/Effect"), QStringLiteral("nwin/effects/"));
}

void NWinScreenEdgesConfig::monitorInit()
{
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
    const auto effects = listBuiltinEffects() << listScriptedEffects();

    for (const KPluginMetaData &effect : effects) {
        if (!effect.value(QStringLiteral("X-NWin-Border-Activate"), false)) {
            continue;
        }

        if (!config.readEntry(effect.pluginId() + QStringLiteral("Enabled"), effect.isEnabledByDefault())) {
            continue;
        }
        m_effects << effect.pluginId();
        m_form->monitorAddItem(effect.name());
        m_effectSettings[effect.pluginId()] = new NWinScreenEdgeEffectSettings(effect.pluginId(), this);
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
        m_scriptSettings[script.pluginId()] = new NWinScreenEdgeScriptSettings(script.pluginId(), this);
    }

    monitorShowEvent();
}

void NWinScreenEdgesConfig::monitorLoadSettings()
{
    // Load ElectricBorderActions
    m_form->monitorChangeEdge(ElectricTop, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->top()));
    m_form->monitorChangeEdge(ElectricTopRight, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->topRight()));
    m_form->monitorChangeEdge(ElectricRight, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->right()));
    m_form->monitorChangeEdge(ElectricBottomRight, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->bottomRight()));
    m_form->monitorChangeEdge(ElectricBottom, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->bottom()));
    m_form->monitorChangeEdge(ElectricBottomLeft, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->bottomLeft()));
    m_form->monitorChangeEdge(ElectricLeft, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->left()));
    m_form->monitorChangeEdge(ElectricTopLeft, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->topLeft()));

    // Load effect-specific actions:

    // PresentWindows BorderActivateAll
    m_form->monitorChangeEdge(m_data->settings()->borderActivateAll(), PresentWindowsAll);

    // PresentWindows BorderActivate
    m_form->monitorChangeEdge(m_data->settings()->borderActivatePresentWindows(), PresentWindowsCurrent);

    // PresentWindows BorderActivateClass
    m_form->monitorChangeEdge(m_data->settings()->borderActivateClass(), PresentWindowsClass);

    // Overview
    m_form->monitorChangeEdge(m_data->settings()->borderActivateOverview(), Overview);
    m_form->monitorChangeEdge(m_data->settings()->borderActivateGrid(), Grid);

    // TabBox
    m_form->monitorChangeEdge(m_data->settings()->borderActivateTabBox(), TabBox);
    // Alternative TabBox
    m_form->monitorChangeEdge(m_data->settings()->borderAlternativeActivate(), TabBoxAlternative);

    // Dynamically loaded effects
    int lastIndex = EffectCount;
    for (int i = 0; i < m_effects.size(); i++) {
        m_form->monitorChangeEdge(m_effectSettings[m_effects[i]]->borderActivate(), lastIndex);
        ++lastIndex;
    }

    // Scripts
    for (int i = 0; i < m_scripts.size(); i++) {
        m_form->monitorChangeEdge(m_scriptSettings[m_scripts[i]]->borderActivate(), lastIndex);
        ++lastIndex;
    }
}

void NWinScreenEdgesConfig::monitorLoadDefaultSettings()
{
    // Load ElectricBorderActions
    m_form->monitorChangeDefaultEdge(ElectricTop, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->defaultTopValue()));
    m_form->monitorChangeDefaultEdge(ElectricTopRight, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->defaultTopRightValue()));
    m_form->monitorChangeDefaultEdge(ElectricRight, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->defaultRightValue()));
    m_form->monitorChangeDefaultEdge(ElectricBottomRight, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->defaultBottomRightValue()));
    m_form->monitorChangeDefaultEdge(ElectricBottom, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->defaultBottomValue()));
    m_form->monitorChangeDefaultEdge(ElectricBottomLeft, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->defaultBottomLeftValue()));
    m_form->monitorChangeDefaultEdge(ElectricLeft, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->defaultLeftValue()));
    m_form->monitorChangeDefaultEdge(ElectricTopLeft, NWinScreenEdgesConfig::electricBorderActionFromString(m_data->settings()->defaultTopLeftValue()));

    // Load effect-specific actions:

    // PresentWindows BorderActivateAll
    m_form->monitorChangeDefaultEdge(m_data->settings()->defaultBorderActivateAllValue(), PresentWindowsAll);

    // PresentWindows BorderActivate
    m_form->monitorChangeDefaultEdge(m_data->settings()->defaultBorderActivatePresentWindowsValue(), PresentWindowsCurrent);

    // PresentWindows BorderActivateClass
    m_form->monitorChangeDefaultEdge(m_data->settings()->defaultBorderActivateClassValue(), PresentWindowsClass);

    // Overview
    m_form->monitorChangeDefaultEdge(m_data->settings()->defaultBorderActivateOverviewValue(), Overview);
    m_form->monitorChangeDefaultEdge(m_data->settings()->defaultBorderActivateGridValue(), Grid);

    // TabBox
    m_form->monitorChangeDefaultEdge(m_data->settings()->defaultBorderActivateTabBoxValue(), TabBox);
    // Alternative TabBox
    m_form->monitorChangeDefaultEdge(m_data->settings()->defaultBorderAlternativeActivateValue(), TabBoxAlternative);
}

void NWinScreenEdgesConfig::monitorSaveSettings()
{
    // Save ElectricBorderActions
    m_data->settings()->setTop(NWinScreenEdgesConfig::electricBorderActionToString(m_form->selectedEdgeItem(ElectricTop)));
    m_data->settings()->setTopRight(NWinScreenEdgesConfig::electricBorderActionToString(m_form->selectedEdgeItem(ElectricTopRight)));
    m_data->settings()->setRight(NWinScreenEdgesConfig::electricBorderActionToString(m_form->selectedEdgeItem(ElectricRight)));
    m_data->settings()->setBottomRight(NWinScreenEdgesConfig::electricBorderActionToString(m_form->selectedEdgeItem(ElectricBottomRight)));
    m_data->settings()->setBottom(NWinScreenEdgesConfig::electricBorderActionToString(m_form->selectedEdgeItem(ElectricBottom)));
    m_data->settings()->setBottomLeft(NWinScreenEdgesConfig::electricBorderActionToString(m_form->selectedEdgeItem(ElectricBottomLeft)));
    m_data->settings()->setLeft(NWinScreenEdgesConfig::electricBorderActionToString(m_form->selectedEdgeItem(ElectricLeft)));
    m_data->settings()->setTopLeft(NWinScreenEdgesConfig::electricBorderActionToString(m_form->selectedEdgeItem(ElectricTopLeft)));

    // Save effect-specific actions:

    // Present Windows
    m_data->settings()->setBorderActivateAll(m_form->monitorCheckEffectHasEdge(PresentWindowsAll));
    m_data->settings()->setBorderActivatePresentWindows(m_form->monitorCheckEffectHasEdge(PresentWindowsCurrent));
    m_data->settings()->setBorderActivateClass(m_form->monitorCheckEffectHasEdge(PresentWindowsClass));

    // Overview
    m_data->settings()->setBorderActivateOverview(m_form->monitorCheckEffectHasEdge(Overview));
    m_data->settings()->setBorderActivateGrid(m_form->monitorCheckEffectHasEdge(Grid));

    // TabBox
    m_data->settings()->setBorderActivateTabBox(m_form->monitorCheckEffectHasEdge(TabBox));
    m_data->settings()->setBorderAlternativeActivate(m_form->monitorCheckEffectHasEdge(TabBoxAlternative));

    // Dynamically loaded effects
    int lastIndex = EffectCount;
    for (int i = 0; i < m_effects.size(); i++) {
        m_effectSettings[m_effects[i]]->setBorderActivate(m_form->monitorCheckEffectHasEdge(lastIndex));
        ++lastIndex;
    }

    // Scripts
    for (int i = 0; i < m_scripts.size(); i++) {
        m_scriptSettings[m_scripts[i]]->setBorderActivate(m_form->monitorCheckEffectHasEdge(lastIndex));
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

    // Disable Edge if ElectricBorders group entries are immutable
    m_form->monitorEnableEdge(ElectricTop, !m_data->settings()->isTopImmutable());
    m_form->monitorEnableEdge(ElectricTopRight, !m_data->settings()->isTopRightImmutable());
    m_form->monitorEnableEdge(ElectricRight, !m_data->settings()->isRightImmutable());
    m_form->monitorEnableEdge(ElectricBottomRight, !m_data->settings()->isBottomRightImmutable());
    m_form->monitorEnableEdge(ElectricBottom, !m_data->settings()->isBottomImmutable());
    m_form->monitorEnableEdge(ElectricBottomLeft, !m_data->settings()->isBottomLeftImmutable());
    m_form->monitorEnableEdge(ElectricLeft, !m_data->settings()->isLeftImmutable());
    m_form->monitorEnableEdge(ElectricTopLeft, !m_data->settings()->isTopLeftImmutable());

    // Disable ElectricBorderCornerRatio if entry is immutable
    m_form->setElectricBorderCornerRatioEnabled(!m_data->settings()->isElectricBorderCornerRatioImmutable());
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

#include "main.moc"

#include "moc_main.cpp"
