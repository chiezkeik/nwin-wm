/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2009 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2020 Cyril Rossi <cyril.rossi@enioka.com>
    SPDX-FileCopyrightText: 2023 Ismael Asensio <isma.af@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "main.h"

#include "config-nwin.h"

#include <nwin_effects_interface.h>

// Qt
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QPointer>
#include <QProcess>
#include <QPushButton>
#include <QSpacerItem>
#include <QStandardPaths>
#include <QStyle>
#include <QTabWidget>
#include <QVBoxLayout>

// KDE
#include <KLocalizedString>
#include <KNSWidgets/Button>
#include <KPluginFactory>
#include <KSeparator>
#include <KTitleWidget>
// Plasma
#include <KPackage/Package>
#include <KPackage/PackageLoader>

// own
#include "nwintabboxconfigform.h"
#include "nwintabboxdata.h"
#include "nwintabboxsettings.h"
#include "shortcutsettings.h"

#include <QTabBar>

K_PLUGIN_FACTORY_WITH_JSON(NWinTabBoxConfigFactory, "kcm_nwintabbox.json", registerPlugin<NWin::NWinTabBoxConfig>(); registerPlugin<NWin::TabBox::NWinTabboxData>();)

namespace NWin
{

using namespace TabBox;

NWinTabBoxConfig::NWinTabBoxConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
    , m_config(KSharedConfig::openConfig("nwinrc"))
    , m_data(new NWinTabboxData(this))
{
    QTabWidget *tabWidget = new QTabWidget(widget());
    tabWidget->setDocumentMode(true);
    tabWidget->tabBar()->setExpanding(true);

    m_primaryTabBoxUi = new NWinTabBoxConfigForm(NWinTabBoxConfigForm::TabboxType::Main,
                                                 m_data->tabBoxConfig(),
                                                 m_data->shortcutConfig(),
                                                 tabWidget);
    m_alternativeTabBoxUi = new NWinTabBoxConfigForm(NWinTabBoxConfigForm::TabboxType::Alternative,
                                                     m_data->tabBoxAlternativeConfig(),
                                                     m_data->shortcutConfig(),
                                                     tabWidget);
    tabWidget->addTab(m_primaryTabBoxUi, i18n("Main"));
    tabWidget->addTab(m_alternativeTabBoxUi, i18n("Alternative"));

    KNSWidgets::Button *ghnsButton = new KNSWidgets::Button(i18n("Get New Task Switcher Styles…"), QStringLiteral("nwinswitcher.knsrc"), widget());
    connect(ghnsButton, &KNSWidgets::Button::dialogFinished, this, [this](auto changedEntries) {
        if (!changedEntries.isEmpty()) {
            initLayoutLists();
        }
    });

    QHBoxLayout *buttonBar = new QHBoxLayout();
    QStyle *style = widget()->style();

    buttonBar->setContentsMargins(style->pixelMetric(QStyle::PM_LayoutLeftMargin), 0, style->pixelMetric(QStyle::PM_LayoutRightMargin), style->pixelMetric(QStyle::PM_LayoutBottomMargin));
    QSpacerItem *buttonBarSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonBar->addItem(buttonBarSpacer);
    buttonBar->addWidget(ghnsButton);

    QVBoxLayout *layout = new QVBoxLayout(widget());
    layout->setContentsMargins(0, 0, 0, 0);
    KTitleWidget *infoLabel = new KTitleWidget(tabWidget);
    infoLabel->setText(i18n("Focus policy settings limit the functionality of navigating through windows."),
                       KTitleWidget::InfoMessage);
    infoLabel->setIcon(KTitleWidget::InfoMessage, KTitleWidget::ImageLeft);
    layout->addWidget(infoLabel, 0);
    layout->addWidget(tabWidget, 1);
    KSeparator *separator = new KSeparator();
    layout->addWidget(separator);
    layout->addLayout(buttonBar);
    widget()->setLayout(layout);

    // Hide the separator if KNS is disabled.
    separator->setVisible(!ghnsButton->isHidden());

    addConfig(m_data->tabBoxConfig(), m_primaryTabBoxUi);
    addConfig(m_data->tabBoxAlternativeConfig(), m_alternativeTabBoxUi);

    initLayoutLists();

    createConnections(m_primaryTabBoxUi);
    createConnections(m_alternativeTabBoxUi);

    // check focus policy - we don't offer configs for unreasonable focus policies
    KConfigGroup config(m_config, QStringLiteral("Windows"));
    QString policy = config.readEntry("FocusPolicy", "ClickToFocus");
    if ((policy == "FocusUnderMouse") || (policy == "FocusStrictlyUnderMouse")) {
        tabWidget->setEnabled(false);
        infoLabel->show();
    } else {
        infoLabel->hide();
    }
}

NWinTabBoxConfig::~NWinTabBoxConfig()
{
}

static QList<KPackage::Package> availableLnFPackages()
{
    QList<KPackage::Package> packages;
    QStringList paths;
    const QStringList dataPaths = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);

    for (const QString &path : dataPaths) {
        QDir dir(path + QLatin1StringView("/plasma/look-and-feel"));
        paths << dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    }

    const auto &p = paths;
    for (const QString &path : p) {
        KPackage::Package pkg = KPackage::PackageLoader::self()->loadPackage(QStringLiteral("Plasma/LookAndFeel"));
        pkg.setPath(path);
        pkg.setFallbackPackage(KPackage::Package());
        if (!pkg.filePath("defaults").isEmpty()) {
            KSharedConfigPtr conf = KSharedConfig::openConfig(pkg.filePath("defaults"));
            KConfigGroup cg = KConfigGroup(conf, QStringLiteral("nwinrc"));
            cg = KConfigGroup(&cg, QStringLiteral("WindowSwitcher"));
            if (!cg.readEntry("LayoutName", QString()).isEmpty()) {
                packages << pkg;
            }
        }
    }

    return packages;
}

void NWinTabBoxConfig::initLayoutLists()
{
    auto model = std::make_unique<QStandardItemModel>();

    auto addToModel = [model = model.get()](const QString &name, const QString &pluginId, const QString &path) {
        QStandardItem *item = new QStandardItem(name);
        item->setData(pluginId, Qt::UserRole);
        item->setData(path, NWinTabBoxConfigForm::LayoutPath);
        model->appendRow(item);
    };

    const auto lnfPackages = availableLnFPackages();
    for (const auto &package : lnfPackages) {
        const auto &metaData = package.metadata();
        const QString switcherFile = package.filePath("windowswitcher", QStringLiteral("WindowSwitcher.qml"));
        if (switcherFile.isEmpty()) {
            // Skip lnfs that don't actually ship a switcher
            continue;
        }

        addToModel(metaData.name(), metaData.pluginId(), switcherFile);
    }

    const QStringList packageRoots{
        QStringLiteral("nwin-wayland/tabbox"),
        QStringLiteral("nwin/tabbox"),
    };
    for (const QString &packageRoot : packageRoots) {
        const QList<KPluginMetaData> offers = KPackage::PackageLoader::self()->listPackages(QStringLiteral("NWin/WindowSwitcher"), packageRoot);
        for (const auto &offer : offers) {
            const QString pluginName = offer.pluginId();
            const QString scriptFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                              packageRoot + QLatin1Char('/') + pluginName + QLatin1StringView("/contents/ui/main.qml"));
            if (scriptFile.isEmpty()) {
                qWarning() << "scriptfile is null" << pluginName;
                continue;
            }

            addToModel(offer.name(), pluginName, scriptFile);
        }
    }

    model->sort(0);

    m_primaryTabBoxUi->setEffectComboModel(model.get());
    m_alternativeTabBoxUi->setEffectComboModel(model.get());

    m_switcherModel = std::move(model);
}

void NWinTabBoxConfig::createConnections(NWinTabBoxConfigForm *form)
{
    connect(form, &NWinTabBoxConfigForm::effectPreviewClicked, this, &NWinTabBoxConfig::showPreview);
    connect(form, &NWinTabBoxConfigForm::configChanged, this, &NWinTabBoxConfig::updateUnmanagedState);

    connect(this, &NWinTabBoxConfig::defaultsIndicatorsVisibleChanged, form, [form, this]() {
        form->setDefaultIndicatorVisible(defaultsIndicatorsVisible());
    });
}

void NWinTabBoxConfig::updateUnmanagedState()
{
    const bool isNeedSave = m_data->tabBoxConfig()->isSaveNeeded()
        || m_data->tabBoxAlternativeConfig()->isSaveNeeded()
        || m_data->shortcutConfig()->isSaveNeeded();

    unmanagedWidgetChangeState(isNeedSave);

    const bool isDefault = m_data->tabBoxConfig()->isDefaults()
        && m_data->tabBoxAlternativeConfig()->isDefaults()
        && m_data->shortcutConfig()->isDefaults();

    unmanagedWidgetDefaultState(isDefault);
}

void NWinTabBoxConfig::load()
{
    KCModule::load();

    m_data->tabBoxConfig()->load();
    m_data->tabBoxAlternativeConfig()->load();
    m_data->shortcutConfig()->load();

    m_primaryTabBoxUi->updateUiFromConfig();
    m_alternativeTabBoxUi->updateUiFromConfig();

    updateUnmanagedState();
}

void NWinTabBoxConfig::save()
{
    m_data->tabBoxConfig()->save();
    m_data->tabBoxAlternativeConfig()->save();
    m_data->shortcutConfig()->save();

    KCModule::save();
    updateUnmanagedState();

    // Reload NWin.
    QDBusMessage message = QDBusMessage::createSignal("/NWin", "org.kde.NWin", "reloadConfig");
    QDBusConnection::sessionBus().send(message);
}

void NWinTabBoxConfig::defaults()
{
    m_data->tabBoxConfig()->setDefaults();
    m_data->tabBoxAlternativeConfig()->setDefaults();
    m_data->shortcutConfig()->setDefaults();

    m_primaryTabBoxUi->updateUiFromConfig();
    m_alternativeTabBoxUi->updateUiFromConfig();

    KCModule::defaults();
    updateUnmanagedState();
}

void NWinTabBoxConfig::showPreview()
{
    auto form = qobject_cast<NWinTabBoxConfigForm *>(sender());
    Q_ASSERT(form);

    // The process will close when losing focus, but check in case of multiple calls
    if (m_previewProcess && m_previewProcess->state() != QProcess::NotRunning) {
        return;
    }

    // Launch the preview helper executable with the required env var
    // that allows the PlasmaDialog to position itself
    // QT_WAYLAND_DISABLE_FIXED_POSITIONS=1 nwin-tabbox-preview <path> [--show-desktop]

    const QString previewHelper = QStandardPaths::findExecutable("nwin-tabbox-preview", {LIBEXEC_DIR});
    if (previewHelper.isEmpty()) {
        qWarning() << "Cannot find tabbox preview helper executable \"nwin-tabbox-preview\" in" << LIBEXEC_DIR;
        return;
    }

    QStringList args;
    args << form->effectComboCurrentData(NWinTabBoxConfigForm::LayoutPath).toString();
    if (form->config()->showDesktopMode()) {
        args << QStringLiteral("--show-desktop");
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(QStringLiteral("QT_WAYLAND_DISABLE_FIXED_POSITIONS"),
               QStringLiteral("1"));

    m_previewProcess = std::make_unique<QProcess>();
    m_previewProcess->setArguments(args);
    m_previewProcess->setProgram(previewHelper);
    m_previewProcess->setProcessEnvironment(env);
    m_previewProcess->setProcessChannelMode(QProcess::ForwardedChannels);
    m_previewProcess->start();
}

} // namespace

#include "main.moc"

#include "moc_main.cpp"
