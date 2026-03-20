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

#include "aitoolbarmanager.h"

#include <services/openaiservice.h>
#include <services/settingsservice.h>
#include <utils/gui.h>

#include <QAction>
#include <QActionGroup>
#include <QComboBox>
#include <QDebug>
#include <QMenu>
#include <QToolBar>
#include <QWidgetAction>

#include "libraries/qtwaitingspinner/waitingspinnerwidget.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

AiToolbarManager::AiToolbarManager(MainWindow *mainWindow, Ui::MainWindow *ui, QToolBar *aiToolbar,
                                   QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui), _aiToolbar(aiToolbar) {
    _aiModelGroup = new QActionGroup(ui->menuAI_model);
}

void AiToolbarManager::initializeOpenAiActivitySpinner() {
    _openAiActivitySpinner = new WaitingSpinnerWidget(0, false, false);
    _openAiActivitySpinner->setNumberOfLines(12);
    _openAiActivitySpinner->setLineLength(5);
    _openAiActivitySpinner->setLineWidth(2);
    _openAiActivitySpinner->setInnerRadius(3);
    _openAiActivitySpinner->setRevolutionsPerSecond(1);
    _openAiActivitySpinner->setToolTip(tr("Waiting for answer from AI"));

    const bool darkMode = SettingsService().value(QStringLiteral("darkMode")).toBool();
    _openAiActivitySpinner->setColor(darkMode ? Qt::white : Qt::black);
}

void AiToolbarManager::reloadOpenAiControls() {
    OpenAiService::deleteInstance();
    generateAiBackendComboBox();
    generateAiModelComboBox();
    generateAiModelMainMenu();

    aiModelMainMenuSetCurrentItem();
}

void AiToolbarManager::onAiBackendComboBoxCurrentIndexChanged(int index) {
    Q_UNUSED(index)

    const QString backendId = _aiBackendComboBox->currentData().toString();

    if (OpenAiService::instance()->setBackendId(backendId)) {
        generateAiModelComboBox();
        aiModelMainMenuSetCurrentItem();
    }
}

/**
 * Puts items into the AI backend combo box
 */
void AiToolbarManager::generateAiBackendComboBox() {
    _aiBackendComboBox->blockSignals(true);
    _aiBackendComboBox->clear();
    auto backendNames = OpenAiService::instance()->getBackendNames();

    for (const auto &key : backendNames.keys()) {
        const QString &name = backendNames.value(key);
        _aiBackendComboBox->addItem(name, key);
    }

    Utils::Gui::setComboBoxIndexByUserData(_aiBackendComboBox,
                                           OpenAiService::instance()->getBackendId());
    _aiBackendComboBox->blockSignals(false);
}

/**
 * Puts items into the AI model main menu
 */
void AiToolbarManager::generateAiModelMainMenu() {
    QMap<QString, QString> backendNames = OpenAiService::instance()->getBackendNames();
    _ui->menuAI_model->clear();

    for (const auto &backendId : backendNames.keys()) {
        const QString &backendName = backendNames.value(backendId);

        // Create a submenu for the backend models
        auto *modelSubMenu = new QMenu(backendName, _ui->menuAI_model);

        // Retrieve models for the current backend
        QStringList models = OpenAiService::instance()->getModelsForBackend(backendId);

        // Add each model as an action to the submenu
        for (const QString &modelId : models) {
            auto *modelAction = new QAction(modelId, modelSubMenu);
            modelAction->setData(QStringList() << backendId << modelId);
            modelSubMenu->addAction(modelAction);
            modelAction->setActionGroup(_aiModelGroup);
            modelAction->setCheckable(true);
        }

        // Add the submenu to the main menu
        _ui->menuAI_model->addMenu(modelSubMenu);
    }
}

void AiToolbarManager::aiModelMainMenuSetCurrentItem() {
    auto currentBackendId = OpenAiService::instance()->getBackendId();
    auto currentModelId = OpenAiService::instance()->getModelId();
    auto action = Utils::Gui::findActionByData(_ui->menuAI_model,
                                               QStringList() << currentBackendId << currentModelId);

    if (action) {
        _aiModelGroup->blockSignals(true);
        action->setChecked(true);
        _aiModelGroup->blockSignals(false);
    }
}

/**
 * Sets the AI model when the AI model combo box index has changed
 */
void AiToolbarManager::onAiModelComboBoxCurrentIndexChanged(int index) {
    Q_UNUSED(index)

    const QString modelId = _aiModelComboBox->currentData().toString();

    if (OpenAiService::instance()->setModelId(modelId)) {
        generateAiModelComboBox();
        aiModelMainMenuSetCurrentItem();
    }
}

void AiToolbarManager::onAiModelGroupChanged(QAction *action) {
    const auto data = action->data().toStringList();
    const auto &backendId = data[0];
    const auto &modelId = data[1];

    if (OpenAiService::instance()->setBackendId(backendId)) {
        generateAiBackendComboBox();
    }

    if (OpenAiService::instance()->setModelId(modelId)) {
        generateAiModelComboBox();
    }
}

/**
 * Puts items into the AI model combo box
 */
void AiToolbarManager::generateAiModelComboBox() {
    _aiModelComboBox->blockSignals(true);
    _aiModelComboBox->clear();
    const auto models = OpenAiService::instance()->getModelsForCurrentBackend();

    foreach (QString model, models) {
        _aiModelComboBox->addItem(model, model);
    }

    Utils::Gui::setComboBoxIndexByUserData(_aiModelComboBox,
                                           OpenAiService::instance()->getModelId());
    _aiModelComboBox->blockSignals(false);
}

void AiToolbarManager::buildAiToolbarAndActions() {
    _aiToolbar->clear();
    _aiToolbar->addAction(_ui->actionEnable_AI);
    _ui->actionEnable_AI->blockSignals(true);
    _ui->actionEnable_AI->setChecked(OpenAiService::getEnabled());
    _ui->actionEnable_AI->blockSignals(false);

    _aiBackendComboBox = new QComboBox(_mainWindow);
    connect(_aiBackendComboBox,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &AiToolbarManager::onAiBackendComboBoxCurrentIndexChanged);
    _aiBackendComboBox->setToolTip(tr("AI backends"));
    _aiBackendComboBox->setObjectName(QStringLiteral("aiBackendComboBox"));
    _aiBackendComboBox->setInsertPolicy(QComboBox::InsertPolicy::InsertAfterCurrent);
    generateAiBackendComboBox();

    _aiModelComboBox = new QComboBox(_mainWindow);
    connect(_aiModelComboBox,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &AiToolbarManager::onAiModelComboBoxCurrentIndexChanged);
    _aiModelComboBox->setToolTip(tr("AI models"));
    _aiModelComboBox->setObjectName(QStringLiteral("aiModelComboBox"));
    _aiModelComboBox->setInsertPolicy(QComboBox::InsertPolicy::InsertAfterCurrent);
    generateAiModelComboBox();

    auto *aiBackendWidgetAction = new QWidgetAction(_mainWindow);
    aiBackendWidgetAction->setDefaultWidget(_aiBackendComboBox);
    aiBackendWidgetAction->setObjectName(QStringLiteral("actionAiBackendComboBox"));
    aiBackendWidgetAction->setText(tr("AI backend selector"));
    _aiToolbar->addAction(aiBackendWidgetAction);

    auto *aiModelWidgetAction = new QWidgetAction(_mainWindow);
    aiModelWidgetAction->setDefaultWidget(_aiModelComboBox);
    aiModelWidgetAction->setObjectName(QStringLiteral("actionAiModelComboBox"));
    aiModelWidgetAction->setText(tr("AI model selector"));
    _aiToolbar->addAction(aiModelWidgetAction);

    _aiModelGroup->setExclusive(true);
    connect(_aiModelGroup, &QActionGroup::triggered, this,
            &AiToolbarManager::onAiModelGroupChanged);
    generateAiModelMainMenu();
    aiModelMainMenuSetCurrentItem();
}

void AiToolbarManager::on_actionEnable_AI_toggled(bool arg1) {
    OpenAiService::setEnabled(arg1);
    qDebug() << __func__ << " - 'checked': " << arg1;
}

void AiToolbarManager::enableOpenAiActivitySpinner(bool enable) {
    if (_openAiActivitySpinner == nullptr) {
        return;
    }

    if (enable) {
        _openAiActivitySpinner->start();
    } else {
        _openAiActivitySpinner->stop();
    }
}
