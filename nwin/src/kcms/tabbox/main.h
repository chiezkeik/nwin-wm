/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2009 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2020 Cyril Rossi <cyril.rossi@enioka.com>
    SPDX-FileCopyrightText: 2023 Ismael Asensio <isma.af@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QStandardItemModel>

#include <kcmodule.h>
#include <ksharedconfig.h>

class QProcess;

namespace NWin
{

class NWinTabBoxConfigForm;
namespace TabBox
{

class NWinTabboxData;
class TabBoxSettings;

}

class NWinTabBoxConfig : public KCModule
{
    Q_OBJECT

public:
    explicit NWinTabBoxConfig(QObject *parent, const KPluginMetaData &data);
    ~NWinTabBoxConfig() override;

public Q_SLOTS:
    void save() override;
    void load() override;
    void defaults() override;

private Q_SLOTS:
    void updateUnmanagedState();
    void showPreview();

private:
    void initLayoutLists();
    void createConnections(NWinTabBoxConfigForm *form);

private:
    NWinTabBoxConfigForm *m_primaryTabBoxUi = nullptr;
    NWinTabBoxConfigForm *m_alternativeTabBoxUi = nullptr;
    std::unique_ptr<QStandardItemModel> m_switcherModel;
    KSharedConfigPtr m_config;

    TabBox::NWinTabboxData *m_data;

    std::unique_ptr<QProcess> m_previewProcess;
};

} // namespace
