/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QAbstractListModel>

namespace NWin
{

class VirtualDesktop;

/*!
 * \qmltype VirtualDesktopModel
 * \inqmlmodule org.kde.nwin
 *
 * \brief Provides a data model for the virtual desktops.
 *
 * This model provides the following roles:
 * \list
 * \li desktop (VirtualDesktop)
 * \endlist
 *
 */
class VirtualDesktopModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int maximum READ maximum CONSTANT)

public:
    enum Role {
        DesktopRole = Qt::UserRole + 1,
    };

    explicit VirtualDesktopModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int maximum() const;

public Q_SLOTS:
    /*!
     * \qmlmethod VirtualDesktop VirtualDesktopModel::create(int position, string name)
     */
    NWin::VirtualDesktop *create(uint position, const QString &name = QString());

    /*!
     * \qmlmethod void VirtualDesktopModel::remove(int position)
     */
    void remove(uint position);

private:
    NWin::VirtualDesktop *desktopFromIndex(const QModelIndex &index) const;

    void handleVirtualDesktopAdded(NWin::VirtualDesktop *desktop);
    void handleVirtualDesktopRemoved(NWin::VirtualDesktop *desktop);
    void handleVirtualDesktopMoved(NWin::VirtualDesktop *desktop, int position);

    QList<NWin::VirtualDesktop *> m_virtualDesktops;
};

} // namespace NWin
