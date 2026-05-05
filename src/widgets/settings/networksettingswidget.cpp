/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "networksettingswidget.h"

#include <QMessageBox>
#include <QNetworkProxy>

#include "helpers/clientproxy.h"
#include "services/cryptoservice.h"
#include "services/metricsservice.h"
#include "services/settingsservice.h"
#include "ui_networksettingswidget.h"

NetworkSettingsWidget::NetworkSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::NetworkSettingsWidget) {
    ui->setupUi(this);
}

NetworkSettingsWidget::~NetworkSettingsWidget() { delete ui; }

/**
 * Initializes the network settings (proxy page setup)
 */
void NetworkSettingsWidget::initialize() { setupProxyPage(); }

void NetworkSettingsWidget::readSettings() {
    SettingsService settings;

    const QSignalBlocker blocker(ui->appMetricsCheckBox);
    Q_UNUSED(blocker)
    ui->appMetricsCheckBox->setChecked(
        settings.value(QStringLiteral("appMetrics/disableTracking")).toBool());

    ui->appHeartbeatCheckBox->setChecked(
        settings.value(QStringLiteral("appMetrics/disableAppHeartbeat")).toBool());

    // Default to false to prevent MITM attacks on fresh installs
    bool ignoreSSLErrors =
        settings.value(QStringLiteral("networking/ignoreSSLErrors"), false).toBool();
    ui->ignoreSSLErrorsCheckBox->setChecked(ignoreSSLErrors);
    ui->letsEncryptInfoLabel->setVisible(ignoreSSLErrors);

    // Load the proxy settings
    loadProxySettings();
}

void NetworkSettingsWidget::storeSettings() {
    SettingsService settings;

    if (!settings.value(QStringLiteral("appMetrics/disableTracking")).toBool() &&
        ui->appMetricsCheckBox->isChecked()) {
        MetricsService::instance()->sendVisit(QStringLiteral("settings/app-metrics-disabled"));
    }

    settings.setValue(QStringLiteral("appMetrics/disableTracking"),
                      ui->appMetricsCheckBox->isChecked());

    if (!settings.value(QStringLiteral("appMetrics/disableAppHeartbeat")).toBool() &&
        ui->appHeartbeatCheckBox->isChecked()) {
        MetricsService::instance()->sendVisit(QStringLiteral("settings/app-heartbeat-disabled"));
    }

    settings.setValue(QStringLiteral("appMetrics/disableAppHeartbeat"),
                      ui->appHeartbeatCheckBox->isChecked());

    settings.setValue(QStringLiteral("networking/ignoreSSLErrors"),
                      ui->ignoreSSLErrorsCheckBox->isChecked());

    // Store the proxy settings
    storeProxySettings();
}

void NetworkSettingsWidget::setupProxyPage() {
    ui->hostLineEdit->setPlaceholderText(tr("hostname of proxy server"));
    ui->userLineEdit->setPlaceholderText(tr("username for proxy server"));
    ui->passwordLineEdit->setPlaceholderText(tr("password for proxy server"));

    ui->typeComboBox->addItem(tr("HTTP(S) proxy"), QNetworkProxy::HttpProxy);
    ui->typeComboBox->addItem(tr("SOCKS5 proxy"), QNetworkProxy::Socks5Proxy);

    ui->authRequiredcheckBox->setEnabled(true);

    // Explicitly set up the enabled status of the proxy auth widgets to ensure
    // toggling the parent enables/disables the children
    ui->userLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->authWidgets->setEnabled(ui->authRequiredcheckBox->isChecked());
    connect(ui->authRequiredcheckBox, SIGNAL(toggled(bool)), ui->authWidgets,
            SLOT(setEnabled(bool)));

    connect(ui->manualProxyRadioButton, SIGNAL(toggled(bool)), ui->manualSettings,
            SLOT(setEnabled(bool)));
    connect(ui->manualProxyRadioButton, SIGNAL(toggled(bool)), ui->typeComboBox,
            SLOT(setEnabled(bool)));
}

/**
 * Loads the proxy settings
 */
void NetworkSettingsWidget::loadProxySettings() {
    SettingsService settings;

    // Load current proxy settings
    int type =
        settings.value(QStringLiteral("networking/proxyType"), QNetworkProxy::NoProxy).toInt();
    switch (type) {
        case QNetworkProxy::NoProxy:
            ui->noProxyRadioButton->setChecked(true);
            break;
        case QNetworkProxy::DefaultProxy:
            ui->systemProxyRadioButton->setChecked(true);
            break;
        case QNetworkProxy::Socks5Proxy:
        case QNetworkProxy::HttpProxy:
            ui->typeComboBox->setCurrentIndex(ui->typeComboBox->findData(type));
            ui->manualProxyRadioButton->setChecked(true);
            break;
        default:
            break;
    }

    ui->hostLineEdit->setText(
        settings.value(QStringLiteral("networking/proxyHostName")).toString());
    ui->portSpinBox->setValue(settings.value(QStringLiteral("networking/proxyPort"), 8080).toInt());
    ui->authRequiredcheckBox->setChecked(
        settings.value(QStringLiteral("networking/proxyNeedsAuth")).toBool());
    ui->userLineEdit->setText(settings.value(QStringLiteral("networking/proxyUser")).toString());
    ui->passwordLineEdit->setText(CryptoService::instance()->decryptToString(
        settings.value(QStringLiteral("networking/proxyPassword")).toString()));
}

/**
 * Stores the proxy settings
 */
void NetworkSettingsWidget::storeProxySettings() {
    SettingsService settings;
    int proxyType = QNetworkProxy::DefaultProxy;

    if (ui->noProxyRadioButton->isChecked()) {
        proxyType = QNetworkProxy::NoProxy;
    } else if (ui->systemProxyRadioButton->isChecked()) {
        proxyType = QNetworkProxy::DefaultProxy;
    } else if (ui->manualProxyRadioButton->isChecked()) {
        proxyType = ui->typeComboBox->itemData(ui->typeComboBox->currentIndex()).toInt();

        settings.setValue(QStringLiteral("networking/proxyNeedsAuth"),
                          ui->authRequiredcheckBox->isChecked());
        settings.setValue(QStringLiteral("networking/proxyUser"), ui->userLineEdit->text());
        settings.setValue(
            QStringLiteral("networking/proxyPassword"),
            CryptoService::instance()->encryptToString(
                ui->passwordLineEdit->text(), QStringLiteral("settings/networking/proxyPassword")));
        settings.setValue(QStringLiteral("networking/proxyHostName"), ui->hostLineEdit->text());
        settings.setValue(QStringLiteral("networking/proxyPort"), ui->portSpinBox->value());
    }

    settings.setValue(QStringLiteral("networking/proxyType"), proxyType);

    ClientProxy proxy;

    // Refresh the Qt proxy settings
    proxy.setupQtProxyFromSettings();
}

void NetworkSettingsWidget::on_ignoreSSLErrorsCheckBox_toggled(bool checked) {
    ui->letsEncryptInfoLabel->setVisible(checked);
}

void NetworkSettingsWidget::on_appMetricsCheckBox_toggled(bool checked) {
    if (checked) {
        int reply;
        reply = QMessageBox::question(this, tr("Disable usage tracking"),
                                      tr("Anonymous usage data helps to decide what parts of "
                                         "QOwnNotes to improve next and to find and fix bugs."
                                         "<br />Please disable it only if you really can't live"
                                         " with it.<br /><br />Really disable usage tracking?"),
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (reply == QMessageBox::No) {
            const QSignalBlocker blocker(ui->appMetricsCheckBox);
            Q_UNUSED(blocker)
            ui->appMetricsCheckBox->setChecked(0);
        }
    }
}
