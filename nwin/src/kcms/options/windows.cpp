/*
    windows.cpp

    SPDX-FileCopyrightText: 1997 Patrick Dowler <dowler@morgul.fsh.uvic.ca>
    SPDX-FileCopyrightText: 2001 Waldo Bastian <bastian@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QApplication>
#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QScreen>
#include <QtDBus>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KWindowSystem>

#include "windows.h"
#include <nwin_effects_interface.h>

#include "nwinoptions_kdeglobals_settings.h"
#include "nwinoptions_settings.h"

#define CLICK_TO_FOCUS 0
#define CLICK_TO_FOCUS_MOUSE_PRECEDENT 1
#define FOCUS_FOLLOWS_MOUSE 2
#define FOCUS_FOLLOWS_MOUSE_PRECEDENT 3
#define FOCUS_UNDER_MOUSE 4
#define FOCUS_STRICTLY_UNDER_MOUSE 5

namespace
{

constexpr int defaultFocusPolicyIndex = CLICK_TO_FOCUS;

}

NWinFocusConfigForm::NWinFocusConfigForm(QWidget *parent)
    : QWidget(parent)
{
    setupUi(parent);
}

KFocusConfig::KFocusConfig(NWinOptionsSettings *settings, QWidget *parent)
    : KCModule(parent, KPluginMetaData())
    , m_ui(new NWinFocusConfigForm(widget()))
{
    if (settings) {
        initialize(settings);
    }
}

void KFocusConfig::initialize(NWinOptionsSettings *settings)
{
    m_settings = settings;
    addConfig(m_settings, widget());

    connect(m_ui->windowFocusPolicy, qOverload<int>(&QComboBox::currentIndexChanged), this, &KFocusConfig::focusPolicyChanged);
    connect(m_ui->windowFocusPolicy, qOverload<int>(&QComboBox::currentIndexChanged), this, &KFocusConfig::updateDefaultIndicator);
    connect(this, SIGNAL(defaultsIndicatorsVisibleChanged(bool)), this, SLOT(updateDefaultIndicator()));

    connect(qApp, &QGuiApplication::screenAdded, this, &KFocusConfig::updateMultiScreen);
    connect(qApp, &QGuiApplication::screenRemoved, this, &KFocusConfig::updateMultiScreen);
    updateMultiScreen();
}

void KFocusConfig::updateMultiScreen()
{
    m_ui->multiscreenBehaviorLabel->setVisible(QApplication::screens().count() > 1);
    m_ui->kcfg_SeparateScreenFocus->setVisible(QApplication::screens().count() > 1);
}

void KFocusConfig::updateDefaultIndicator()
{
    const bool isDefault = m_ui->windowFocusPolicy->currentIndex() == defaultFocusPolicyIndex;
    m_ui->windowFocusPolicy->setProperty("_kde_highlight_neutral", defaultsIndicatorsVisible() && !isDefault);
    m_ui->windowFocusPolicy->update();
}

void KFocusConfig::updateFocusPolicyExplanatoryText()
{
    const int focusPolicy = m_ui->windowFocusPolicy->currentIndex();
    switch (focusPolicy) {
    case CLICK_TO_FOCUS:
        m_ui->windowFocusPolicyDescriptionLabel->setText(i18n("<em>Click to focus:</em> A window becomes active when you click into it. This behavior is common on other operating systems and likely what you want."));
        break;
    case CLICK_TO_FOCUS_MOUSE_PRECEDENT:
        m_ui->windowFocusPolicyDescriptionLabel->setText(i18n("<em>Click to focus (mouse precedence):</em> Mostly the same as <em>Click to focus</em>. If an active window has to be chosen by the system (eg. because the currently active one was closed) the window under the mouse is the preferred candidate. Unusual, but possible variant of <em>Click to focus</em>."));
        break;
    case FOCUS_FOLLOWS_MOUSE:
        m_ui->windowFocusPolicyDescriptionLabel->setText(i18n("<em>Focus follows mouse:</em> Moving the mouse onto a window will activate it. Eg. windows randomly appearing under the mouse will not gain the focus. <em>Focus stealing prevention</em> takes place as usual. Think as <em>Click to focus</em> just without having to actually click."));
        break;
    case FOCUS_FOLLOWS_MOUSE_PRECEDENT:
        m_ui->windowFocusPolicyDescriptionLabel->setText(i18n("This is mostly the same as <em>Focus follows mouse</em>. If an active window has to be chosen by the system (eg. because the currently active one was closed) the window under the mouse is the preferred candidate. Choose this, if you want a hover controlled focus."));
        break;
    case FOCUS_UNDER_MOUSE:
        m_ui->windowFocusPolicyDescriptionLabel->setText(i18n("<em>Focus under mouse:</em> The focus always remains on the window under the mouse.<br/><strong>Warning:</strong> <em>Focus stealing prevention</em> and the <em>tabbox ('Alt+Tab')</em> contradict the activation policy and will not work. You very likely want to use <em>Focus follows mouse (mouse precedence)</em> instead!"));
        break;
    case FOCUS_STRICTLY_UNDER_MOUSE:
        m_ui->windowFocusPolicyDescriptionLabel->setText(i18n("<em>Focus strictly under mouse:</em> The focus is always on the window under the mouse (in doubt nowhere) very much like the focus behavior in an unmanaged legacy X11 environment.<br/><strong>Warning:</strong> <em>Focus stealing prevention</em> and the <em>tabbox ('Alt+Tab')</em> contradict the activation policy and will not work. You very likely want to use <em>Focus follows mouse (mouse precedence)</em> instead!"));
        break;
    }
}

void KFocusConfig::focusPolicyChanged()
{
    int selectedFocusPolicy = 0;
    bool selectedNextFocusPrefersMouseItem = false;
    const bool loadedNextFocusPrefersMouseItem = m_settings->nextFocusPrefersMouse();

    updateFocusPolicyExplanatoryText();

    int focusPolicy = m_ui->windowFocusPolicy->currentIndex();
    switch (focusPolicy) {
    case CLICK_TO_FOCUS:
        selectedFocusPolicy = NWinOptionsSettings::EnumFocusPolicy::ClickToFocus;
        break;
    case CLICK_TO_FOCUS_MOUSE_PRECEDENT:
        selectedFocusPolicy = NWinOptionsSettings::EnumFocusPolicy::ClickToFocus;
        selectedNextFocusPrefersMouseItem = true;
        break;
    case FOCUS_FOLLOWS_MOUSE:
        selectedFocusPolicy = NWinOptionsSettings::EnumFocusPolicy::FocusFollowsMouse;
        break;
    case FOCUS_FOLLOWS_MOUSE_PRECEDENT:
        selectedFocusPolicy = NWinOptionsSettings::EnumFocusPolicy::FocusFollowsMouse;
        selectedNextFocusPrefersMouseItem = true;
        break;
    case FOCUS_UNDER_MOUSE:
        selectedFocusPolicy = NWinOptionsSettings::EnumFocusPolicy::FocusUnderMouse;
        break;
    case FOCUS_STRICTLY_UNDER_MOUSE:
        selectedFocusPolicy = NWinOptionsSettings::EnumFocusPolicy::FocusStrictlyUnderMouse;
        break;
    }

    unmanagedWidgetChangeState(m_settings->focusPolicy() != selectedFocusPolicy || loadedNextFocusPrefersMouseItem != selectedNextFocusPrefersMouseItem);

    unmanagedWidgetDefaultState(focusPolicy == defaultFocusPolicyIndex);

    // the auto raise related widgets are: autoRaise
    m_ui->kcfg_AutoRaise->setEnabled(focusPolicy != CLICK_TO_FOCUS && focusPolicy != CLICK_TO_FOCUS_MOUSE_PRECEDENT);

    m_ui->kcfg_FocusStealingPreventionLevel->setDisabled(focusPolicy == FOCUS_UNDER_MOUSE || focusPolicy == FOCUS_STRICTLY_UNDER_MOUSE);

    // the delayed focus related widgets are: delayFocus
    m_ui->delayFocusOnLabel->setEnabled(focusPolicy != CLICK_TO_FOCUS);
    m_ui->kcfg_DelayFocusInterval->setEnabled(focusPolicy != CLICK_TO_FOCUS);
}

void KFocusConfig::load(void)
{
    KCModule::load();

    const bool loadedNextFocusPrefersMouseItem = m_settings->nextFocusPrefersMouse();

    int focusPolicy = m_settings->focusPolicy();

    switch (focusPolicy) {
    // the ClickToFocus and FocusFollowsMouse have special values when
    // NextFocusPrefersMouse is true
    case NWinOptionsSettings::EnumFocusPolicy::ClickToFocus:
        m_ui->windowFocusPolicy->setCurrentIndex(CLICK_TO_FOCUS + loadedNextFocusPrefersMouseItem);
        break;
    case NWinOptionsSettings::EnumFocusPolicy::FocusFollowsMouse:
        m_ui->windowFocusPolicy->setCurrentIndex(FOCUS_FOLLOWS_MOUSE + loadedNextFocusPrefersMouseItem);
        break;
    default:
        // +2 to ignore the two special values
        m_ui->windowFocusPolicy->setCurrentIndex(focusPolicy + 2);
        break;
    }
    updateFocusPolicyExplanatoryText();
}

void KFocusConfig::save(void)
{
    KCModule::save();

    int idxFocusPolicy = m_ui->windowFocusPolicy->currentIndex();
    switch (idxFocusPolicy) {
    case CLICK_TO_FOCUS:
    case CLICK_TO_FOCUS_MOUSE_PRECEDENT:
        m_settings->setFocusPolicy(NWinOptionsSettings::EnumFocusPolicy::ClickToFocus);
        break;
    case FOCUS_FOLLOWS_MOUSE:
    case FOCUS_FOLLOWS_MOUSE_PRECEDENT:
        // the ClickToFocus and FocusFollowsMouse have special values when
        // NextFocusPrefersMouse is true
        m_settings->setFocusPolicy(NWinOptionsSettings::EnumFocusPolicy::FocusFollowsMouse);
        break;
    case FOCUS_UNDER_MOUSE:
        m_settings->setFocusPolicy(NWinOptionsSettings::EnumFocusPolicy::FocusUnderMouse);
        break;
    case FOCUS_STRICTLY_UNDER_MOUSE:
        m_settings->setFocusPolicy(NWinOptionsSettings::EnumFocusPolicy::FocusStrictlyUnderMouse);
        break;
    }
    m_settings->setNextFocusPrefersMouse(idxFocusPolicy == CLICK_TO_FOCUS_MOUSE_PRECEDENT || idxFocusPolicy == FOCUS_FOLLOWS_MOUSE_PRECEDENT);

    m_settings->save();
}

void KFocusConfig::defaults()
{
    KCModule::defaults();
    m_ui->windowFocusPolicy->setCurrentIndex(defaultFocusPolicyIndex);
}

NWinAdvancedConfigForm::NWinAdvancedConfigForm(QWidget *parent)
    : QWidget(parent)
{
    setupUi(parent);
}

NWinPipConfigForm::NWinPipConfigForm(QWidget *parent)
    : QWidget(parent)
{
    setupUi(parent);
}

KAdvancedConfig::KAdvancedConfig(NWinOptionsSettings *settings, NWinOptionsKDEGlobalsSettings *globalSettings, QWidget *parent)
    : KCModule(parent, KPluginMetaData())
    , m_ui(new NWinAdvancedConfigForm(widget()))
{
    if (settings && globalSettings) {
        initialize(settings, globalSettings);
    }
}

void KAdvancedConfig::initialize(NWinOptionsSettings *settings, NWinOptionsKDEGlobalsSettings *globalSettings)
{
    m_settings = settings;
    addConfig(m_settings, widget());
    addConfig(globalSettings, widget());

    m_ui->kcfg_Placement->setItemData(NWinOptionsSettings::PlacementChoices::Smart, "Smart");
    m_ui->kcfg_Placement->setItemData(NWinOptionsSettings::PlacementChoices::Maximizing, "Maximizing");
    m_ui->kcfg_Placement->setItemData(NWinOptionsSettings::PlacementChoices::Random, "Random");
    m_ui->kcfg_Placement->setItemData(NWinOptionsSettings::PlacementChoices::Centered, "Centered");
    m_ui->kcfg_Placement->setItemData(NWinOptionsSettings::PlacementChoices::ZeroCornered, "ZeroCornered");
    m_ui->kcfg_Placement->setItemData(NWinOptionsSettings::PlacementChoices::UnderMouse, "UnderMouse");

    // Don't show the option to prevent apps from remembering their window
    // positions on Wayland because it doesn't work on Wayland and the feature
    // will eventually be implemented in a different way there.
    // This option lives in the kdeglobals file because it is consumed by
    // kxmlgui.
    m_ui->kcfg_AllowKDEAppsToRememberWindowPositions->setVisible(KWindowSystem::isPlatformX11());

    m_ui->kcfg_ActivationDesktopPolicy->setItemData(NWinOptionsSettings::ActivationDesktopPolicyChoices::SwitchToOtherDesktop, "SwitchToOtherDesktop");
    m_ui->kcfg_ActivationDesktopPolicy->setItemData(NWinOptionsSettings::ActivationDesktopPolicyChoices::BringToCurrentDesktop, "BringToCurrentDesktop");
}

NWinMovingConfigForm::NWinMovingConfigForm(QWidget *parent)
    : QWidget(parent)
{
    setupUi(parent);
}

KMovingConfig::KMovingConfig(NWinOptionsSettings *settings, QWidget *parent)
    : KCModule(parent, KPluginMetaData())
    , m_ui(new NWinMovingConfigForm(widget()))
{
    if (settings) {
        initialize(settings);
    }
}

void KMovingConfig::initialize(NWinOptionsSettings *settings)
{
    m_settings = settings;
    addConfig(m_settings, widget());
}

KPipConfig::KPipConfig(NWinOptionsSettings *settings, QWidget *parent)
    : KCModule(parent, KPluginMetaData())
    , m_ui(new NWinPipConfigForm(widget()))
{
    if (settings) {
        initialize(settings);
    }
}

void KPipConfig::initialize(NWinOptionsSettings *settings)
{
    m_settings = settings;
    addConfig(m_settings, widget());
}

#include "moc_windows.cpp"
