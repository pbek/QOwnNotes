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

#include "webcompanionsettingswidget.h"

#include <QApplication>
#include <QClipboard>

#include "dialogs/websockettokendialog.h"
#include "services/cryptoservice.h"
#include "services/settingsservice.h"
#include "services/websocketserverservice.h"
#include "ui_webcompanionsettingswidget.h"
#include "utils/misc.h"

WebCompanionSettingsWidget::WebCompanionSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::WebCompanionSettingsWidget) {
    ui->setupUi(this);

    // Connect enableSocketServerCheckBox to needRestart signal
    connect(ui->enableSocketServerCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(needRestart()));
}

WebCompanionSettingsWidget::~WebCompanionSettingsWidget() { delete ui; }

/**
 * Initializes the web companion page (lazy initialization on first visit)
 */
void WebCompanionSettingsWidget::initialize() {
    ui->webCompannionLabel->setText(ui->webCompannionLabel->text().arg(
        "https://github.com/qownnotes/web-companion",
        "https://chrome.google.com/webstore/detail/qownnotes-web-companion/"
        "pkgkfnampapjbopomdpnkckbjdnpkbkp",
        "https://addons.mozilla.org/firefox/addon/qownnotes-web-companion"));

    ui->commandLineSnippetManagerLabel->setText(
        ui->commandLineSnippetManagerLabel->text().arg("https://github.com/qownnotes/qc"));

    // Store original HTML texts for enable/disable state management
    ui->commandSnippetTagLabel->setText(ui->commandSnippetTagLabel->text().arg(
        "https://www.qownnotes.org/getting-started/command-line-snippet-manager.html"));
    _commandSnippetTagLabelHtml = ui->commandSnippetTagLabel->text();

    ui->commandSnippetsNoteNameLabel->hide();
    ui->commandSnippetsNoteNameLineEdit->hide();

    ui->bookmarkTagLabel->setText(ui->bookmarkTagLabel->text().arg(
        "https://www.qownnotes.org/getting-started/browser-extension.html"));
    _bookmarkTagLabelHtml = ui->bookmarkTagLabel->text();
}

void WebCompanionSettingsWidget::readSettings() {
    ui->enableSocketServerCheckBox->setChecked(Utils::Misc::isSocketServerEnabled());
    on_enableSocketServerCheckBox_toggled();

    ui->webSocketServerServicePortSpinBox->setValue(WebSocketServerService::getSettingsPort());
    ui->bookmarkSuggestionApiEnabledCheckBox->setChecked(
        WebSocketServerService::isBookmarkSuggestionApiEnabled());
    ui->bookmarkSuggestionApiPortSpinBox->setValue(
        WebSocketServerService::getBookmarkSuggestionApiPort());
    ui->bookmarkSuggestionApiTokenLineEdit->setText(
        WebSocketServerService::getOrGenerateBookmarkSuggestionApiToken());
    on_bookmarkSuggestionApiEnabledCheckBox_toggled(
        ui->bookmarkSuggestionApiEnabledCheckBox->isChecked());

    ui->bookmarksTagLineEdit->setText(WebSocketServerService::getBookmarksTag());
    ui->bookmarksNoteNameLineEdit->setText(WebSocketServerService::getBookmarksNoteName());
    ui->commandSnippetsTagLineEdit->setText(WebSocketServerService::getCommandSnippetsTag());
    ui->commandSnippetsNoteNameLineEdit->setText(
        WebSocketServerService::getCommandSnippetsNoteName());
}

void WebCompanionSettingsWidget::storeSettings() {
    SettingsService settings;

    settings.setValue(QStringLiteral("enableSocketServer"),
                      ui->enableSocketServerCheckBox->isChecked());
    settings.setValue(QStringLiteral("webSocketServerService/port"),
                      ui->webSocketServerServicePortSpinBox->value());
    settings.setValue(QStringLiteral("webSocketServerService/bookmarkSuggestionApiEnabled"),
                      ui->bookmarkSuggestionApiEnabledCheckBox->isChecked());
    settings.setValue(QStringLiteral("webSocketServerService/bookmarkSuggestionApiPort"),
                      ui->bookmarkSuggestionApiPortSpinBox->value());
    QString bookmarkSuggestionApiToken = ui->bookmarkSuggestionApiTokenLineEdit->text().trimmed();
    if (bookmarkSuggestionApiToken.isEmpty()) {
        bookmarkSuggestionApiToken = Utils::Misc::generateRandomString(32);
        ui->bookmarkSuggestionApiTokenLineEdit->setText(bookmarkSuggestionApiToken);
    }
    settings.setValue(
        QStringLiteral("webSocketServerService/bookmarkSuggestionApiToken"),
        CryptoService::instance()->encryptToString(
            bookmarkSuggestionApiToken,
            QStringLiteral("settings/webSocketServerService/bookmarkSuggestionApiToken")));
    settings.setValue(QStringLiteral("webSocketServerService/bookmarksTag"),
                      ui->bookmarksTagLineEdit->text());
    settings.setValue(QStringLiteral("webSocketServerService/bookmarksNoteName"),
                      ui->bookmarksNoteNameLineEdit->text());
    settings.setValue(QStringLiteral("webSocketServerService/commandSnippetsTag"),
                      ui->commandSnippetsTagLineEdit->text());
    settings.setValue(QStringLiteral("webSocketServerService/commandSnippetsNoteName"),
                      ui->commandSnippetsNoteNameLineEdit->text());
}

void WebCompanionSettingsWidget::on_webSocketServerServicePortResetButton_clicked() {
    ui->webSocketServerServicePortSpinBox->setValue(WebSocketServerService::getDefaultPort());
}

void WebCompanionSettingsWidget::on_enableSocketServerCheckBox_toggled() {
    bool checked = ui->enableSocketServerCheckBox->isChecked();
    ui->browserExtensionFrame->setEnabled(checked);

    // Update labels with disabled color when unchecked
    QString disabledColor = palette().color(QPalette::Disabled, QPalette::Text).name();

    if (!_bookmarkTagLabelHtml.isEmpty()) {
        if (checked) {
            ui->bookmarkTagLabel->setText(_bookmarkTagLabelHtml);
        } else {
            // Wrap content in a span with disabled color
            ui->bookmarkTagLabel->setText(QStringLiteral("<span style=\"color:%1;\">%2</span>")
                                              .arg(disabledColor, _bookmarkTagLabelHtml));
        }
    }

    if (!_commandSnippetTagLabelHtml.isEmpty()) {
        if (checked) {
            ui->commandSnippetTagLabel->setText(_commandSnippetTagLabelHtml);
        } else {
            // Wrap content in a span with disabled color
            ui->commandSnippetTagLabel->setText(
                QStringLiteral("<span style=\"color:%1;\">%2</span>")
                    .arg(disabledColor, _commandSnippetTagLabelHtml));
        }
    }

    ui->bookmarkSuggestionApiGroupBox->setEnabled(checked);
}

void WebCompanionSettingsWidget::on_bookmarkSuggestionApiEnabledCheckBox_toggled(bool checked) {
    ui->bookmarkSuggestionApiPortLabel->setEnabled(checked);
    ui->bookmarkSuggestionApiPortSpinBox->setEnabled(checked);
    ui->bookmarkSuggestionApiPortResetButton->setEnabled(checked);
    ui->bookmarkSuggestionApiTokenLabel->setEnabled(checked);
    ui->bookmarkSuggestionApiTokenLineEdit->setEnabled(checked);
    ui->bookmarkSuggestionApiShowTokenButton->setEnabled(checked);
    ui->bookmarkSuggestionApiCopyTokenButton->setEnabled(checked);
    ui->bookmarkSuggestionApiGenerateTokenButton->setEnabled(checked);
}

void WebCompanionSettingsWidget::on_bookmarkSuggestionApiPortResetButton_clicked() {
    ui->bookmarkSuggestionApiPortSpinBox->setValue(
        WebSocketServerService::getBookmarkSuggestionApiDefaultPort());
}

void WebCompanionSettingsWidget::on_bookmarkSuggestionApiShowTokenButton_clicked() {
    ui->bookmarkSuggestionApiTokenLineEdit->setEchoMode(
        ui->bookmarkSuggestionApiTokenLineEdit->echoMode() == QLineEdit::EchoMode::Password
            ? QLineEdit::EchoMode::Normal
            : QLineEdit::EchoMode::Password);
}

void WebCompanionSettingsWidget::on_bookmarkSuggestionApiCopyTokenButton_clicked() {
    QApplication::clipboard()->setText(ui->bookmarkSuggestionApiTokenLineEdit->text());
}

void WebCompanionSettingsWidget::on_bookmarkSuggestionApiGenerateTokenButton_clicked() {
    ui->bookmarkSuggestionApiTokenLineEdit->setText(Utils::Misc::generateRandomString(32));
    ui->bookmarkSuggestionApiTokenLineEdit->setEchoMode(QLineEdit::EchoMode::Normal);
}

void WebCompanionSettingsWidget::on_webSocketTokenButton_clicked() {
    auto webSocketTokenDialog = new WebSocketTokenDialog();
    webSocketTokenDialog->exec();
    delete (webSocketTokenDialog);
}
