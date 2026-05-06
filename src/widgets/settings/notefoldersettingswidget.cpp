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

#include "notefoldersettingswidget.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>

#include "dialogs/filedialog.h"
#include "dialogs/settingsdialog.h"
#include "entities/notesubfolder.h"
#include "mainwindow.h"
#include "services/cloudservice.h"
#include "services/settingsservice.h"
#include "ui_notefoldersettingswidget.h"
#include "utils/gui.h"
#include "utils/misc.h"

NoteFolderSettingsWidget::NoteFolderSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::NoteFolderSettingsWidget) {
    ui->setupUi(this);
}

NoteFolderSettingsWidget::~NoteFolderSettingsWidget() { delete ui; }

/**
 * Sets up the note folder page: populates the list, selects the current folder,
 * and creates the status bar for the remote path tree widget.
 */
void NoteFolderSettingsWidget::initialize() {
    ui->noteFolderEditFrame->setEnabled(NoteFolder::countAll() > 0);
    setNoteFolderRemotePathTreeWidgetFrameVisibility(false);

    QList<NoteFolder> noteFolders = NoteFolder::fetchAll();
    int noteFoldersCount = noteFolders.count();

    // Populate the note folder list
    if (noteFoldersCount > 0) {
        Q_FOREACH (NoteFolder noteFolder, noteFolders) {
            auto *item = new QListWidgetItem(noteFolder.getName());
            item->setData(Qt::UserRole, noteFolder.getId());
            ui->noteFolderListWidget->addItem(item);

            // Set the current row
            if (noteFolder.getId() == NoteFolder::currentNoteFolderId()) {
                ui->noteFolderListWidget->setCurrentItem(item);
            }
        }
    }

    // Disable the remove button if there is only one item
    ui->noteFolderRemoveButton->setEnabled(noteFoldersCount > 1);

    // Set local path placeholder text
    ui->noteFolderLocalPathLineEdit->setPlaceholderText(Utils::Misc::defaultNotesPath());

    _noteFolderRemotePathTreeStatusBar = new QStatusBar(this);
    ui->noteFolderRemotePathTreeWidgetFrame->layout()->addWidget(
        _noteFolderRemotePathTreeStatusBar);
}

/**
 * Sets the currently selected note folder item in the list according to the
 * active note folder. Called from readSettings() after initialize().
 */
void NoteFolderSettingsWidget::readSettings() {
    QListWidgetItem *noteFolderListItem = Utils::Gui::getListWidgetItemWithUserData(
        ui->noteFolderListWidget, NoteFolder::currentNoteFolderId());
    if (noteFolderListItem != nullptr) {
        ui->noteFolderListWidget->setCurrentItem(noteFolderListItem);
    }
}

/**
 * No batch settings to store — all changes are saved immediately in slot handlers.
 */
void NoteFolderSettingsWidget::storeSettings() {}

/**
 * Populates the cloud connection combo box with the given connections.
 * Called by the dialog's initCloudConnectionComboBox().
 */
void NoteFolderSettingsWidget::populateCloudConnectionComboBox(
    const QList<CloudConnection> &connections, int selectedId) {
    const QSignalBlocker blocker(ui->noteFolderCloudConnectionComboBox);
    Q_UNUSED(blocker)

    ui->noteFolderCloudConnectionComboBox->clear();
    Q_FOREACH (CloudConnection cloudConnection, connections) {
        ui->noteFolderCloudConnectionComboBox->addItem(cloudConnection.getName(),
                                                       cloudConnection.getId());
    }

    Utils::Gui::setComboBoxIndexByUserData(ui->noteFolderCloudConnectionComboBox, selectedId);
}

void NoteFolderSettingsWidget::on_noteFolderListWidget_currentItemChanged(
    QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous)

    setNoteFolderRemotePathTreeWidgetFrameVisibility(false);

    int noteFolderId = current->data(Qt::UserRole).toInt();
    _selectedNoteFolder = NoteFolder::fetch(noteFolderId);
    if (_selectedNoteFolder.isFetched()) {
        ui->noteFolderNameLineEdit->setText(_selectedNoteFolder.getName());
        ui->noteFolderLocalPathLineEdit->setText(_selectedNoteFolder.getLocalPath());
        ui->noteFolderRemotePathLineEdit->setText(_selectedNoteFolder.getRemotePath());
        ui->noteFolderShowSubfoldersCheckBox->setChecked(_selectedNoteFolder.isShowSubfolders());
        ui->noteFolderAllSubfoldersCheckBox->setChecked(_selectedNoteFolder.isAllSubfolders());
        ui->allowDifferentNoteFileNameCheckBox->setChecked(
            _selectedNoteFolder.settingsValue(QStringLiteral("allowDifferentNoteFileName"))
                .toBool());
        ui->noteFolderGitCommitCheckBox->setChecked(_selectedNoteFolder.isUseGit());
        Utils::Gui::setComboBoxIndexByUserData(ui->noteFolderCloudConnectionComboBox,
                                               _selectedNoteFolder.getCloudConnectionId());

        const QSignalBlocker blocker(ui->noteFolderActiveCheckBox);
        Q_UNUSED(blocker)
        ui->noteFolderActiveCheckBox->setChecked(_selectedNoteFolder.isCurrent());
    }

    updateSubfolderVisibility();
}

void NoteFolderSettingsWidget::on_noteFolderAddButton_clicked() {
    const int cloudConnectionId = _selectedNoteFolder.getCloudConnectionId();
    const QString currentPath = _selectedNoteFolder.getLocalPath();

    _selectedNoteFolder = NoteFolder();
    _selectedNoteFolder.setName(tr("new folder"));
    _selectedNoteFolder.setLocalPath(currentPath);
    _selectedNoteFolder.setPriority(ui->noteFolderListWidget->count());
    _selectedNoteFolder.setCloudConnectionId(cloudConnectionId);
    _selectedNoteFolder.suggestRemotePath();
    _selectedNoteFolder.store();

    if (_selectedNoteFolder.isFetched()) {
        auto *item = new QListWidgetItem(_selectedNoteFolder.getName());
        item->setData(Qt::UserRole, _selectedNoteFolder.getId());
        ui->noteFolderListWidget->addItem(item);

        // Set the current row
        ui->noteFolderListWidget->setCurrentRow(ui->noteFolderListWidget->count() - 1);

        // Enable the remove button
        ui->noteFolderRemoveButton->setEnabled(true);

        // Focus the folder name edit and select the text
        ui->noteFolderNameLineEdit->setFocus();
        ui->noteFolderNameLineEdit->selectAll();
    }
}

/**
 * Removes the current note folder
 */
void NoteFolderSettingsWidget::on_noteFolderRemoveButton_clicked() {
    if (ui->noteFolderListWidget->count() < 2) {
        return;
    }

    if (Utils::Gui::question(this, tr("Remove note folder"),
                             tr("Remove the current note folder <strong>%1</strong>?")
                                 .arg(_selectedNoteFolder.getName()),
                             QStringLiteral("remove-note-folder")) == QMessageBox::Yes) {
        bool wasCurrent = _selectedNoteFolder.isCurrent();

        SettingsService settings;

        // Remove saved searches
        QString settingsKey =
            "savedSearches/noteFolder-" + QString::number(_selectedNoteFolder.getId());
        settings.remove(settingsKey);

        // Remove tree widget expand state setting
        settingsKey = NoteSubFolder::treeWidgetExpandStateSettingsKey(_selectedNoteFolder.getId());
        settings.remove(settingsKey);

        // Remove the note folder from the database
        _selectedNoteFolder.remove();

        // Remove the list item
        ui->noteFolderListWidget->takeItem(ui->noteFolderListWidget->currentRow());

        // Disable the remove button if there is only one item left
        ui->noteFolderRemoveButton->setEnabled(ui->noteFolderListWidget->count() > 1);

        // If the removed note folder was the current folder, set the first as new current
        if (wasCurrent) {
            QList<NoteFolder> noteFolders = NoteFolder::fetchAll();
            if (noteFolders.count() > 0) {
                noteFolders[0].setAsCurrent();
            }
        }
    }
}

/**
 * Updates the name of the current note folder edit
 */
void NoteFolderSettingsWidget::on_noteFolderNameLineEdit_editingFinished() {
    QString text = ui->noteFolderNameLineEdit->text().remove(QStringLiteral("\n")).trimmed();
    text.truncate(50);

    // Fallback to directory name in case name edit is empty
    if (text.isEmpty()) {
        const QString localPath = ui->noteFolderLocalPathLineEdit->text();
        text = QDir(localPath).dirName();
    }

    _selectedNoteFolder.setName(text);
    _selectedNoteFolder.store();

    ui->noteFolderListWidget->currentItem()->setText(text);
}

/**
 * Updates the remote path of the current note folder edit
 */
void NoteFolderSettingsWidget::on_noteFolderRemotePathLineEdit_editingFinished() {
    QString text = ui->noteFolderRemotePathLineEdit->text();
    _selectedNoteFolder.setRemotePath(text);
    QString remotePath = _selectedNoteFolder.fixRemotePath();
    _selectedNoteFolder.store();

    // Set new path if fixed path differs
    if (text != remotePath) {
        const QSignalBlocker blocker(ui->noteFolderRemotePathLineEdit);
        Q_UNUSED(blocker)

        ui->noteFolderRemotePathLineEdit->setText(remotePath);
    }
}

void NoteFolderSettingsWidget::on_noteFolderLocalPathButton_clicked() {
    QString dir = QFileDialog::getExistingDirectory(
        this, tr("Please select the folder where your notes will get stored to"),
        _selectedNoteFolder.getLocalPath(), QFileDialog::ShowDirsOnly);

    QDir d = QDir(dir);

    if (d.exists() && (!dir.isEmpty())) {
        ui->noteFolderLocalPathLineEdit->setText(dir);
        _selectedNoteFolder.setLocalPath(dir);
        _selectedNoteFolder.store();
    }
}

/**
 * Sets the current note folder as active note folder
 */
void NoteFolderSettingsWidget::on_noteFolderActiveCheckBox_stateChanged(int arg1) {
    Q_UNUSED(arg1)

    if (!ui->noteFolderActiveCheckBox->isChecked()) {
        const QSignalBlocker blocker(ui->noteFolderActiveCheckBox);
        Q_UNUSED(blocker)
        ui->noteFolderActiveCheckBox->setChecked(true);
    } else {
        _selectedNoteFolder.setAsCurrent();
        MainWindow::instance()->resetBrokenTagNotesLinkFlag();
    }
}

void NoteFolderSettingsWidget::on_noteFolderRemotePathButton_clicked() {
    // Request the dialog to store settings so the Cloud connection is up-to-date
    Q_EMIT storeSettingsRequested();

    setNoteFolderRemotePathTreeWidgetFrameVisibility(true);

    _noteFolderRemotePathTreeStatusBar->showMessage(tr("Loading folders from server"));

    CloudService *cloud = CloudService::instance(true, _selectedNoteFolder.getCloudConnectionId());
    cloud->settingsGetFileList(qobject_cast<SettingsDialog *>(window()), QLatin1String(""));
}

/**
 * Populates the note folder remote path tree with items.
 * Callback function from CloudService::loadDirectory() via SettingsDialog.
 */
void NoteFolderSettingsWidget::setNoteFolderRemotePathList(QStringList pathList) {
    if (pathList.count() <= 1) {
        _noteFolderRemotePathTreeStatusBar->showMessage(
            tr("No more folders were found in the current folder"), 1000);
    } else {
        _noteFolderRemotePathTreeStatusBar->clearMessage();
    }

    Q_FOREACH (QString path, pathList) {
        if (!path.isEmpty()) {
            addPathToNoteFolderRemotePathTreeWidget(nullptr, path);
        }
    }
}

void NoteFolderSettingsWidget::addPathToNoteFolderRemotePathTreeWidget(QTreeWidgetItem *parent,
                                                                       const QString &path) {
    if (path.isEmpty()) {
        return;
    }

    QStringList pathPartList = path.split(QStringLiteral("/"));
    QString pathPart = pathPartList.takeFirst();
    QTreeWidgetItem *item = findNoteFolderRemotePathTreeWidgetItem(parent, pathPart);

    const QSignalBlocker blocker(ui->noteFolderRemotePathTreeWidget);
    Q_UNUSED(blocker)

    if (item == nullptr) {
        item = new QTreeWidgetItem();
        item->setText(0, pathPart);
        if (parent == nullptr) {
            ui->noteFolderRemotePathTreeWidget->addTopLevelItem(item);
        } else {
            parent->addChild(item);
            parent->setExpanded(true);
        }
    }

    if (pathPartList.count() > 0) {
        addPathToNoteFolderRemotePathTreeWidget(item, pathPartList.join(QStringLiteral("/")));
    }
}

QTreeWidgetItem *NoteFolderSettingsWidget::findNoteFolderRemotePathTreeWidgetItem(
    QTreeWidgetItem *parent, const QString &text) {
    if (parent == nullptr) {
        for (int i = 0; i < ui->noteFolderRemotePathTreeWidget->topLevelItemCount(); i++) {
            QTreeWidgetItem *item = ui->noteFolderRemotePathTreeWidget->topLevelItem(i);
            if (item->text(0) == text) {
                return item;
            }
        }
    } else {
        for (int i = 0; i < parent->childCount(); i++) {
            QTreeWidgetItem *item = parent->child(i);
            if (item->text(0) == text) {
                return item;
            }
        }
    }

    return nullptr;
}

void NoteFolderSettingsWidget::on_noteFolderRemotePathTreeWidget_currentItemChanged(
    QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous)

    QString folderName = generatePathFromCurrentNoteFolderRemotePathItem(current);
    _noteFolderRemotePathTreeStatusBar->showMessage(
        tr("Loading folders in '%1' from server").arg(current->text(0)));

    CloudService *cloud = CloudService::instance(true, _selectedNoteFolder.getCloudConnectionId());
    cloud->settingsGetFileList(qobject_cast<SettingsDialog *>(window()), folderName);
}

void NoteFolderSettingsWidget::on_noteFolderCloudConnectionComboBox_currentIndexChanged(int index) {
    Q_UNUSED(index)
    _selectedNoteFolder.setCloudConnectionId(
        ui->noteFolderCloudConnectionComboBox->currentData().toInt());
    _selectedNoteFolder.store();

    // If there already were fetched remote folders, fetch them again
    if (ui->noteFolderRemotePathTreeWidgetFrame->isVisible()) {
        on_noteFolderRemotePathButton_clicked();
    }
}

void NoteFolderSettingsWidget::on_useCloudPathButton_clicked() {
    QTreeWidgetItem *item = ui->noteFolderRemotePathTreeWidget->currentItem();
    if (item == nullptr) {
        return;
    }

    ui->noteFolderRemotePathLineEdit->clear();
    ui->noteFolderRemotePathLineEdit->setText(
        generatePathFromCurrentNoteFolderRemotePathItem(item));
    setNoteFolderRemotePathTreeWidgetFrameVisibility(false);
    on_noteFolderRemotePathLineEdit_editingFinished();
}

/**
 * Recursively generates the path string from the tree widget items
 */
QString NoteFolderSettingsWidget::generatePathFromCurrentNoteFolderRemotePathItem(
    QTreeWidgetItem *item) {
    if (item == nullptr) {
        return QString();
    }

    QTreeWidgetItem *parent = item->parent();
    if (parent != nullptr) {
        return generatePathFromCurrentNoteFolderRemotePathItem(parent) + QStringLiteral("/") +
               item->text(0);
    }

    return item->text(0);
}

void NoteFolderSettingsWidget::setNoteFolderRemotePathTreeWidgetFrameVisibility(bool visible) {
    ui->noteFolderRemotePathTreeWidgetFrame->setVisible(visible);
    ui->noteFolderVerticalSpacerFrame->setVisible(!visible);

    const QSignalBlocker blocker(ui->noteFolderRemotePathTreeWidget);
    Q_UNUSED(blocker)
    ui->noteFolderRemotePathTreeWidget->clear();
}

void NoteFolderSettingsWidget::on_noteFolderShowSubfoldersCheckBox_toggled(bool checked) {
    _selectedNoteFolder.setShowSubfolders(checked);

    // Reset the active note subfolder if showing subfolders was turned off
    if (!checked) {
        _selectedNoteFolder.resetActiveNoteSubFolder();
    }

    _selectedNoteFolder.store();

    updateSubfolderVisibility();
}

void NoteFolderSettingsWidget::on_noteFolderAllSubfoldersCheckBox_toggled(bool checked) {
    _selectedNoteFolder.setAllSubfolders(checked);

    updateSubfolderVisibility();
}

/**
 * Updates the visibility of the "All subfolders" checkbox and subfolder tree
 * based on the current "Use note subfolders" and "All subfolders" settings
 */
void NoteFolderSettingsWidget::updateSubfolderVisibility() {
    const bool showSubfolders = ui->noteFolderShowSubfoldersCheckBox->isChecked();
    const bool allSubfolders = ui->noteFolderAllSubfoldersCheckBox->isChecked();

    // Show the entire subfolder settings frame only when subfolders are enabled
    ui->noteFolderSubfolderSettingsFrame->setVisible(showSubfolders);
    ui->noteFolderSubfolderTreeWidget->setVisible(showSubfolders && !allSubfolders);

    if (showSubfolders && !allSubfolders) {
        populateSubfolderTree();
    }
}

/**
 * Populates the subfolder tree widget from the file system
 */
void NoteFolderSettingsWidget::populateSubfolderTree() {
    ui->noteFolderSubfolderTreeWidget->clear();

    const QString localPath = _selectedNoteFolder.getLocalPath();
    if (localPath.isEmpty() || !QDir(localPath).exists()) {
        return;
    }

    // Disconnect to avoid saving while populating
    disconnect(ui->noteFolderSubfolderTreeWidget, &QTreeWidget::itemChanged, this,
               &NoteFolderSettingsWidget::saveSubfolderTreeSelection);

    const QStringList excludedPaths = _selectedNoteFolder.excludedSubfolderPaths();

    // First pass: build the tree without check states (avoid auto-tristate interference)
    populateSubfolderTreeFromDir(nullptr, localPath, QString());

    // Second pass: apply check states bottom-up so auto-tristate works correctly
    applySubfolderTreeCheckStates(ui->noteFolderSubfolderTreeWidget, excludedPaths);

    ui->noteFolderSubfolderTreeWidget->expandAll();

    // Reconnect
    connect(ui->noteFolderSubfolderTreeWidget, &QTreeWidget::itemChanged, this,
            &NoteFolderSettingsWidget::saveSubfolderTreeSelection);
}

/**
 * Recursively populates subfolder tree items from a directory (without setting check states)
 */
void NoteFolderSettingsWidget::populateSubfolderTreeFromDir(QTreeWidgetItem *parentItem,
                                                            const QString &path,
                                                            const QString &relativePath) {
    QDir dir(path);
    const QStringList folders =
        dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden, QDir::Name);

    for (const QString &folder : folders) {
        if (NoteSubFolder::willFolderBeIgnored(folder)) {
            continue;
        }

        const QString childRelPath =
            relativePath.isEmpty() ? folder : relativePath + QLatin1Char('/') + folder;
        const QString childFullPath = path + QDir::separator() + folder;

        auto *item = new QTreeWidgetItem();
        item->setText(0, folder);
        item->setData(0, Qt::UserRole, childRelPath);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsAutoTristate);

        if (parentItem) {
            parentItem->addChild(item);
        } else {
            ui->noteFolderSubfolderTreeWidget->addTopLevelItem(item);
        }

        // Recurse into subdirectories
        populateSubfolderTreeFromDir(item, childFullPath, childRelPath);
    }
}

/**
 * Applies check states to subfolder tree items bottom-up.
 * Items whose path (or an ancestor path) is in excludedPaths are unchecked.
 * Processing bottom-up ensures auto-tristate computes parent states correctly.
 */
void NoteFolderSettingsWidget::applySubfolderTreeCheckStates(QTreeWidget *tree,
                                                             const QStringList &excludedPaths) {
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        applyCheckStateToItem(tree->topLevelItem(i), excludedPaths);
    }
}

/**
 * Recursively applies check states to a tree item and its children (bottom-up).
 * Children are processed first so auto-tristate computes parent states correctly.
 */
void NoteFolderSettingsWidget::applyCheckStateToItem(QTreeWidgetItem *item,
                                                     const QStringList &excludedPaths) {
    const QString path = item->data(0, Qt::UserRole).toString();

    // Check if this path or any ancestor is in the excluded list
    bool excluded = false;
    for (const QString &excludedPath : excludedPaths) {
        if (path == excludedPath || path.startsWith(excludedPath + QLatin1Char('/'))) {
            excluded = true;
            break;
        }
    }

    if (excluded) {
        // Mark excluded branches recursively so every visible item gets a checkbox
        for (int i = 0; i < item->childCount(); ++i) {
            applyCheckStateToItem(item->child(i), excludedPaths);
        }

        item->setCheckState(0, Qt::Unchecked);
        return;
    }

    // Process children first (bottom-up)
    for (int i = 0; i < item->childCount(); ++i) {
        applyCheckStateToItem(item->child(i), excludedPaths);
    }

    // Leaf nodes: set checked explicitly; non-leaf: auto-tristate computes from children
    if (item->childCount() == 0) {
        item->setCheckState(0, Qt::Checked);
    }
}

/**
 * Saves the subfolder tree selection when an item check state changes
 */
void NoteFolderSettingsWidget::saveSubfolderTreeSelection() {
    QStringList excludedPaths;

    for (int i = 0; i < ui->noteFolderSubfolderTreeWidget->topLevelItemCount(); ++i) {
        collectExcludedSubfolderPaths(ui->noteFolderSubfolderTreeWidget->topLevelItem(i),
                                      excludedPaths);
    }

    _selectedNoteFolder.setExcludedSubfolderPaths(excludedPaths);
}

/**
 * Recursively collects paths of unchecked subfolder tree items.
 * Only stores the topmost unchecked parent (children are excluded implicitly).
 */
void NoteFolderSettingsWidget::collectExcludedSubfolderPaths(QTreeWidgetItem *item,
                                                             QStringList &excludedPaths) {
    const QString relativePath = item->data(0, Qt::UserRole).toString();

    if (item->checkState(0) == Qt::Unchecked) {
        // This folder and all children are excluded — only store this path
        excludedPaths.append(relativePath);
        return;
    }

    // If partially or fully checked, recurse into children
    for (int i = 0; i < item->childCount(); ++i) {
        collectExcludedSubfolderPaths(item->child(i), excludedPaths);
    }
}

void NoteFolderSettingsWidget::on_allowDifferentNoteFileNameCheckBox_toggled(bool checked) {
    _selectedNoteFolder.setSettingsValue(QStringLiteral("allowDifferentNoteFileName"), checked);
}

void NoteFolderSettingsWidget::on_noteFolderGitCommitCheckBox_toggled(bool checked) {
    _selectedNoteFolder.setUseGit(checked);
    _selectedNoteFolder.store();
}
