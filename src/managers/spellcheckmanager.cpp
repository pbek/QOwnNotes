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

#include "spellcheckmanager.h"

#include <dialogs/dictionarymanagerdialog.h>
#include <services/settingsservice.h>

#include <QActionGroup>

#include "libraries/sonnet/src/core/speller.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

SpellCheckManager::SpellCheckManager(MainWindow *mainWindow, Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui) {
    _languageGroup = new QActionGroup(this);
    _spellBackendGroup = new QActionGroup(this);
}

void SpellCheckManager::on_actionCheck_spelling_toggled(bool checked) {
    SettingsService settings;
    settings.setValue(QStringLiteral("checkSpelling"), checked);
    _ui->noteTextEdit->updateSettings();
    _ui->encryptedNoteTextEdit->updateSettings();

    // if spell checking was turned on still turn it off for the current note
    // if encrypted text is shown
    if (checked) {
        _mainWindow->updateNoteEncryptionUI();
    }
}

void SpellCheckManager::loadDictionaryNames() {
    SettingsService settings;

    QStringList languages = Sonnet::Speller::availableLanguages();
    QStringList langNames = Sonnet::Speller::availableLanguageNames();

    // if there are no dictionaries installed, disable the spellchecker
    if (languages.isEmpty()) {
        settings.setValue(QStringLiteral("checkSpelling"), false);
        _ui->actionCheck_spelling->setEnabled(false);
        _ui->menuLanguages->setTitle(QStringLiteral("No dictionaries found"));
        _ui->menuLanguages->setEnabled(false);
        _ui->noteTextEdit->updateSettings();
        return;
    }

    _languageGroup->setExclusive(true);
    connect(_languageGroup, &QActionGroup::triggered, this, &SpellCheckManager::onLanguageChanged);

    // first add autoDetect
    QAction *autoDetect = _ui->menuLanguages->addAction(tr("Automatically detect"));
    autoDetect->setCheckable(true);
    autoDetect->setData(QStringLiteral("auto"));
    autoDetect->setActionGroup(_languageGroup);
    QString prevLang =
        settings.value(QStringLiteral("spellCheckLanguage"), QStringLiteral("auto")).toString();
    // if only one dictionary found, disable auto detect
    if (languages.length() > 1) {
        if (prevLang == QStringLiteral("auto")) {
            autoDetect->setChecked(true);
            autoDetect->trigger();
        }
    } else {
        autoDetect->setChecked(false);
        autoDetect->setEnabled(false);
    }

    // not really possible but just in case
    if (langNames.length() != languages.length()) {
        qWarning() << "Error: langNames.length != languages.length()";
        return;
    }

    QStringList::const_iterator it = langNames.constBegin();
    QStringList::const_iterator itt = languages.constBegin();
    for (; it != langNames.constEnd(); ++it, ++itt) {
        QAction *action = _ui->menuLanguages->addAction(*it);
        action->setCheckable(true);
        action->setActionGroup(_languageGroup);
        action->setData(*itt);

        if (*itt == prevLang || languages.length() == 1) {
            action->setChecked(true);
            action->trigger();
        }
    }
}

void SpellCheckManager::onLanguageChanged(QAction *action) {
    QString lang = action->data().toString();
    SettingsService settings;
    settings.setValue(QStringLiteral("spellCheckLanguage"), lang);
    _ui->noteTextEdit->updateSettings();
}

void SpellCheckManager::loadSpellingBackends() {
    SettingsService settings;
    QString prevBackend =
        settings.value(QStringLiteral("spellCheckBackend"), QStringLiteral("Hunspell")).toString();

    _spellBackendGroup->setExclusive(true);
    connect(_spellBackendGroup, &QActionGroup::triggered, this,
            &SpellCheckManager::onSpellBackendChanged);

    QAction *hs = _ui->menuSpelling_backend->addAction(QStringLiteral("Hunspell"));
    hs->setCheckable(true);
    hs->setData("Hunspell");
    hs->setActionGroup(_spellBackendGroup);
    QAction *as = _ui->menuSpelling_backend->addAction(QStringLiteral("Aspell"));
    as->setCheckable(true);
    as->setActionGroup(_spellBackendGroup);
    as->setData("Aspell");

    if (prevBackend == hs->data()) {
        hs->setChecked(true);
    } else {
        as->setChecked(true);
    }
}

void SpellCheckManager::onSpellBackendChanged(QAction *action) {
    QString backend = action->data().toString();
    SettingsService settings;
    settings.setValue(QStringLiteral("spellCheckBackend"), backend);
    _mainWindow->showRestartNotificationIfNeeded(true);
}

void SpellCheckManager::on_actionManage_dictionaries_triggered() {
    auto *dialog = new DictionaryManagerDialog(_mainWindow);
    dialog->exec();
    delete (dialog);

    // shows a restart application notification
    _mainWindow->showRestartNotificationIfNeeded();
}
