/*
    SPDX-FileCopyrightText: 2020 Aleix Pol Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2026 Xaver Hugl <xaver.hugl@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "virtualkeyboard.h"

#include <KQuickManagedConfigModule>
#include <KService>
#include <QAbstractListModel>

class KDesktopFile;
class VirtualKeyboardData;
class VirtualKeyboardSettings;

class VirtualKeyboardsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        DesktopFileNameRole = Qt::UserRole + 1,
    };
    Q_ENUM(Roles)

    VirtualKeyboardsModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;

    QHash<int, QByteArray> roleNames() const override;

    Q_SCRIPTABLE int inputMethodIndex(const QString &desktopFile) const;

private:
    KService::List m_services;
};

class KcmVirtualKeyboard : public KQuickManagedConfigModule
{
    Q_OBJECT
    Q_PROPERTY(VirtualKeyboardSettings *settings READ settings CONSTANT)
    Q_PROPERTY(QAbstractItemModel *model READ keyboardsModel CONSTANT)
    Q_PROPERTY(NWinVirtualKeyboard *dbusInterface READ dbusInterface CONSTANT)
    Q_PROPERTY(NWinVirtualKeyboard::VirtualKeyboardMode mode READ mode WRITE setMode NOTIFY modeChanged)

public:
    explicit KcmVirtualKeyboard(QObject *parent, const KPluginMetaData &metaData);
    ~KcmVirtualKeyboard() override;

    VirtualKeyboardSettings *settings() const;
    void save() override;
    VirtualKeyboardsModel *keyboardsModel() const
    {
        return m_model;
    }

    NWinVirtualKeyboard *dbusInterface() const
    {
        return m_dbusInterface.get();
    }

    NWinVirtualKeyboard::VirtualKeyboardMode mode() const
    {
        return m_mode;
    }
    void setMode(NWinVirtualKeyboard::VirtualKeyboardMode mode);

Q_SIGNALS:
    void modeChanged(NWinVirtualKeyboard::VirtualKeyboardMode mode);

private:
    VirtualKeyboardData *m_data;
    VirtualKeyboardsModel *const m_model;
    std::unique_ptr<NWinVirtualKeyboard> m_dbusInterface;
    NWinVirtualKeyboard::VirtualKeyboardMode m_mode;
};
