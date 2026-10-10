/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2009 Lucas Murray <lmurray@undefinedfire.com>
    SPDX-FileCopyrightText: 2020 Cyril Rossi <cyril.rossi@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "nwinscreenedgeconfigform.h"
#include "ui_main.h"

namespace NWin
{

NWinScreenEdgesConfigForm::NWinScreenEdgesConfigForm(QWidget *parent)
    : NWinScreenEdge(parent)
    , ui(std::make_unique<Ui::NWinScreenEdgesConfigUI>())
{
    ui->setupUi(this);

    connect(ui->kcfg_ElectricBorderDelay, qOverload<int>(&QSpinBox::valueChanged), this, &NWinScreenEdgesConfigForm::sanitizeCooldown);

    // Visual feedback of action group conflicts
    connect(ui->kcfg_ElectricBorders, qOverload<int>(&QComboBox::currentIndexChanged), this, &NWinScreenEdgesConfigForm::groupChanged);
    connect(ui->kcfg_ElectricBorderMaximize, &QCheckBox::stateChanged, this, &NWinScreenEdgesConfigForm::groupChanged);
    connect(ui->kcfg_ElectricBorderTiling, &QCheckBox::stateChanged, this, &NWinScreenEdgesConfigForm::groupChanged);

    connect(ui->remainActiveOnFullscreen, &QCheckBox::stateChanged, this, &NWinScreenEdgesConfigForm::onChanged);
    connect(ui->electricBorderCornerRatioSpin, qOverload<int>(&QSpinBox::valueChanged), this, &NWinScreenEdgesConfigForm::onChanged);
    connect(ui->electricBorderCornerRatioSpin, qOverload<int>(&QSpinBox::valueChanged), this, &NWinScreenEdgesConfigForm::updateDefaultIndicators);
}

NWinScreenEdgesConfigForm::~NWinScreenEdgesConfigForm() = default;

void NWinScreenEdgesConfigForm::setElectricBorderCornerRatio(double value)
{
    m_referenceCornerRatio = value;
    ui->electricBorderCornerRatioSpin->setValue(m_referenceCornerRatio * 100.);
}

void NWinScreenEdgesConfigForm::setDefaultElectricBorderCornerRatio(double value)
{
    m_defaultCornerRatio = value;
    updateDefaultIndicators();
}

void NWinScreenEdgesConfigForm::setRemainActiveOnFullscreen(bool remainActive)
{
    m_remainActiveOnFullscreen = remainActive;
    ui->remainActiveOnFullscreen->setChecked(remainActive);
    updateDefaultIndicators();
}

double NWinScreenEdgesConfigForm::electricBorderCornerRatio() const
{
    return ui->electricBorderCornerRatioSpin->value() / 100.;
}

void NWinScreenEdgesConfigForm::setElectricBorderCornerRatioEnabled(bool enable)
{
    ui->electricBorderCornerRatioSpin->setEnabled(enable);
}

void NWinScreenEdgesConfigForm::reload()
{
    ui->electricBorderCornerRatioSpin->setValue(m_referenceCornerRatio * 100.);
    NWinScreenEdge::reload();
}

void NWinScreenEdgesConfigForm::setDefaults()
{
    ui->electricBorderCornerRatioSpin->setValue(m_defaultCornerRatio * 100.);
    ui->remainActiveOnFullscreen->setChecked(false);
    NWinScreenEdge::setDefaults();
}

void NWinScreenEdgesConfigForm::setDefaultsIndicatorsVisible(bool visible)
{
    if (m_defaultIndicatorVisible != visible) {
        m_defaultIndicatorVisible = visible;
        updateDefaultIndicators();
    }
}

bool NWinScreenEdgesConfigForm::remainActiveOnFullscreen() const
{
    return ui->remainActiveOnFullscreen->isChecked();
}

Monitor *NWinScreenEdgesConfigForm::monitor() const
{
    return ui->monitor;
}

bool NWinScreenEdgesConfigForm::isSaveNeeded() const
{
    return m_referenceCornerRatio != electricBorderCornerRatio() || m_remainActiveOnFullscreen != remainActiveOnFullscreen();
}

bool NWinScreenEdgesConfigForm::isDefault() const
{
    return m_defaultCornerRatio == electricBorderCornerRatio() && m_remainActiveOnFullscreen == false;
}

void NWinScreenEdgesConfigForm::sanitizeCooldown()
{
    ui->kcfg_ElectricBorderCooldown->setMinimum(ui->kcfg_ElectricBorderDelay->value() + 50);
}

void NWinScreenEdgesConfigForm::groupChanged()
{
    // Monitor conflicts
    bool hide = false;
    if (ui->kcfg_ElectricBorders->currentIndex() == 2) {
        hide = true;
    }
    monitorHideEdge(ElectricTop, hide);
    monitorHideEdge(ElectricRight, hide);
    monitorHideEdge(ElectricBottom, hide);
    monitorHideEdge(ElectricLeft, hide);
}

void NWinScreenEdgesConfigForm::updateDefaultIndicators()
{
    ui->electricBorderCornerRatioSpin->setProperty("_kde_highlight_neutral", m_defaultIndicatorVisible && (electricBorderCornerRatio() != m_defaultCornerRatio));
    ui->electricBorderCornerRatioSpin->update();
    ui->remainActiveOnFullscreen->setProperty("_kde_highlight_neutral", m_defaultIndicatorVisible && remainActiveOnFullscreen() == true);
    ui->remainActiveOnFullscreen->update();
}

} // namespace

#include "moc_nwinscreenedgeconfigform.cpp"
