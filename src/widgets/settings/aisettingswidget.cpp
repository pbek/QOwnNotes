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

#include "aisettingswidget.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QPushButton>
#include <QUrl>

#include "services/cryptoservice.h"
#include "services/openaiservice.h"
#include "services/settingsservice.h"
#include "ui_aisettingswidget.h"

AiSettingsWidget::AiSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::AiSettingsWidget) {
    ui->setupUi(this);
}

AiSettingsWidget::~AiSettingsWidget() { delete ui; }

/**
 * Initializes the AI page (lazy initialization on first visit)
 */
void AiSettingsWidget::initialize() {
    ui->groqApiTestButton->setDisabled(true);
    ui->openAiApiTestButton->setDisabled(true);
    ui->aiScriptingGroupBox->setHidden(true);

    ui->openAiScriptingLabel->setText(ui->openAiScriptingLabel->text().arg(
        "https://www.qownnotes.org/scripting/hooks.html#openaibackendshook"));
    ui->openAiScriptingLabel3->setText(
        ui->openAiScriptingLabel3->text().arg("https://www.qownnotes.org/scripting/"
                                              "methods-and-objects.html#use-a-completion-"
                                              "prompt-on-the-currently-selected-ai-model"));
}

void AiSettingsWidget::readSettings() {
    SettingsService settings;

    ui->groqApiKeyLineEdit->setText(CryptoService::instance()->decryptToString(
        settings.value(QStringLiteral("ai/groq/apiKey")).toString()));

    ui->openAiApiKeyLineEdit->setText(CryptoService::instance()->decryptToString(
        settings.value(QStringLiteral("ai/openai/apiKey")).toString()));

    ui->openAiResponseTimeoutSpinBox->setValue(OpenAiService::getResponseTimeout());
}

void AiSettingsWidget::storeSettings() {
    SettingsService settings;

    settings.setValue(
        QStringLiteral("ai/groq/apiKey"),
        CryptoService::instance()->encryptToString(ui->groqApiKeyLineEdit->text(),
                                                   QStringLiteral("settings/ai/groq/apiKey")));

    settings.setValue(
        QStringLiteral("ai/openai/apiKey"),
        CryptoService::instance()->encryptToString(ui->openAiApiKeyLineEdit->text(),
                                                   QStringLiteral("settings/ai/openai/apiKey")));
    settings.setValue(QStringLiteral("ai/responseTimeout"),
                      ui->openAiResponseTimeoutSpinBox->value());
}

void AiSettingsWidget::on_groqApiKeyWebButton_clicked() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://console.groq.com/keys")));
}

void AiSettingsWidget::on_openAiApiKeyWebButton_clicked() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://platform.openai.com/api-keys")));
}

void AiSettingsWidget::on_groqApiTestButton_clicked() {
    runAiApiTest(QStringLiteral("groq"), QStringLiteral("llama3-8b-8192"),
                 ui->groqApiKeyLineEdit->text());
}

void AiSettingsWidget::on_openAiApiTestButton_clicked() {
    runAiApiTest(QStringLiteral("openai"), QStringLiteral("gpt-4o"),
                 ui->openAiApiKeyLineEdit->text());
}

void AiSettingsWidget::on_groqApiKeyLineEdit_textChanged(const QString &arg1) {
    ui->groqApiTestButton->setDisabled(arg1.isEmpty());
}

void AiSettingsWidget::on_openAiApiKeyLineEdit_textChanged(const QString &arg1) {
    ui->openAiApiTestButton->setDisabled(arg1.isEmpty());
}

void AiSettingsWidget::runAiApiTest(QString backend, QString model, QString apiKey) {
    OpenAiService *openAiService = OpenAiService::instance();
    openAiService->setBackendId(backend);
    openAiService->setModelId(model);
    if (!apiKey.isEmpty()) {
        openAiService->setApiKeyForCurrentBackend(apiKey);
    }
    QString result = openAiService->complete("Test");
    QMessageBox::information(this, tr("API test result for %1 (%2)").arg(backend, model), result);
}

/**
 * Builds the tree widget showing AI backends added via scripting
 */
void AiSettingsWidget::buildAiScriptingTreeWidget() {
    OpenAiService *openAiService = OpenAiService::instance();
    auto backendNames = openAiService->getBackendNames();
    qDebug() << __func__ << " - 'backendNames': " << backendNames;

    if (backendNames.count() > 2) {
        ui->aiScriptingTreeWidget->clear();
        ui->aiScriptingGroupBox->setVisible(true);
    } else {
        ui->aiScriptingGroupBox->setVisible(false);
        return;
    }

    for (const auto &backendId : backendNames.keys()) {
        // Continue on groq and openai
        if (backendId == QStringLiteral("groq") || backendId == QStringLiteral("openai")) {
            continue;
        }

        const QString &backendName = backendNames.value(backendId);

        auto backendItem = new QTreeWidgetItem(ui->aiScriptingTreeWidget);
        backendItem->setText(0, backendName);
        backendItem->setToolTip(0, tr("AI backend: %1").arg(backendId));
        backendItem->setData(0, Qt::UserRole, backendId);
        backendItem->setText(1, openAiService->getApiBaseUrlForBackend(backendId));
        backendItem->setToolTip(1, tr("API base URL").arg(backendId));
        backendItem->setFlags(backendItem->flags() & ~Qt::ItemIsSelectable);

        auto models = openAiService->getModelsForBackend(backendId);
        for (const auto &model : models) {
            auto modelItem = new QTreeWidgetItem(backendItem);
            modelItem->setText(0, model);
            modelItem->setToolTip(0, tr("AI model: %1").arg(model));
            modelItem->setData(0, Qt::UserRole, model);
            modelItem->setFlags(modelItem->flags() | Qt::ItemIsSelectable);

            // Add test button in new column
            auto testButton = new QPushButton();
            testButton->setText(tr("Test", "verb"));
            testButton->setToolTip(tr("Test connection to %1 (%2)").arg(backendName, model));
            testButton->setIcon(
                QIcon::fromTheme(QStringLiteral("network-connect"),
                                 QIcon(":/icons/breeze-qownnotes/16x16/network-connect.svg")));
            testButton->setProperty("backend", backendId);
            testButton->setProperty("model", model);
            connect(testButton, &QPushButton::clicked, this, [this, testButton]() {
                QString backend = testButton->property("backend").toString();
                QString model = testButton->property("model").toString();
                runAiApiTest(backend, model);
            });

            ui->aiScriptingTreeWidget->setItemWidget(modelItem, 2, testButton);
        }
    }

    ui->aiScriptingTreeWidget->expandAll();
    ui->aiScriptingTreeWidget->resizeColumnToContents(0);
    ui->aiScriptingTreeWidget->resizeColumnToContents(1);
    ui->aiScriptingTreeWidget->resizeColumnToContents(2);
}

void AiSettingsWidget::on_searchScriptRepositoryButton_clicked() {
    emit searchScriptRepositoryRequested();
    buildAiScriptingTreeWidget();
}
