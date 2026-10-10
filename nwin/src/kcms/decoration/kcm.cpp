/*
    SPDX-FileCopyrightText: 2019 Valerio Pilo <vpilo@coldshock.net>
    SPDX-FileCopyrightText: 2019 Cyril Rossi <cyril.rossi@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/
#include "kcm.h"

#include "config-nwin.h"

#include "declarative-plugin/buttonsmodel.h"
#include "decorationmodel.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KPluginFactory>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSortFilterProxyModel>

#include "nwindecorationdata.h"
#include "nwindecorationsettings.h"
#include "utils.h"

K_PLUGIN_FACTORY_WITH_JSON(KCMNWinDecorationFactory, "kcm_nwindecoration.json", registerPlugin<KCMNWinDecoration>(); registerPlugin<NWinDecorationData>();)

Q_DECLARE_METATYPE(KDecoration3::BorderSize)

namespace
{

const KDecoration3::BorderSize s_defaultRecommendedBorderSize = KDecoration3::BorderSize::Normal;

}

KCMNWinDecoration::KCMNWinDecoration(QObject *parent, const KPluginMetaData &metaData)
    : KQuickManagedConfigModule(parent, metaData)
    , m_themesModel(new KDecoration3::Configuration::DecorationsModel(this))
    , m_proxyThemesModel(new QSortFilterProxyModel(this))
    , m_leftButtonsModel(new KDecoration3::Preview::ButtonsModel(DecorationButtonsList(), this))
    , m_rightButtonsModel(new KDecoration3::Preview::ButtonsModel(DecorationButtonsList(), this))
    , m_availableButtonsModel(new KDecoration3::Preview::ButtonsModel(this))
    , m_data(new NWinDecorationData(this))
    , m_excludeFromCaptureButtonSelected(isExcludeFromCaptureSelected())
{
    setButtons(Apply | Default | Help);
    qmlRegisterAnonymousType<QAbstractListModel>("org.kde.nwin.NWinDecoration", 1);
    qmlRegisterAnonymousType<QSortFilterProxyModel>("org.kde.nwin.NWinDecoration", 1);
    qmlRegisterAnonymousType<NWinDecorationSettings>("org.kde.nwin.NWinDecoration", 1);
    m_proxyThemesModel->setSourceModel(m_themesModel);
    m_proxyThemesModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyThemesModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_proxyThemesModel->sort(0);

    connect(m_proxyThemesModel, &QSortFilterProxyModel::rowsInserted, this, &KCMNWinDecoration::themeChanged);
    connect(m_proxyThemesModel, &QSortFilterProxyModel::rowsRemoved, this, &KCMNWinDecoration::themeChanged);
    connect(m_proxyThemesModel, &QSortFilterProxyModel::modelReset, this, &KCMNWinDecoration::themeChanged);

    connect(m_data->settings(), &NWinDecorationSettings::themeChanged, this, &KCMNWinDecoration::themeChanged);
    connect(m_data->settings(), &NWinDecorationSettings::borderSizeChanged, this, &KCMNWinDecoration::borderSizeChanged);

    connect(m_data->settings(), &NWinDecorationSettings::borderSizeAutoChanged, this, &KCMNWinDecoration::borderIndexChanged);
    connect(this, &KCMNWinDecoration::borderSizeChanged, this, &KCMNWinDecoration::borderIndexChanged);
    connect(this, &KCMNWinDecoration::themeChanged, this, &KCMNWinDecoration::borderIndexChanged);

    connect(this, &KCMNWinDecoration::themeChanged, this, [this]() {
        if (m_data->settings()->borderSizeAuto()) {
            setBorderSize(recommendedBorderSize());
        }
    });

    connect(m_leftButtonsModel, &QAbstractItemModel::rowsInserted, this, &KCMNWinDecoration::onLeftButtonsChanged);
    connect(m_leftButtonsModel, &QAbstractItemModel::rowsMoved, this, &KCMNWinDecoration::onLeftButtonsChanged);
    connect(m_leftButtonsModel, &QAbstractItemModel::rowsRemoved, this, &KCMNWinDecoration::onLeftButtonsChanged);
    connect(m_leftButtonsModel, &QAbstractItemModel::modelReset, this, &KCMNWinDecoration::onLeftButtonsChanged);

    connect(m_rightButtonsModel, &QAbstractItemModel::rowsInserted, this, &KCMNWinDecoration::onRightButtonsChanged);
    connect(m_rightButtonsModel, &QAbstractItemModel::rowsMoved, this, &KCMNWinDecoration::onRightButtonsChanged);
    connect(m_rightButtonsModel, &QAbstractItemModel::rowsRemoved, this, &KCMNWinDecoration::onRightButtonsChanged);
    connect(m_rightButtonsModel, &QAbstractItemModel::modelReset, this, &KCMNWinDecoration::onRightButtonsChanged);

    connect(m_data->settings(), &NWinDecorationSettings::buttonsOnLeftChanged, this, &KCMNWinDecoration::checkExcludeFromCaptureButtonPresence);
    connect(m_data->settings(), &NWinDecorationSettings::buttonsOnRightChanged, this, &KCMNWinDecoration::checkExcludeFromCaptureButtonPresence);

    connect(this, &KCMNWinDecoration::borderSizeChanged, this, &KCMNWinDecoration::settingsChanged);

    // Update the themes when the color scheme or a theme's settings change
    QDBusConnection::sessionBus()
        .connect(QString(), QStringLiteral("/NWin"), QStringLiteral("org.kde.NWin"), QStringLiteral("reloadConfig"),
                 this, SLOT(reloadNWinSettings()));

    QMetaObject::invokeMethod(m_themesModel, &KDecoration3::Configuration::DecorationsModel::init, Qt::QueuedConnection);
}

NWinDecorationSettings *KCMNWinDecoration::settings() const
{
    return m_data->settings();
}

void KCMNWinDecoration::reloadNWinSettings()
{
    QMetaObject::invokeMethod(m_themesModel, &KDecoration3::Configuration::DecorationsModel::init, Qt::QueuedConnection);
}

void KCMNWinDecoration::load()
{
    KQuickManagedConfigModule::load();

    m_leftButtonsModel->replace(Utils::buttonsFromString(settings()->buttonsOnLeft()));
    m_rightButtonsModel->replace(Utils::buttonsFromString(settings()->buttonsOnRight()));

    setBorderSize(borderSizeIndexFromString(settings()->borderSize()));

    Q_EMIT themeChanged();
}

void KCMNWinDecoration::save()
{
    if (!settings()->borderSizeAuto()) {
        settings()->setBorderSize(borderSizeIndexToString(m_borderSizeIndex));
    } else {
        settings()->setBorderSize(settings()->defaultBorderSizeValue());
    }

    KQuickManagedConfigModule::save();

    // Send a signal to all nwin instances
    QDBusMessage message = QDBusMessage::createSignal(QStringLiteral("/NWin"),
                                                      QStringLiteral("org.kde.NWin"),
                                                      QStringLiteral("reloadConfig"));
    QDBusConnection::sessionBus().send(message);
}

void KCMNWinDecoration::defaults()
{
    KQuickManagedConfigModule::defaults();

    setBorderSize(recommendedBorderSize());

    m_leftButtonsModel->replace(Utils::buttonsFromString(settings()->buttonsOnLeft()));
    m_rightButtonsModel->replace(Utils::buttonsFromString(settings()->buttonsOnRight()));
}

void KCMNWinDecoration::onLeftButtonsChanged()
{
    settings()->setButtonsOnLeft(Utils::buttonsToString(m_leftButtonsModel->buttons()));
}

void KCMNWinDecoration::onRightButtonsChanged()
{
    settings()->setButtonsOnRight(Utils::buttonsToString(m_rightButtonsModel->buttons()));
}

QSortFilterProxyModel *KCMNWinDecoration::themesModel() const
{
    return m_proxyThemesModel;
}

QAbstractListModel *KCMNWinDecoration::leftButtonsModel()
{
    return m_leftButtonsModel;
}

QAbstractListModel *KCMNWinDecoration::rightButtonsModel()
{
    return m_rightButtonsModel;
}

QAbstractListModel *KCMNWinDecoration::availableButtonsModel() const
{
    return m_availableButtonsModel;
}

bool KCMNWinDecoration::excludeFromCaptureButtonSelected() const
{
    return m_excludeFromCaptureButtonSelected;
}

QStringList KCMNWinDecoration::borderSizesModel() const
{
    // Use index 0 for borderSizeAuto == true
    // The rest of indexes get offset by 1
    QStringList model = Utils::getBorderSizeNames().values();
    model.insert(0, i18nc("%1 is the name of a border size", "Theme default (%1)", model.at(recommendedBorderSize())));
    return model;
}

int KCMNWinDecoration::borderIndex() const
{
    return settings()->borderSizeAuto() ? 0 : m_borderSizeIndex + 1;
}

void KCMNWinDecoration::setBorderIndex(int index)
{
    const bool borderAuto = (index == 0);
    settings()->setBorderSizeAuto(borderAuto);
    setBorderSize(borderAuto ? recommendedBorderSize() : index - 1);
}

int KCMNWinDecoration::borderSize() const
{
    return m_borderSizeIndex;
}

int KCMNWinDecoration::recommendedBorderSize() const
{
    typedef KDecoration3::Configuration::DecorationsModel::DecorationRole DecoRole;
    const QModelIndex proxyIndex = m_proxyThemesModel->index(theme(), 0);
    if (proxyIndex.isValid()) {
        const QModelIndex index = m_proxyThemesModel->mapToSource(proxyIndex);
        if (index.isValid()) {
            QVariant ret = m_themesModel->data(index, DecoRole::RecommendedBorderSizeRole);
            return Utils::getBorderSizeNames().keys().indexOf(Utils::stringToBorderSize(ret.toString()));
        }
    }
    return Utils::getBorderSizeNames().keys().indexOf(s_defaultRecommendedBorderSize);
}

int KCMNWinDecoration::theme() const
{
    return m_proxyThemesModel->mapFromSource(m_themesModel->findDecoration(settings()->pluginName(), settings()->theme())).row();
}

void KCMNWinDecoration::setBorderSize(int index)
{
    if (m_borderSizeIndex != index) {
        m_borderSizeIndex = index;
        Q_EMIT borderSizeChanged();
    }
}

void KCMNWinDecoration::setBorderSize(KDecoration3::BorderSize size)
{
    settings()->setBorderSize(Utils::borderSizeToString(size));
}

void KCMNWinDecoration::setTheme(int index)
{
    QModelIndex dataIndex = m_proxyThemesModel->index(index, 0);
    if (dataIndex.isValid()) {
        settings()->setTheme(m_proxyThemesModel->data(dataIndex, KDecoration3::Configuration::DecorationsModel::ThemeNameRole).toString());
        settings()->setPluginName(m_proxyThemesModel->data(dataIndex, KDecoration3::Configuration::DecorationsModel::PluginNameRole).toString());
        Q_EMIT themeChanged();
    }
}

bool KCMNWinDecoration::isSaveNeeded() const
{
    return !settings()->borderSizeAuto() && borderSizeIndexFromString(settings()->borderSize()) != m_borderSizeIndex;
}

int KCMNWinDecoration::borderSizeIndexFromString(const QString &size) const
{
    return Utils::getBorderSizeNames().keys().indexOf(Utils::stringToBorderSize(size));
}

QString KCMNWinDecoration::borderSizeIndexToString(int index) const
{
    return Utils::borderSizeToString(Utils::getBorderSizeNames().keys().at(index));
}

bool KCMNWinDecoration::isExcludeFromCaptureSelected() const
{
    const auto left = Utils::buttonsFromString(settings()->buttonsOnLeft());
    const auto right = Utils::buttonsFromString(settings()->buttonsOnRight());
    const auto excludeFromCapture = KDecoration3::DecorationButtonType::ExcludeFromCapture;
    return left.contains(excludeFromCapture) || right.contains(excludeFromCapture);
}

void KCMNWinDecoration::checkExcludeFromCaptureButtonPresence()
{
    const bool selected = isExcludeFromCaptureSelected();
    if (m_excludeFromCaptureButtonSelected == selected) {
        return;
    }

    m_excludeFromCaptureButtonSelected = selected;
    Q_EMIT excludeFromCaptureButtonSelectedChanged();
}

#include "kcm.moc"
#include "moc_kcm.cpp"
