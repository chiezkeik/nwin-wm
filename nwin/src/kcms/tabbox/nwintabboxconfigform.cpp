/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2009 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2020 Cyril Rossi <cyril.rossi@enioka.com>
    SPDX-FileCopyrightText: 2023 Ismael Asensio <isma.af@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "nwintabboxconfigform.h"
#include "nwintabboxsettings.h"
#include "shortcutsettings.h"
#include "ui_main.h"

namespace NWin
{

using namespace TabBox;

NWinTabBoxConfigForm::NWinTabBoxConfigForm(TabboxType type, TabBoxSettings *config, ShortcutSettings *shortcutsConfig, QWidget *parent)
    : QWidget(parent)
    , m_config(config)
    , m_shortcuts(shortcutsConfig)
    , ui(new Ui::NWinTabBoxConfigForm)
{
    ui->setupUi(this);

    if (QApplication::screens().count() < 2) {
        ui->filterScreens->hide();
        ui->screenFilter->hide();
    }

    connect(this, &NWinTabBoxConfigForm::configChanged, this, &NWinTabBoxConfigForm::updateDefaultIndicators);

    connect(ui->effectPreviewButton, &QPushButton::clicked, this, &NWinTabBoxConfigForm::effectPreviewClicked);

    connect(ui->filterScreens, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onFilterScreen);
    connect(ui->currentScreen, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onFilterScreen);
    connect(ui->otherScreens, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onFilterScreen);

    connect(ui->filterDesktops, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onFilterDesktop);
    connect(ui->currentDesktop, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onFilterDesktop);
    connect(ui->otherDesktops, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onFilterDesktop);

    connect(ui->filterActivities, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onFilterActivites);
    connect(ui->currentActivity, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onFilterActivites);
    connect(ui->otherActivities, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onFilterActivites);

    connect(ui->filterMinimization, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onFilterMinimization);
    connect(ui->visibleWindows, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onFilterMinimization);
    connect(ui->hiddenWindows, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onFilterMinimization);

    connect(ui->oneAppWindow, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onApplicationMode);
    connect(ui->orderMinimized, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onOrderMinimizedMode);
    connect(ui->showDesktop, &QAbstractButton::clicked, this, &NWinTabBoxConfigForm::onShowDesktopMode);

    connect(ui->switchingModeCombo, &QComboBox::activated, this, &NWinTabBoxConfigForm::onSwitchingMode);
    connect(ui->showScreenCombo, &QComboBox::activated, this, &NWinTabBoxConfigForm::onShowScreenMode);
    connect(ui->effectCombo, &QComboBox::activated, this, &NWinTabBoxConfigForm::onEffectCombo);

    auto initShortcutWidget = [this](KKeySequenceWidget *primary, KKeySequenceWidget *alternate, const QString &name) {
        primary->setCheckActionCollections({m_shortcuts->actionCollection()});
        primary->setProperty("shortcutAction", name);
        connect(primary, &KKeySequenceWidget::keySequenceChanged, this, [this, name](const QKeySequence &seq) {
            if (m_shortcuts->primaryShortcut(name) != seq) {
                m_shortcuts->setShortcuts(name, {seq, m_shortcuts->alternateShortcut(name)});
                Q_EMIT configChanged();
            }
        });

        alternate->setCheckActionCollections({m_shortcuts->actionCollection()});
        alternate->setProperty("shortcutAction", name);
        connect(alternate, &KKeySequenceWidget::keySequenceChanged, this, [this, name](const QKeySequence &seq) {
            if (m_shortcuts->alternateShortcut(name) != seq) {
                m_shortcuts->setShortcuts(name, {m_shortcuts->primaryShortcut(name), seq});
                Q_EMIT configChanged();
            }
        });
    };

    if (TabboxType::Main == type) {
        initShortcutWidget(ui->scAll, ui->scAllAlternate, QStringLiteral("Walk Through Windows"));
        initShortcutWidget(ui->scAllReverse, ui->scAllReverseAlternate, QStringLiteral("Walk Through Windows (Reverse)"));
        initShortcutWidget(ui->scCurrent, ui->scCurrentAlternate, QStringLiteral("Walk Through Windows of Current Application"));
        initShortcutWidget(ui->scCurrentReverse, ui->scCurrentReverseAlternate, QStringLiteral("Walk Through Windows of Current Application (Reverse)"));
    } else if (TabboxType::Alternative == type) {
        initShortcutWidget(ui->scAll, ui->scAllAlternate, QStringLiteral("Walk Through Windows Alternative"));
        initShortcutWidget(ui->scAllReverse, ui->scAllReverseAlternate, QStringLiteral("Walk Through Windows Alternative (Reverse)"));
        initShortcutWidget(ui->scCurrent, ui->scCurrentAlternate, QStringLiteral("Walk Through Windows of Current Application Alternative"));
        initShortcutWidget(ui->scCurrentReverse, ui->scCurrentReverseAlternate, QStringLiteral("Walk Through Windows of Current Application Alternative (Reverse)"));
    }

    updateUiFromConfig();
}

NWinTabBoxConfigForm::~NWinTabBoxConfigForm()
{
    delete ui;
}

TabBoxSettings *NWinTabBoxConfigForm::config() const
{
    return m_config;
}

bool NWinTabBoxConfigForm::highlightWindows() const
{
    return ui->kcfg_HighlightWindows->isChecked();
}

bool NWinTabBoxConfigForm::showTabBox() const
{
    return ui->kcfg_ShowTabBox->isChecked();
}

int NWinTabBoxConfigForm::filterScreen() const
{
    if (ui->filterScreens->isChecked()) {
        return ui->currentScreen->isChecked() ? TabBoxConfig::OnlyCurrentScreenClients : TabBoxConfig::ExcludeCurrentScreenClients;
    } else {
        return TabBoxConfig::IgnoreMultiScreen;
    }
}

int NWinTabBoxConfigForm::filterDesktop() const
{
    if (ui->filterDesktops->isChecked()) {
        return ui->currentDesktop->isChecked() ? TabBoxConfig::OnlyCurrentDesktopClients : TabBoxConfig::ExcludeCurrentDesktopClients;
    } else {
        return TabBoxConfig::AllDesktopsClients;
    }
}

int NWinTabBoxConfigForm::filterActivities() const
{
    if (ui->filterActivities->isChecked()) {
        return ui->currentActivity->isChecked() ? TabBoxConfig::OnlyCurrentActivityClients : TabBoxConfig::ExcludeCurrentActivityClients;
    } else {
        return TabBoxConfig::AllActivitiesClients;
    }
}

int NWinTabBoxConfigForm::filterMinimization() const
{
    if (ui->filterMinimization->isChecked()) {
        return ui->visibleWindows->isChecked() ? TabBoxConfig::ExcludeMinimizedClients : TabBoxConfig::OnlyMinimizedClients;
    } else {
        return TabBoxConfig::IgnoreMinimizedStatus;
    }
}

int NWinTabBoxConfigForm::applicationMode() const
{
    return ui->oneAppWindow->isChecked() ? TabBoxConfig::OneWindowPerApplication : TabBoxConfig::AllWindowsAllApplications;
}

int NWinTabBoxConfigForm::orderMinimizedMode() const
{
    return ui->orderMinimized->isChecked() ? TabBoxConfig::GroupByMinimized : TabBoxConfig::NoGroupByMinimized;
}

int NWinTabBoxConfigForm::showDesktopMode() const
{
    return ui->showDesktop->isChecked() ? TabBoxConfig::ShowDesktopClient : TabBoxConfig::DoNotShowDesktopClient;
}

int NWinTabBoxConfigForm::switchingMode() const
{
    return ui->switchingModeCombo->currentIndex();
}

int NWinTabBoxConfigForm::showScreenMode() const
{
    return ui->showScreenCombo->currentIndex();
}

QString NWinTabBoxConfigForm::layoutName() const
{
    return ui->effectCombo->currentData().toString();
}

void NWinTabBoxConfigForm::setFilterScreen(TabBox::TabBoxConfig::ClientMultiScreenMode mode)
{
    ui->filterScreens->setChecked(mode != TabBoxConfig::IgnoreMultiScreen);
    ui->currentScreen->setChecked(mode == TabBoxConfig::OnlyCurrentScreenClients);
    ui->otherScreens->setChecked(mode == TabBoxConfig::ExcludeCurrentScreenClients);
}

void NWinTabBoxConfigForm::setFilterDesktop(TabBox::TabBoxConfig::ClientDesktopMode mode)
{
    ui->filterDesktops->setChecked(mode != TabBoxConfig::AllDesktopsClients);
    ui->currentDesktop->setChecked(mode == TabBoxConfig::OnlyCurrentDesktopClients);
    ui->otherDesktops->setChecked(mode == TabBoxConfig::ExcludeCurrentDesktopClients);
}

void NWinTabBoxConfigForm::setFilterActivities(TabBox::TabBoxConfig::ClientActivitiesMode mode)
{
    ui->filterActivities->setChecked(mode != TabBoxConfig::AllActivitiesClients);
    ui->currentActivity->setChecked(mode == TabBoxConfig::OnlyCurrentActivityClients);
    ui->otherActivities->setChecked(mode == TabBoxConfig::ExcludeCurrentActivityClients);
}

void NWinTabBoxConfigForm::setFilterMinimization(TabBox::TabBoxConfig::ClientMinimizedMode mode)
{
    ui->filterMinimization->setChecked(mode != TabBoxConfig::IgnoreMinimizedStatus);
    ui->visibleWindows->setChecked(mode == TabBoxConfig::ExcludeMinimizedClients);
    ui->hiddenWindows->setChecked(mode == TabBoxConfig::OnlyMinimizedClients);
}

void NWinTabBoxConfigForm::setApplicationMode(TabBox::TabBoxConfig::ClientApplicationsMode mode)
{
    ui->oneAppWindow->setChecked(mode == TabBoxConfig::OneWindowPerApplication);
}

void NWinTabBoxConfigForm::setOrderMinimizedMode(TabBox::TabBoxConfig::OrderMinimizedMode mode)
{
    ui->orderMinimized->setChecked(mode == TabBoxConfig::GroupByMinimized);
}

void NWinTabBoxConfigForm::setShowDesktopMode(TabBox::TabBoxConfig::ShowDesktopMode mode)
{
    ui->showDesktop->setChecked(mode == TabBoxConfig::ShowDesktopClient);
}

void NWinTabBoxConfigForm::setSwitchingModeChanged(TabBox::TabBoxConfig::ClientSwitchingMode mode)
{
    ui->switchingModeCombo->setCurrentIndex(mode);
}

void NWinTabBoxConfigForm::setShowScreenMode(TabBox::TabBoxConfig::ShowScreenMode mode)
{
    ui->showScreenCombo->setCurrentIndex(mode);
}

void NWinTabBoxConfigForm::setLayoutName(const QString &layoutName)
{
    const int index = ui->effectCombo->findData(layoutName);
    if (index >= 0) {
        ui->effectCombo->setCurrentIndex(index);
    }
}

void NWinTabBoxConfigForm::setEffectComboModel(QStandardItemModel *model)
{
    // We don't want to lose the config layout when resetting the combo model
    const QString layout = m_config->layoutName();
    ui->effectCombo->setModel(model);
    setLayoutName(layout);
}

QVariant NWinTabBoxConfigForm::effectComboCurrentData(int role) const
{
    return ui->effectCombo->currentData(role);
}

void NWinTabBoxConfigForm::onFilterScreen()
{
    m_config->setMultiScreenMode(filterScreen());
    Q_EMIT configChanged();
}

void NWinTabBoxConfigForm::onFilterDesktop()
{
    m_config->setDesktopMode(filterDesktop());
    Q_EMIT configChanged();
}

void NWinTabBoxConfigForm::onFilterActivites()
{
    m_config->setActivitiesMode(filterActivities());
    Q_EMIT configChanged();
}

void NWinTabBoxConfigForm::onFilterMinimization()
{
    m_config->setMinimizedMode(filterMinimization());
    Q_EMIT configChanged();
}

void NWin::NWinTabBoxConfigForm::onApplicationMode()
{
    m_config->setApplicationsMode(applicationMode());
    Q_EMIT configChanged();
}

void NWinTabBoxConfigForm::onOrderMinimizedMode()
{
    m_config->setOrderMinimizedMode(orderMinimizedMode());
    Q_EMIT configChanged();
}

void NWinTabBoxConfigForm::onShowDesktopMode()
{
    m_config->setShowDesktopMode(showDesktopMode());
    Q_EMIT configChanged();
}

void NWinTabBoxConfigForm::onSwitchingMode()
{
    m_config->setSwitchingMode(switchingMode());
    Q_EMIT configChanged();
}

void NWinTabBoxConfigForm::onShowScreenMode()
{
    m_config->setShowScreenMode(showScreenMode());
    Q_EMIT configChanged();
}

void NWinTabBoxConfigForm::onEffectCombo()
{
    if (!ui->kcfg_ShowTabBox->isChecked()) {
        return;
    }

    m_config->setLayoutName(layoutName());
    Q_EMIT configChanged();
}

void NWinTabBoxConfigForm::updateUiFromConfig()
{
    setFilterScreen(static_cast<TabBoxConfig::ClientMultiScreenMode>(m_config->multiScreenMode()));
    setFilterDesktop(static_cast<TabBoxConfig::ClientDesktopMode>(m_config->desktopMode()));
    setFilterActivities(static_cast<TabBoxConfig::ClientActivitiesMode>(m_config->activitiesMode()));
    setFilterMinimization(static_cast<TabBoxConfig::ClientMinimizedMode>(m_config->minimizedMode()));
    setApplicationMode(static_cast<TabBoxConfig::ClientApplicationsMode>(m_config->applicationsMode()));
    setOrderMinimizedMode(static_cast<TabBoxConfig::OrderMinimizedMode>(m_config->orderMinimizedMode()));
    setShowDesktopMode(static_cast<TabBoxConfig::ShowDesktopMode>(m_config->showDesktopMode()));
    setSwitchingModeChanged(static_cast<TabBoxConfig::ClientSwitchingMode>(m_config->switchingMode()));
    setShowScreenMode(static_cast<TabBoxConfig::ShowScreenMode>(m_config->showScreenMode()));
    setLayoutName(m_config->layoutName());

    for (const auto &widget : {ui->scAll, ui->scAllReverse, ui->scCurrent, ui->scCurrentReverse}) {
        const QString actionName = widget->property("shortcutAction").toString();
        widget->setKeySequence(m_shortcuts->primaryShortcut(actionName));
    }
    for (const auto &widget : {ui->scAllAlternate, ui->scAllReverseAlternate, ui->scCurrentAlternate, ui->scCurrentReverseAlternate}) {
        const QString actionName = widget->property("shortcutAction").toString();
        widget->setKeySequence(m_shortcuts->alternateShortcut(actionName));
    }

    updateDefaultIndicators();
}

void NWinTabBoxConfigForm::setEnabledUi()
{
    ui->kcfg_HighlightWindows->setEnabled(!m_config->isHighlightWindowsImmutable());

    ui->filterScreens->setEnabled(!m_config->isMultiScreenModeImmutable());
    ui->currentScreen->setEnabled(!m_config->isMultiScreenModeImmutable());
    ui->otherScreens->setEnabled(!m_config->isMultiScreenModeImmutable());

    ui->filterDesktops->setEnabled(!m_config->isDesktopModeImmutable());
    ui->currentDesktop->setEnabled(!m_config->isDesktopModeImmutable());
    ui->otherDesktops->setEnabled(!m_config->isDesktopModeImmutable());

    ui->filterActivities->setEnabled(!m_config->isActivitiesModeImmutable());
    ui->currentActivity->setEnabled(!m_config->isActivitiesModeImmutable());
    ui->otherActivities->setEnabled(!m_config->isActivitiesModeImmutable());

    ui->filterMinimization->setEnabled(!m_config->isMinimizedModeImmutable());
    ui->visibleWindows->setEnabled(!m_config->isMinimizedModeImmutable());
    ui->hiddenWindows->setEnabled(!m_config->isMinimizedModeImmutable());

    ui->oneAppWindow->setEnabled(!m_config->isApplicationsModeImmutable());
    ui->orderMinimized->setEnabled(!m_config->isOrderMinimizedModeImmutable());
    ui->showDesktop->setEnabled(!m_config->isShowDesktopModeImmutable());
    ui->switchingModeCombo->setEnabled(!m_config->isSwitchingModeImmutable());
    ui->showScreenCombo->setEnabled(!m_config->isShowScreenModeImmutable());
    ui->effectCombo->setEnabled(!m_config->isLayoutNameImmutable());
}

void NWinTabBoxConfigForm::setDefaultIndicatorVisible(bool show)
{
    m_showDefaultIndicator = show;
    updateDefaultIndicators();
}

void NWinTabBoxConfigForm::updateDefaultIndicators()
{
    applyDefaultIndicator({ui->filterScreens, ui->currentScreen, ui->otherScreens},
                          m_config->multiScreenMode() == m_config->defaultMultiScreenModeValue());
    applyDefaultIndicator({ui->filterDesktops, ui->currentDesktop, ui->otherDesktops},
                          m_config->desktopMode() == m_config->defaultDesktopModeValue());
    applyDefaultIndicator({ui->filterActivities, ui->currentActivity, ui->otherActivities},
                          m_config->activitiesMode() == m_config->defaultActivitiesModeValue());
    applyDefaultIndicator({ui->filterMinimization, ui->visibleWindows, ui->hiddenWindows},
                          m_config->minimizedMode() == m_config->defaultMinimizedModeValue());
    applyDefaultIndicator({ui->oneAppWindow}, m_config->applicationsMode() == m_config->defaultApplicationsModeValue());
    applyDefaultIndicator({ui->orderMinimized}, m_config->orderMinimizedMode() == m_config->defaultOrderMinimizedModeValue());
    applyDefaultIndicator({ui->showDesktop}, m_config->showDesktopMode() == m_config->defaultShowDesktopModeValue());
    applyDefaultIndicator({ui->switchingModeCombo}, m_config->switchingMode() == m_config->defaultSwitchingModeValue());
    applyDefaultIndicator({ui->showScreenCombo}, m_config->showScreenMode() == m_config->defaultShowScreenModeValue());
    applyDefaultIndicator({ui->effectCombo}, m_config->layoutName() == m_config->defaultLayoutNameValue());

    for (const auto &widget : {ui->scAll, ui->scAllAlternate, ui->scAllReverse, ui->scAllReverseAlternate, ui->scCurrent, ui->scCurrentAlternate, ui->scCurrentReverse, ui->scCurrentReverseAlternate}) {
        const QString actionName = widget->property("shortcutAction").toString();
        applyDefaultIndicator({widget}, m_shortcuts->isDefault(actionName));
    }
}

void NWinTabBoxConfigForm::applyDefaultIndicator(QList<QWidget *> widgets, bool isDefault)
{
    for (auto widget : widgets) {
        widget->setProperty("_kde_highlight_neutral", m_showDefaultIndicator && !isDefault);
        widget->update();
    }
}

} // namespace

#include "moc_nwintabboxconfigform.cpp"
