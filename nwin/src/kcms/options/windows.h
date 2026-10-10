/*
    windows.h

    SPDX-FileCopyrightText: 1997 Patrick Dowler <dowler@morgul.fsh.uvic.ca>
    SPDX-FileCopyrightText: 2001 Waldo Bastian <bastian@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KCModule>
#include <QWidget>

#include "ui_advanced.h"
#include "ui_focus.h"
#include "ui_moving.h"
#include "ui_pip.h"

class QRadioButton;
class QCheckBox;
class QPushButton;
class QGroupBox;
class QLabel;
class QSlider;
class QGroupBox;
class QSpinBox;

class KColorButton;

class NWinOptionsSettings;
class NWinOptionsKDEGlobalsSettings;

class NWinFocusConfigForm : public QWidget, public Ui::NWinFocusConfigForm
{
    Q_OBJECT

public:
    explicit NWinFocusConfigForm(QWidget *parent);
};

class NWinMovingConfigForm : public QWidget, public Ui::NWinMovingConfigForm
{
    Q_OBJECT

public:
    explicit NWinMovingConfigForm(QWidget *parent);
};

class NWinAdvancedConfigForm : public QWidget, public Ui::NWinAdvancedConfigForm
{
    Q_OBJECT

public:
    explicit NWinAdvancedConfigForm(QWidget *parent);
};

class NWinPipConfigForm : public QWidget, public Ui::NWinPipConfigForm
{
    Q_OBJECT

public:
    explicit NWinPipConfigForm(QWidget *parent);
};

class KFocusConfig : public KCModule
{
    Q_OBJECT

public:
    KFocusConfig(NWinOptionsSettings *settings, QWidget *parent);

    void load() override;
    void save() override;
    void defaults() override;

protected:
    void initialize(NWinOptionsSettings *settings);

private Q_SLOTS:
    void focusPolicyChanged();
    void updateMultiScreen();
    void updateDefaultIndicator();

private:
    NWinFocusConfigForm *m_ui;
    NWinOptionsSettings *m_settings;

    void updateFocusPolicyExplanatoryText();
};

class KMovingConfig : public KCModule
{
    Q_OBJECT

public:
    KMovingConfig(NWinOptionsSettings *settings, QWidget *parent);

protected:
    void initialize(NWinOptionsSettings *settings);

private:
    NWinOptionsSettings *m_settings;
    NWinMovingConfigForm *m_ui;
};

class KAdvancedConfig : public KCModule
{
    Q_OBJECT

public:
    KAdvancedConfig(NWinOptionsSettings *settings, NWinOptionsKDEGlobalsSettings *globalSettings, QWidget *parent);

protected:
    void initialize(NWinOptionsSettings *settings, NWinOptionsKDEGlobalsSettings *globalSettings);

private:
    NWinAdvancedConfigForm *m_ui;
    NWinOptionsSettings *m_settings;
};

class KPipConfig : public KCModule
{
    Q_OBJECT

public:
    KPipConfig(NWinOptionsSettings *settings, QWidget *parent);

protected:
    void initialize(NWinOptionsSettings *settings);

private:
    NWinOptionsSettings *m_settings;
    NWinPipConfigForm *m_ui;
};
