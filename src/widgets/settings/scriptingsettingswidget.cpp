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

#include "scriptingsettingswidget.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QMenu>
#include <QPointer>
#include <QSignalBlocker>

#include "dialogs/filedialog.h"
#include "dialogs/scriptrepositorydialog.h"
#include "entities/script.h"
#include "services/scriptingservice.h"
#include "ui_scriptingsettingswidget.h"
#include "utils/gui.h"
#include "utils/misc.h"
#include "widgets/scriptsettingwidget.h"

ScriptingSettingsWidget::ScriptingSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::ScriptingSettingsWidget) {
    ui->setupUi(this);
}

ScriptingSettingsWidget::~ScriptingSettingsWidget() { delete ui; }

/**
 * Sets up the scripting page and prepares the add-script button menu
 */
void ScriptingSettingsWidget::initialize() {
    _newScriptName = tr("New script");

    ui->scriptSearchLineEdit->clear();

    // Reload the script list
    reloadScriptList();

    QString issueUrl = QStringLiteral("https://github.com/pbek/QOwnNotes/issues");
    QString documentationUrl = QStringLiteral("https://www.qownnotes.org/scripting/");
    ui->scriptInfoLabel->setText(tr("Take a look at the <a href=\"%1\">Scripting documentation</a> "
                                    "to get started fast.")
                                     .arg(documentationUrl) +
                                 "<br>" +
                                 tr("If you need access to a certain functionality in "
                                    "QOwnNotes please open an issue on the "
                                    "<a href=\"%1\"> QOwnNotes issue page</a>.")
                                     .arg(issueUrl));

    // Setup the "add script" button menu
    auto *addScriptMenu = new QMenu(this);

    QAction *searchScriptAction = addScriptMenu->addAction(tr("Search script repository"));
    searchScriptAction->setIcon(QIcon::fromTheme(
        QStringLiteral("edit-find"), QIcon(":icons/breeze-qownnotes/16x16/edit-find.svg")));
    searchScriptAction->setToolTip(tr("Find a script in the script repository"));
    connect(searchScriptAction, SIGNAL(triggered()), this, SLOT(searchScriptInRepository()));

    QAction *updateScriptAction = addScriptMenu->addAction(tr("Check for script updates"));
    updateScriptAction->setIcon(QIcon::fromTheme(
        QStringLiteral("svn-update"), QIcon(":icons/breeze-qownnotes/16x16/svn-update.svg")));
    connect(updateScriptAction, SIGNAL(triggered()), this, SLOT(checkForScriptUpdates()));

    QAction *addAction = addScriptMenu->addAction(tr("Add local script"));
    addAction->setIcon(QIcon::fromTheme(QStringLiteral("document-new"),
                                        QIcon(":icons/breeze-qownnotes/16x16/document-new.svg")));
    addAction->setToolTip(tr("Add an existing, local script"));
    connect(addAction, SIGNAL(triggered()), this, SLOT(addLocalScript()));

    ui->scriptAddButton->setMenu(addScriptMenu);
}

/**
 * Stores settings for the scripting page (enabled states of scripts)
 */
void ScriptingSettingsWidget::storeSettings() { storeScriptListEnabledState(); }

/**
 * Reloads the script list
 */
void ScriptingSettingsWidget::reloadScriptList() const {
    QList<Script> scripts = Script::fetchAll();
    int scriptsCount = scripts.count();
    ui->scriptListWidget->clear();

    const QString searchText = ui->scriptSearchLineEdit->text();

    // Populate the script list
    if (scriptsCount > 0) {
        Q_FOREACH (Script script, scripts) {
            if (!searchText.isEmpty() && !scriptMatchesSearchFilter(script, searchText)) {
                continue;
            }
            auto *item = new QListWidgetItem(script.getName());
            item->setData(Qt::UserRole, script.getId());
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(script.getEnabled() ? Qt::Checked : Qt::Unchecked);
            ui->scriptListWidget->addItem(item);
        }

        // Set the current row
        if (ui->scriptListWidget->count() > 0) {
            ui->scriptListWidget->setCurrentRow(0);
        }
    }

    // Disable the edit frame if there is no item
    const bool hasVisibleScripts = ui->scriptListWidget->count() > 0;
    ui->scriptEditFrame->setEnabled(hasVisibleScripts);
    if (hasVisibleScripts) {
        ui->scriptEditFrame->setVisible(true);
    }

    // Disable the remove button if there is no item
    ui->scriptRemoveButton->setEnabled(hasVisibleScripts);
}

bool ScriptingSettingsWidget::scriptMatchesSearchFilter(const Script &script,
                                                        const QString &searchText) {
    const QString trimmedSearchText = searchText.trimmed();

    if (trimmedSearchText.isEmpty()) {
        return true;
    }

    const QString searchTextLower = trimmedSearchText.toLower();

    if (script.getName().toLower().contains(searchTextLower)) {
        return true;
    }

    if (!script.isScriptFromRepository()) {
        return false;
    }

    const ScriptInfoJson infoJson = script.getScriptInfoJson();

    const QString plainDescription = Utils::Misc::unescapeHtml(infoJson.description);
    if (plainDescription.toLower().contains(searchTextLower)) {
        return true;
    }

    if (infoJson.richAuthorText.toLower().contains(searchTextLower)) {
        return true;
    }

    if (infoJson.version.toLower().contains(searchTextLower)) {
        return true;
    }

    if (infoJson.identifier.toLower().contains(searchTextLower)) {
        return true;
    }

    return infoJson.name.toLower().contains(searchTextLower);
}

/**
 * Resets the script metadata fields in the UI
 */
void ScriptingSettingsWidget::clearScriptInfoJsonUi() const {
    ui->scriptVersionLabel->clear();
    ui->scriptDescriptionLabel->clear();
    ui->scriptAuthorsLabel->clear();
    ui->scriptRepositoryLinkLabel->clear();
    ui->textLabel2->setVisible(false);
    ui->scriptRepositoryLinkLabel->setVisible(false);
}

/**
 * Populates the script metadata fields in the UI
 */
void ScriptingSettingsWidget::applyScriptInfoJsonToUi(const ScriptInfoJson &infoJson,
                                                      bool showRepositoryLink) const {
    ui->scriptVersionLabel->setText(infoJson.version);
    ui->scriptDescriptionLabel->setText(infoJson.description);
    ui->scriptAuthorsLabel->setText(infoJson.richAuthorText);

    const bool hasRepositoryLink = showRepositoryLink && !infoJson.identifier.isEmpty();
    ui->textLabel2->setVisible(hasRepositoryLink);
    ui->scriptRepositoryLinkLabel->setVisible(hasRepositoryLink);

    if (hasRepositoryLink) {
        ui->scriptRepositoryLinkLabel->setText(
            "<a href=\"https://github.com/qownnotes/scripts/tree/"
            "master/" +
            infoJson.identifier + "\">" + tr("Open repository") + "</a>");
    } else {
        ui->scriptRepositoryLinkLabel->clear();
    }
}

/**
 * Returns script metadata for a local script if info.json matches the file name
 */
bool ScriptingSettingsWidget::getLocalScriptInfoJson(const QString &scriptPath,
                                                     ScriptInfoJson &infoJson) {
    infoJson = ScriptInfoJson();

    if (scriptPath.isEmpty()) {
        return false;
    }

    const QFileInfo scriptFileInfo(scriptPath);
    if (!scriptFileInfo.exists()) {
        return false;
    }

    QFile infoFile(scriptFileInfo.dir().filePath(QStringLiteral("info.json")));
    if (!infoFile.exists() || !infoFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    const QJsonDocument jsonDocument = QJsonDocument::fromJson(infoFile.readAll());
    if (!jsonDocument.isObject()) {
        return false;
    }

    const ScriptInfoJson localInfoJson(jsonDocument.object());
    if (QFileInfo(localInfoJson.script).fileName() != scriptFileInfo.fileName()) {
        return false;
    }

    infoJson = localInfoJson;
    return true;
}

void ScriptingSettingsWidget::on_scriptSearchLineEdit_textChanged(const QString &arg1) {
    Q_UNUSED(arg1)
    reloadScriptList();
}

/**
 * Adds a new local script
 */
void ScriptingSettingsWidget::addLocalScript() {
    _selectedScript = Script();
    _selectedScript.setName(_newScriptName);
    _selectedScript.setPriority(ui->scriptListWidget->count());
    _selectedScript.store();

    if (_selectedScript.isFetched()) {
        auto *item = new QListWidgetItem(_selectedScript.getName());
        item->setData(Qt::UserRole, _selectedScript.getId());
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);
        ui->scriptListWidget->addItem(item);

        // Set the current row
        ui->scriptListWidget->setCurrentRow(ui->scriptListWidget->count() - 1);

        // Enable the remove button
        ui->scriptRemoveButton->setEnabled(true);

        // Focus the script name edit and select the text
        ui->scriptNameLineEdit->setFocus();
        ui->scriptNameLineEdit->selectAll();

        // Open the dialog to select the script
        on_scriptPathButton_clicked();
    }
}

/**
 * Removes the current script
 */
void ScriptingSettingsWidget::on_scriptRemoveButton_clicked() {
    if (ui->scriptListWidget->count() < 1) {
        return;
    }

    if (Utils::Gui::question(
            this, tr("Remove script"),
            tr("Remove the current script <strong>%1</strong>?").arg(_selectedScript.getName()),
            QStringLiteral("remove-script")) == QMessageBox::Yes) {
        // Remove the script from the database
        _selectedScript.remove();

        // Remove the list item
        ui->scriptListWidget->takeItem(ui->scriptListWidget->currentRow());

        bool scriptsAvailable = ui->scriptListWidget->count() > 0;
        // Disable the remove button if there is no item left
        ui->scriptRemoveButton->setEnabled(scriptsAvailable);

        // Disable the edit frame if there is no item
        ui->scriptEditFrame->setEnabled(scriptsAvailable);
        ui->scriptEditFrame->setVisible(scriptsAvailable);

        // Reload the scripting engine
        ScriptingService::instance()->reloadEngine();
    }
}

/**
 * Allows the user to choose the script path
 */
void ScriptingSettingsWidget::on_scriptPathButton_clicked() {
    QString path = ui->scriptPathLineEdit->text();
    QString dirPath = path;

    // Get the path of the script if a script was set
    if (!path.isEmpty()) {
        dirPath = QFileInfo(path).dir().path();
    }

    // In portable mode the data path will be opened if path was empty
    if (path.isEmpty() && Utils::Misc::isInPortableMode()) {
        dirPath = Utils::Misc::portableDataPath();
    }

    FileDialog dialog(QStringLiteral("ScriptPath"));

    if (!dirPath.isEmpty()) {
        dialog.setDirectory(dirPath);
    }

    if (!path.isEmpty()) {
        dialog.selectFile(path);
    }

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(tr("QML files") + " (*.qml)");
    dialog.setWindowTitle(tr("Please select your QML file"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        path = dialog.selectedFile();

        QFile file(path);

        if (file.exists() && (!path.isEmpty())) {
            QString scriptName = _selectedScript.getName();

            // Set the script name from the file name if none was set yet
            if (scriptName.isEmpty() || (scriptName == _newScriptName)) {
                scriptName = QFileInfo(file).baseName();
                ui->scriptNameLineEdit->setText(scriptName);
                ui->scriptNameLabel->setText(scriptName);
                _selectedScript.setName(scriptName);

                const QSignalBlocker blocker(ui->scriptListWidget);
                Q_UNUSED(blocker)
                ui->scriptListWidget->currentItem()->setText(scriptName);
            }

            ui->scriptPathLineEdit->setText(path);
            _selectedScript.setScriptPath(path);
            _selectedScript.store();

            // Validate the script
            validateCurrentScript();

            // Reload the scripting engine
            ScriptingService::instance()->reloadEngine();

            // Trigger the item change so that the page is reloaded for
            // script variables
            reloadCurrentScriptPage();
        }
    }
}

/**
 * Loads the current script in the UI when the current item changed
 */
void ScriptingSettingsWidget::on_scriptListWidget_currentItemChanged(QListWidgetItem *current,
                                                                     QListWidgetItem *previous) {
    Q_UNUSED(current)
    Q_UNUSED(previous)

    reloadCurrentScriptPage();
}

/**
 * Loads the current script in the UI
 */
void ScriptingSettingsWidget::reloadCurrentScriptPage() {
    QListWidgetItem *item = ui->scriptListWidget->currentItem();

    clearScriptInfoJsonUi();
    ui->scriptRepositoryItemFrame->setVisible(false);

    if (item == nullptr) {
        return;
    }

    ui->scriptValidationLabel->clear();

    int scriptId = item->data(Qt::UserRole).toInt();
    _selectedScript = Script::fetch(scriptId);
    if (_selectedScript.isFetched()) {
        ui->scriptNameLabel->setText("<b>" + _selectedScript.getName() + "</b>");
        ui->scriptPathLineEdit->setText(_selectedScript.getScriptPath());
        ui->scriptEditFrame->setEnabled(true);

        bool isScriptFromRepository = _selectedScript.isScriptFromRepository();
        ScriptInfoJson localInfoJson;
        const bool hasLocalInfoJson =
            !isScriptFromRepository &&
            getLocalScriptInfoJson(_selectedScript.getScriptPath(), localInfoJson);

        ui->scriptNameLineEdit->setReadOnly(isScriptFromRepository);
        ui->scriptPathButton->setDisabled(isScriptFromRepository);
        ui->scriptRepositoryItemFrame->setVisible(isScriptFromRepository || hasLocalInfoJson);
        ui->localScriptItemFrame->setHidden(isScriptFromRepository);
        ui->repositoryScriptItemFrame->setHidden(!isScriptFromRepository);
        ui->scriptNameLineEdit->setHidden(isScriptFromRepository);
        ui->scriptNameLineEditLabel->setHidden(isScriptFromRepository);

        // Add additional information if script was from the script repository
        if (isScriptFromRepository) {
            ScriptInfoJson infoJson = _selectedScript.getScriptInfoJson();
            applyScriptInfoJsonToUi(infoJson, true);
        } else {
            ui->scriptNameLineEdit->setText(_selectedScript.getName());

            if (hasLocalInfoJson) {
                applyScriptInfoJsonToUi(localInfoJson, false);
            }
        }

        // Get the registered script settings variables
        QList<QVariant> variables =
            ScriptingService::instance()->getSettingsVariables(_selectedScript.getId());

        bool hasScriptSettings = variables.count() > 0;
        ui->scriptSettingsFrame->setVisible(hasScriptSettings);

        if (hasScriptSettings) {
            // Remove the current ScriptSettingWidget widgets in the scriptSettingsFrame
            QList<ScriptSettingWidget *> widgets =
                ui->scriptSettingsFrame->findChildren<ScriptSettingWidget *>();
            Q_FOREACH (ScriptSettingWidget *widget, widgets) {
                delete widget;
            }

            foreach (QVariant variable, variables) {
                QMap<QString, QVariant> varMap = variable.toMap();

                // Populate the variable UI
                ScriptSettingWidget *scriptSettingWidget =
                    new ScriptSettingWidget(this, _selectedScript, varMap);

                ui->scriptSettingsFrame->layout()->addWidget(scriptSettingWidget);
            }
        }

        // Validate the script
        validateCurrentScript();
    } else {
        ui->scriptEditFrame->setEnabled(false);
        ui->scriptRepositoryItemFrame->setVisible(false);
        ui->scriptNameLineEdit->clear();
        ui->scriptPathLineEdit->clear();
    }
}

/**
 * Validates the current script
 */
void ScriptingSettingsWidget::validateCurrentScript() {
    ui->scriptValidationLabel->clear();

    if (_selectedScript.isFetched()) {
        QString path = _selectedScript.getScriptPath();

        // Check the script validity if the path is not empty
        if (!path.isEmpty()) {
            QString errorMessage;
            bool result = ScriptingService::validateScript(_selectedScript, errorMessage);
            QString validationText = result ? tr("Your script seems to be valid")
                                            : tr("There were script errors:\n%1").arg(errorMessage);
            ui->scriptValidationLabel->setText(validationText);
            ui->scriptValidationLabel->setStyleSheet(
                QStringLiteral("color: %1;").arg(result ? "green" : "red"));
        }
    }
}

/**
 * Stores a script name after it was edited
 */
void ScriptingSettingsWidget::on_scriptNameLineEdit_editingFinished() {
    QString text = ui->scriptNameLineEdit->text();
    _selectedScript.setName(text);
    _selectedScript.store();

    ui->scriptListWidget->currentItem()->setText(text);
}

/**
 * Stores the enabled states of the scripts
 */
void ScriptingSettingsWidget::storeScriptListEnabledState() {
    for (int i = 0; i < ui->scriptListWidget->count(); i++) {
        QListWidgetItem *item = ui->scriptListWidget->item(i);
        bool enabled = item->checkState() == Qt::Checked;
        int scriptId = item->data(Qt::UserRole).toInt();

        Script script = Script::fetch(scriptId);
        if (script.isFetched()) {
            if (script.getEnabled() != enabled) {
                script.setEnabled(enabled);
                script.store();
            }
        }
    }

    // Reload the scripting engine
    ScriptingService::instance()->reloadEngine();
}

void ScriptingSettingsWidget::on_scriptValidationButton_clicked() { validateCurrentScript(); }

/**
 * Reloads the scripting engine
 */
void ScriptingSettingsWidget::on_scriptReloadEngineButton_clicked() {
    // Store the enabled states and reload the scripting engine
    storeScriptListEnabledState();

    // Trigger the item change so that the page is reloaded for script variables
    reloadCurrentScriptPage();
}

void ScriptingSettingsWidget::on_scriptReloadEngineButton2_clicked() {
    on_scriptReloadEngineButton_clicked();
}

/**
 * Saves the enabled state of all items and reload the current script page to
 * make the script settings available when a script was enabled or disabled
 */
void ScriptingSettingsWidget::on_scriptListWidget_itemChanged(QListWidgetItem *item) {
    Q_UNUSED(item)

    storeScriptListEnabledState();
    reloadCurrentScriptPage();
}

/**
 * Opens the script repository dialog to search for scripts
 */
void ScriptingSettingsWidget::searchScriptInRepository(bool checkForUpdates) {
    QPointer<ScriptRepositoryDialog> dialog = new ScriptRepositoryDialog(this, checkForUpdates);
    dialog->exec();

    if (!dialog) {
        return;
    }

    Script lastInstalledScript = dialog->getLastInstalledScript();
    delete dialog;

    // Reload the script list
    reloadScriptList();

    // Select the last installed script
    if (lastInstalledScript.isFetched()) {
        auto item = Utils::Gui::getListWidgetItemWithUserData(ui->scriptListWidget,
                                                              lastInstalledScript.getId());
        ui->scriptListWidget->setCurrentItem(item);
    }

    // Reload the scripting engine
    ScriptingService::instance()->reloadEngine();

    // Reload page so the script settings will be viewed
    reloadCurrentScriptPage();
}

/**
 * Opens a dialog to check for script updates
 */
void ScriptingSettingsWidget::checkForScriptUpdates() {
    ScriptRepositoryDialog::checkForScriptUpdates(this);
}
