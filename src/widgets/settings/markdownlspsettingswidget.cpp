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

#include "markdownlspsettingswidget.h"

#include <QDir>
#include <QEventLoop>
#include <QMessageBox>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTimer>

#include "entities/notefolder.h"
#include "services/markdownlspclient.h"
#include "services/markdownlspignoredrules.h"
#include "services/settingsservice.h"
#include "ui_markdownlspsettingswidget.h"
#include "version.h"

namespace {
struct KnownMarkdownLspServer {
    const char *name;
    const char *executable;
    QStringList arguments;
};

const QVector<KnownMarkdownLspServer> kKnownMarkdownLspServers = {
    {"Marksman", "marksman", {}},
    {"Rumdl", "rumdl", {QStringLiteral("server")}},
};
}    // namespace

MarkdownLspSettingsWidget::MarkdownLspSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::MarkdownLspSettingsWidget) {
    ui->setupUi(this);
    ui->gridLayout_markdownLsp->setColumnStretch(1, 1);
}

MarkdownLspSettingsWidget::~MarkdownLspSettingsWidget() { delete ui; }

void MarkdownLspSettingsWidget::initialize() { updateStatusLabel(); }

void MarkdownLspSettingsWidget::readSettings() {
    SettingsService settings;

    ui->markdownLspEnabledCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/markdownLspEnabled"), false).toBool());
    ui->markdownLspCommandLineEdit->setText(
        settings.value(QStringLiteral("Editor/markdownLspCommand"), QStringLiteral("marksman"))
            .toString());
    ui->markdownLspArgumentsLineEdit->setText(
        settings.value(QStringLiteral("Editor/markdownLspArguments"))
            .toStringList()
            .join(QLatin1Char(' ')));
    ui->markdownLspVerboseLoggingCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/markdownLspVerboseLogging"), false).toBool());

    on_markdownLspEnabledCheckBox_toggled(ui->markdownLspEnabledCheckBox->isChecked());
    updateStatusLabel();
}

void MarkdownLspSettingsWidget::storeSettings() {
    SettingsService settings;

    settings.setValue(QStringLiteral("Editor/markdownLspEnabled"),
                      ui->markdownLspEnabledCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/markdownLspCommand"),
                      ui->markdownLspCommandLineEdit->text().trimmed());
    settings.setValue(QStringLiteral("Editor/markdownLspArguments"), argumentsFromUi());
    settings.setValue(QStringLiteral("Editor/markdownLspVerboseLogging"),
                      ui->markdownLspVerboseLoggingCheckBox->isChecked());
}

void MarkdownLspSettingsWidget::on_markdownLspEnabledCheckBox_toggled(bool checked) {
    Q_UNUSED(checked)
    updateStatusLabel();
}

QStringList MarkdownLspSettingsWidget::argumentsFromUi() const {
    return ui->markdownLspArgumentsLineEdit->text().split(
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
        QRegularExpression(QStringLiteral("\\s+")), QString::SkipEmptyParts);
#else
        QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
#endif
}

QString MarkdownLspSettingsWidget::resolveCommandPath(const QString &command) {
    const QString trimmed = command.trimmed();
    if (trimmed.isEmpty()) {
        return QString();
    }

    const QString resolved = QStandardPaths::findExecutable(trimmed);
    return resolved.isEmpty() ? trimmed : resolved;
}

void MarkdownLspSettingsWidget::updateStatusLabel(const QString &overrideText) {
    if (!overrideText.isEmpty()) {
        ui->markdownLspStatusLabel->setText(overrideText);
        return;
    }

    const QString command = ui->markdownLspCommandLineEdit->text().trimmed();
    if (command.isEmpty()) {
        ui->markdownLspStatusLabel->setText(tr("No Markdown LSP server configured."));
        return;
    }

    const QString resolvedCommand = resolveCommandPath(command);
    const QString argumentsText = ui->markdownLspArgumentsLineEdit->text().trimmed();
    QString status = tr("Command: %1").arg(resolvedCommand);
    if (!argumentsText.isEmpty()) {
        status += tr(" | Arguments: %1").arg(argumentsText);
    }
    if (!ui->markdownLspEnabledCheckBox->isChecked()) {
        status += tr(" | Currently disabled");
    }

    ui->markdownLspStatusLabel->setText(status);
}

void MarkdownLspSettingsWidget::on_markdownLspAutoDetectButton_clicked() {
    for (const KnownMarkdownLspServer &server : kKnownMarkdownLspServers) {
        const QString detectedPath =
            QStandardPaths::findExecutable(QString::fromLatin1(server.executable));
        if (detectedPath.isEmpty()) {
            continue;
        }

        ui->markdownLspCommandLineEdit->setText(detectedPath);
        ui->markdownLspArgumentsLineEdit->setText(server.arguments.join(QLatin1Char(' ')));
        ui->markdownLspEnabledCheckBox->setChecked(true);

        QString status = tr("Configured %1 at %2").arg(tr(server.name), detectedPath);
        if (!server.arguments.isEmpty()) {
            status += tr(" with arguments: %1").arg(server.arguments.join(QLatin1Char(' ')));
        }

        updateStatusLabel(status);
        QMessageBox::information(this, tr("Markdown LSP"), status);
        return;
    }

    const QString message =
        tr("Could not find a supported Markdown LSP server in PATH. Checked: marksman, rumdl.");
    updateStatusLabel(message);
    QMessageBox::warning(this, tr("Markdown LSP"), message);
}

void MarkdownLspSettingsWidget::on_markdownLspResetIgnoredRulesButton_clicked() {
    auto *ignoredRules = MarkdownLspIgnoredRules::instance();
    if (ignoredRules == nullptr) {
        return;
    }

    const int count = ignoredRules->ignoredRules().size();
    if (count == 0) {
        QMessageBox::information(this, tr("Markdown LSP"),
                                 tr("There are no ignored rules to reset."));
        return;
    }

    if (QMessageBox::question(this, tr("Markdown LSP"),
                              tr("Reset %n ignored Markdown LSP rule(s)?", "", count),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        ignoredRules->clearIgnoredRules();
        QMessageBox::information(this, tr("Markdown LSP"),
                                 tr("All ignored Markdown LSP rules have been reset."));
    }
}

void MarkdownLspSettingsWidget::on_markdownLspTestConnectionButton_clicked() {
    const QString command = ui->markdownLspCommandLineEdit->text().trimmed();
    if (command.isEmpty()) {
        const QString message = tr("Please enter a Markdown LSP server command first.");
        updateStatusLabel(message);
        QMessageBox::warning(this, tr("Markdown LSP"), message);
        return;
    }

    auto *client = new MarkdownLspClient(this);
    client->setServerCommand(command, argumentsFromUi());
    client->setVerboseLogging(ui->markdownLspVerboseLoggingCheckBox->isChecked());

    QEventLoop loop;
    QTimer timeoutTimer(this);
    timeoutTimer.setSingleShot(true);

    bool success = false;
    QString failureMessage;

    connect(client, &MarkdownLspClient::errorMessage, &loop, [&](const QString &message) {
        failureMessage = message.trimmed();
        loop.quit();
    });
    connect(client, &MarkdownLspClient::serverInitialized, &loop, [&]() {
        success = true;
        loop.quit();
    });
    connect(&timeoutTimer, &QTimer::timeout, &loop, [&]() {
        failureMessage = tr("Timed out waiting for the Markdown LSP server to initialize.");
        loop.quit();
    });

    if (!client->start()) {
        if (failureMessage.isEmpty()) {
            failureMessage = tr("Failed to start Markdown LSP server: %1").arg(command);
        }
    } else {
        QString rootPath = NoteFolder::currentLocalPath();
        if (rootPath.isEmpty()) {
            rootPath = QDir::homePath();
        }

        timeoutTimer.start(5000);
        client->initialize(rootPath, QStringLiteral("QOwnNotes"), QStringLiteral(VERSION));
        loop.exec();
    }

    client->shutdown();
    client->deleteLater();

    if (success) {
        const QString message =
            tr("Markdown LSP connection successful: %1").arg(resolveCommandPath(command));
        updateStatusLabel(message);
        QMessageBox::information(this, tr("Markdown LSP"), message);
        return;
    }

    if (failureMessage.isEmpty()) {
        failureMessage = tr("Markdown LSP connection failed.");
    }

    updateStatusLabel(failureMessage);
    QMessageBox::warning(this, tr("Markdown LSP"), failureMessage);
}
