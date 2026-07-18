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

#include "languagetoolsettingswidget.h"

#include <QMessageBox>

#include "services/cryptoservice.h"
#include "services/settingsservice.h"
#include "ui_languagetoolsettingswidget.h"

#ifdef LANGUAGETOOL_ENABLED
#include "services/languagetoolchecker.h"
#include "services/languagetoolclient.h"
#endif

LanguageToolSettingsWidget::LanguageToolSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::LanguageToolSettingsWidget) {
    ui->setupUi(this);
}

LanguageToolSettingsWidget::~LanguageToolSettingsWidget() { delete ui; }

/**
 * Initializes the language tool settings (populates combo box)
 */
void LanguageToolSettingsWidget::initialize() {
#ifdef LANGUAGETOOL_ENABLED
    ui->languageToolLanguageComboBox->addItem(tr("Auto-detect"), QStringLiteral("auto"));
    ui->languageToolLanguageComboBox->addItem(QStringLiteral("English (US)"),
                                              QStringLiteral("en-US"));
    ui->languageToolLanguageComboBox->addItem(QStringLiteral("English (GB)"),
                                              QStringLiteral("en-GB"));
    ui->languageToolLanguageComboBox->addItem(QStringLiteral("German"), QStringLiteral("de-DE"));
    ui->languageToolLanguageComboBox->addItem(QStringLiteral("French"), QStringLiteral("fr"));
    ui->languageToolLanguageComboBox->addItem(QStringLiteral("Spanish"), QStringLiteral("es"));
    ui->languageToolLanguageComboBox->addItem(QStringLiteral("Italian"), QStringLiteral("it"));
    ui->languageToolLanguageComboBox->addItem(QStringLiteral("Dutch"), QStringLiteral("nl"));
    setLanguageToolOptionsEnabled(false);
#endif
}

void LanguageToolSettingsWidget::readSettings() {
#ifdef LANGUAGETOOL_ENABLED
    SettingsService settings;

    ui->languageToolEnabledCheckBox->setChecked(
        settings.value(QStringLiteral("languageToolEnabled"), false).toBool());
    ui->languageToolServerUrlLineEdit->setText(
        settings
            .value(QStringLiteral("languageToolServerUrl"), QStringLiteral("http://localhost:8081"))
            .toString());
    ui->languageToolApiKeyLineEdit->setText(
        CryptoService::instance()->decryptToStringWithPlaintextFallback(
            settings.value(QStringLiteral("languageToolApiKey")).toString()));
    ui->languageToolCheckDelaySpinBox->setValue(
        settings.value(QStringLiteral("languageToolCheckDelay"), 1500).toInt());

    const QString languageToolLanguage =
        settings.value(QStringLiteral("languageToolLanguage"), QStringLiteral("auto")).toString();
    int languageIndex = ui->languageToolLanguageComboBox->findData(languageToolLanguage);
    if (languageIndex < 0) {
        languageIndex = ui->languageToolLanguageComboBox->findText(languageToolLanguage);
    }
    if (languageIndex < 0) {
        ui->languageToolLanguageComboBox->setEditText(languageToolLanguage);
    } else {
        ui->languageToolLanguageComboBox->setCurrentIndex(languageIndex);
    }

    const QStringList enabledCategories =
        settings
            .value(QStringLiteral("languageToolEnabledCategories"),
                   QStringList() << QStringLiteral("TYPOS") << QStringLiteral("GRAMMAR")
                                 << QStringLiteral("STYLE") << QStringLiteral("REDUNDANCY")
                                 << QStringLiteral("PUNCTUATION") << QStringLiteral("TYPOGRAPHY"))
            .toStringList();
    ui->languageToolSpellingCheckBox->setChecked(
        enabledCategories.contains(QStringLiteral("TYPOS")));
    ui->languageToolGrammarCheckBox->setChecked(
        enabledCategories.contains(QStringLiteral("GRAMMAR")));
    ui->languageToolStyleCheckBox->setChecked(
        enabledCategories.contains(QStringLiteral("STYLE")) ||
        enabledCategories.contains(QStringLiteral("REDUNDANCY")));
    ui->languageToolPunctuationCheckBox->setChecked(
        enabledCategories.contains(QStringLiteral("PUNCTUATION")));
    ui->languageToolTypographyCheckBox->setChecked(
        enabledCategories.contains(QStringLiteral("TYPOGRAPHY")));
    setLanguageToolOptionsEnabled(ui->languageToolEnabledCheckBox->isChecked());
#endif
}

void LanguageToolSettingsWidget::storeSettings() {
#ifdef LANGUAGETOOL_ENABLED
    SettingsService settings;

    settings.setValue(QStringLiteral("languageToolEnabled"),
                      ui->languageToolEnabledCheckBox->isChecked());
    settings.setValue(QStringLiteral("languageToolServerUrl"),
                      ui->languageToolServerUrlLineEdit->text().trimmed());
    settings.setValue(QStringLiteral("languageToolLanguage"),
                      ui->languageToolLanguageComboBox->currentData().toString().isEmpty()
                          ? ui->languageToolLanguageComboBox->currentText().trimmed()
                          : ui->languageToolLanguageComboBox->currentData().toString());
    settings.setValue(
        QStringLiteral("languageToolApiKey"),
        CryptoService::instance()->encryptToString(ui->languageToolApiKeyLineEdit->text().trimmed(),
                                                   QStringLiteral("settings/languageToolApiKey")));
    settings.setValue(QStringLiteral("languageToolCheckDelay"),
                      ui->languageToolCheckDelaySpinBox->value());
    settings.setValue(QStringLiteral("languageToolEnabledCategories"),
                      languageToolEnabledCategoriesFromUi());

    // Let the checker know that the settings (including the API key) have
    // changed, so it can reload them once now instead of on every keystroke.
    LanguageToolChecker::instance()->applySettings();
#endif
}

#ifdef LANGUAGETOOL_ENABLED
QStringList LanguageToolSettingsWidget::languageToolEnabledCategoriesFromUi() const {
    QStringList categories;
    if (ui->languageToolSpellingCheckBox->isChecked()) {
        categories.append(QStringLiteral("TYPOS"));
    }
    if (ui->languageToolGrammarCheckBox->isChecked()) {
        categories.append(QStringLiteral("GRAMMAR"));
    }
    if (ui->languageToolStyleCheckBox->isChecked()) {
        categories.append(QStringLiteral("STYLE"));
        categories.append(QStringLiteral("REDUNDANCY"));
    }
    if (ui->languageToolPunctuationCheckBox->isChecked()) {
        categories.append(QStringLiteral("PUNCTUATION"));
    }
    if (ui->languageToolTypographyCheckBox->isChecked()) {
        categories.append(QStringLiteral("TYPOGRAPHY"));
    }

    return categories;
}

void LanguageToolSettingsWidget::setLanguageToolOptionsEnabled(bool enabled) {
    ui->languageToolOptionsWidget->setEnabled(enabled);
}

void LanguageToolSettingsWidget::on_languageToolEnabledCheckBox_toggled(bool checked) {
    setLanguageToolOptionsEnabled(checked);
}

void LanguageToolSettingsWidget::on_languageToolTestConnectionButton_clicked() {
    auto *client = new LanguageToolClient(this);
    LanguageToolClient::RequestOptions options;
    options.requestId = 1;
    options.serverUrl = ui->languageToolServerUrlLineEdit->text().trimmed();
    options.language = ui->languageToolLanguageComboBox->currentData().toString().isEmpty()
                           ? ui->languageToolLanguageComboBox->currentText().trimmed()
                           : ui->languageToolLanguageComboBox->currentData().toString();
    options.apiKey = ui->languageToolApiKeyLineEdit->text().trimmed();
    options.timeoutMs = 5000;
    options.text = QStringLiteral("This are a test sentence.");
    options.enabledCategories = languageToolEnabledCategoriesFromUi();

    connect(client, &LanguageToolClient::checkFinished, this,
            [this, client](int, const QVector<LanguageToolMatch> &) {
                QMessageBox::information(this, tr("LanguageTool"),
                                         tr("LanguageTool connection successful."));
                client->deleteLater();
            });
    connect(client, &LanguageToolClient::checkError, this,
            [this, client](int, const QString &errorMessage) {
                QMessageBox::warning(this, tr("LanguageTool"),
                                     tr("LanguageTool connection failed: %1").arg(errorMessage));
                client->deleteLater();
            });

    client->checkText(options);
}

void LanguageToolSettingsWidget::on_languageToolResetIgnoredRulesButton_clicked() {
    auto *checker = LanguageToolChecker::instance();
    if (checker == nullptr) {
        return;
    }

    const int count = checker->ignoredRules().size();
    if (count == 0) {
        QMessageBox::information(this, tr("LanguageTool"),
                                 tr("There are no ignored rules to reset."));
        return;
    }

    if (QMessageBox::question(this, tr("LanguageTool"), tr("Reset %n ignored rule(s)?", "", count),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        checker->clearIgnoredRules();
        QMessageBox::information(this, tr("LanguageTool"),
                                 tr("All ignored rules have been reset."));
    }
}

void LanguageToolSettingsWidget::on_languageToolResetIgnoredWordsButton_clicked() {
    auto *checker = LanguageToolChecker::instance();
    if (checker == nullptr) {
        return;
    }

    const int count = checker->ignoredWords().size();
    if (count == 0) {
        QMessageBox::information(this, tr("LanguageTool"),
                                 tr("There are no ignored words to reset."));
        return;
    }

    if (QMessageBox::question(this, tr("LanguageTool"), tr("Reset %n ignored word(s)?", "", count),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        checker->clearIgnoredWords();
        QMessageBox::information(this, tr("LanguageTool"),
                                 tr("All ignored words have been reset."));
    }
}
#endif
