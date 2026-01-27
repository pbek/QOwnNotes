#include "storedattachmentsdialog.h"

#include <entities/note.h>
#include <entities/notefolder.h>
#include <entities/notesubfolder.h>
#include <mainwindow.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QMenu>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QtCore/QMimeDatabase>
#include <QtCore/QMimeType>
#include <QtWidgets/QMessageBox>

#include "ui_storedattachmentsdialog.h"
#include "widgets/qownnotesmarkdowntextedit.h"

StoredAttachmentsDialog::StoredAttachmentsDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::StoredAttachmentsDialog) {
    ui->setupUi(this);
    afterSetupUI();
    ui->infoFrame->setEnabled(false);
    ui->fileTreeWidget->installEventFilter(this);

    refreshAttachmentFiles();
}

StoredAttachmentsDialog::~StoredAttachmentsDialog() { delete ui; }

void StoredAttachmentsDialog::refreshAttachmentFiles() {
    QDir attachmentsDir(NoteFolder::currentAttachmentsPath());

    if (!attachmentsDir.exists()) {
        ui->progressBar->setValue(ui->progressBar->maximum());
        return;
    }

    QStringList attachmentFiles;
    QVector<Note> noteList;

    if (_currentNoteOnly) {
        MainWindow *mainWindow = MainWindow::instance();
        if (mainWindow == nullptr) {
            return;
        }

        const auto note = mainWindow->getCurrentNote();
        if (note.isFetched()) {
            attachmentFiles = note.getAttachmentsFileList();
            noteList = {note};
        }
    } else {
        attachmentFiles =
            attachmentsDir.entryList(QStringList(QStringLiteral("*")), QDir::Files, QDir::Time);
        noteList = Note::fetchAll();
    }

    attachmentFiles.removeDuplicates();
    int noteListCount = noteList.count();
    _fileNoteList.clear();

    ui->progressBar->setMaximum(noteListCount);
    ui->progressBar->show();

    Q_FOREACH (Note note, noteList) {
        QStringList attachmentsFileList = note.getAttachmentsFileList();
        // we don't want to keep the whole note text
        note.setNoteText("");

        Q_FOREACH (const QString &fileName, attachmentsFileList) {
            // remove all found attachments from the files list if _orphanedAttachmentsOnly is
            // enabled
            if (_orphanedAttachmentsOnly) {
                attachmentFiles.removeAll(fileName);
            }

            if (!_fileNoteList[fileName].contains(note)) {
                _fileNoteList[fileName].append(note);
            }
        }

        ui->progressBar->setValue(ui->progressBar->value() + 1);
    }

    ui->progressBar->hide();
    ui->fileTreeWidget->clear();

    Q_FOREACH (QString fileName, attachmentFiles) {
        auto *item = new QTreeWidgetItem();
        item->setText(0, fileName);
        item->setData(0, Qt::UserRole, fileName);
        item->setFlags(item->flags() | Qt::ItemIsEditable);

        QString filePath = getFilePath(item);
        QFileInfo info(filePath);
        item->setToolTip(0, tr("Last modified at %1").arg(info.lastModified().toString()) +
                                QStringLiteral("\n") +
                                Utils::Misc::toHumanReadableByteSize(info.size()));

        ui->fileTreeWidget->addTopLevelItem(item);
    }

    // Re-apply search text
    on_searchLineEdit_textChanged(ui->searchLineEdit->text());

    ui->fileTreeWidget->sortItems(0, Qt::AscendingOrder);

    // jump to the first item
    if (attachmentFiles.count() > 0) {
        auto *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Home, Qt::NoModifier);
        QApplication::postEvent(ui->fileTreeWidget, event);
    } else {
        // Clear the file details when the list is empty
        loadCurrentFileDetails();
    }
}

/**
 * Shows the currently selected attachment
 *
 * @param current
 * @param previous
 */
void StoredAttachmentsDialog::on_fileTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                                   QTreeWidgetItem *previous) {
    Q_UNUSED(current);
    Q_UNUSED(previous);
    loadCurrentFileDetails();
}

void StoredAttachmentsDialog::loadCurrentFileDetails() {
    QTreeWidgetItem *current = ui->fileTreeWidget->currentItem();

    if (current == nullptr) {
        ui->infoFrame->setEnabled(false);
        ui->pathLabel->clear();
        ui->typeLabel->clear();
        ui->sizeLabel->clear();
        ui->notesFrame->hide();

        return;
    }

    QString filePath = getFilePath(current);
    ui->pathLabel->setText(filePath);

    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(filePath);
    ui->typeLabel->setText(type.comment());

    QFileInfo fileInfo(filePath);
    QString fileSizeText = Utils::Misc::toHumanReadableByteSize(fileInfo.size());
    ui->sizeLabel->setText(fileSizeText);

    ui->infoFrame->setEnabled(true);

    // Show which notes use the current file
    const QString fileName = current->text(0);
    if (_fileNoteList.contains(fileName)) {
        auto notes = _fileNoteList[fileName];
        ui->noteTreeWidget->clear();

        Q_FOREACH (Note note, notes) {
            auto *item = new QTreeWidgetItem();
            item->setText(0, note.getName());
            item->setData(0, Qt::UserRole, note.getId());
            Utils::Gui::setTreeWidgetItemToolTipForNote(item, note);

            NoteSubFolder noteSubFolder = note.getNoteSubFolder();
            if (noteSubFolder.isFetched()) {
                item->setToolTip(0, tr("Path: %1").arg(noteSubFolder.relativePath()));
            }

            ui->noteTreeWidget->addTopLevelItem(item);
        }

        ui->notesFrame->show();
    } else {
        ui->notesFrame->hide();
    }
}

/**
 * Gets the file path of a tree widget item
 *
 * @param item
 * @return
 */
QString StoredAttachmentsDialog::getFilePath(QTreeWidgetItem *item) {
    if (item == nullptr) {
        return QString();
    }

    QString fileName = NoteFolder::currentAttachmentsPath() + QDir::separator() +
                       item->data(0, Qt::UserRole).toString();
    return fileName;
}

/**
 * Deletes selected attachments
 */
void StoredAttachmentsDialog::on_deleteButton_clicked() {
    int selectedItemsCount = ui->fileTreeWidget->selectedItems().count();

    if (selectedItemsCount == 0) {
        return;
    }

    if (Utils::Gui::question(
            this, tr("Delete selected files"),
            tr("Delete <strong>%n</strong> selected file(s)?", "", selectedItemsCount),
            QStringLiteral("delete-attachment-files")) != QMessageBox::Yes) {
        return;
    }

    // delete all selected files
    Q_FOREACH (QTreeWidgetItem *item, ui->fileTreeWidget->selectedItems()) {
        QString filePath = getFilePath(item);
        bool removed = QFile::remove(filePath);

        if (removed) {
            delete item;
        }
    }
}

/**
 * Event filters
 *
 * @param obj
 * @param event
 * @return
 */
bool StoredAttachmentsDialog::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (obj == ui->fileTreeWidget) {
            // delete the currently selected attachments
            if ((keyEvent->key() == Qt::Key_Delete) || (keyEvent->key() == Qt::Key_Backspace)) {
                on_deleteButton_clicked();
                return true;
            }
            return false;
        }
    }

    return MasterDialog::eventFilter(obj, event);
}

void StoredAttachmentsDialog::on_insertButton_clicked() {
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == nullptr) {
        return;
    }

    int selectedItemsCount = ui->fileTreeWidget->selectedItems().count();

    if (selectedItemsCount == 0) {
        return;
    }

    QOwnNotesMarkdownTextEdit *textEdit = mainWindow->activeNoteTextEdit();
    Note note = mainWindow->getCurrentNote();

    // insert all selected attachments
    Q_FOREACH (QTreeWidgetItem *item, ui->fileTreeWidget->selectedItems()) {
        QString filePath = getFilePath(item);
        QFileInfo fileInfo(filePath);
        QString attachmentsUrlString = note.attachmentUrlStringForFileName(fileInfo.fileName());
        QString attachmentLink = "[" + fileInfo.baseName() + "](" + attachmentsUrlString + ")\n";
        textEdit->insertPlainText(attachmentLink);
    }

    refreshAttachmentFiles();
}

void StoredAttachmentsDialog::on_openFileButton_clicked() {
    QTreeWidgetItem *item = ui->fileTreeWidget->currentItem();

    if (item == nullptr) {
        return;
    }

    QString filePath = getFilePath(item);
    Utils::Misc::openPath(filePath);
}

void StoredAttachmentsDialog::on_openFolderButton_clicked() {
    QTreeWidgetItem *item = ui->fileTreeWidget->currentItem();

    if (item == nullptr) {
        return;
    }

    QString filePath = getFilePath(item);
    Utils::Misc::openFolderSelect(filePath);
}

void StoredAttachmentsDialog::on_fileTreeWidget_customContextMenuRequested(const QPoint &pos) {
    // don't open the most of the context menu if no tags are selected
    const bool hasSelected = ui->fileTreeWidget->selectedItems().count() > 0;

    const QPoint globalPos = ui->fileTreeWidget->mapToGlobal(pos);
    QMenu menu;

    // allow these actions only if items are selected
    QAction *renameAction = nullptr;
    QAction *removeAction = nullptr;
    QAction *openAction = nullptr;
    QAction *addAction = nullptr;
    if (hasSelected) {
        openAction = menu.addAction(tr("&Open attachment"));
        renameAction = menu.addAction(tr("&Rename attachment"));
        removeAction = menu.addAction(tr("&Delete attachments"));
        addAction = menu.addAction(tr("&Add attachments to current note"));
    }

    QAction *selectedItem = menu.exec(globalPos);

    if (selectedItem == nullptr) {
        return;
    }

    QTreeWidgetItem *item = ui->fileTreeWidget->currentItem();

    if (selectedItem == removeAction) {
        on_deleteButton_clicked();
    } else if (selectedItem == renameAction) {
        ui->fileTreeWidget->editItem(item);
    } else if (selectedItem == addAction) {
        on_insertButton_clicked();
    } else if (selectedItem == openAction) {
        on_openFileButton_clicked();
    }
}

void StoredAttachmentsDialog::refreshAndJumpToFileName(const QString &fileName) {
    // refresh the attachment files to get the new data
    refreshAttachmentFiles();

    // look for the item to jump back to
    auto item = Utils::Gui::getTreeWidgetItemWithUserData(ui->fileTreeWidget, fileName);

    // set the previous item with a timer (didn't work without timer)
    QTimer::singleShot(0, this, [this, item]() { ui->fileTreeWidget->setCurrentItem(item); });
}

void StoredAttachmentsDialog::on_searchLineEdit_textChanged(const QString &arg1) {
    Utils::Gui::searchForTextInTreeWidget(
        ui->fileTreeWidget, arg1,
        Utils::Gui::TreeWidgetSearchFlags(Utils::Gui::TreeWidgetSearchFlag::EveryWordSearch));
}

void StoredAttachmentsDialog::on_fileTreeWidget_itemDoubleClicked(QTreeWidgetItem *item,
                                                                  int column) {
    Q_UNUSED(item)
    Q_UNUSED(column)
    on_insertButton_clicked();
}

void StoredAttachmentsDialog::openCurrentNote() {
    QTreeWidgetItem *item = ui->noteTreeWidget->currentItem();

    if (item == nullptr) {
        return;
    }

    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow == nullptr) {
        Q_UNUSED(item)
        return;
    }

    mainWindow->setCurrentNoteFromNoteId(item->data(0, Qt::UserRole).toInt());
    mainWindow->openCurrentNoteInTab();
}

void StoredAttachmentsDialog::on_noteTreeWidget_itemDoubleClicked(QTreeWidgetItem *item,
                                                                  int column) {
    Q_UNUSED(item)
    Q_UNUSED(column)
    openCurrentNote();
}

void StoredAttachmentsDialog::on_refreshButton_clicked() { refreshAttachmentFiles(); }

void StoredAttachmentsDialog::on_fileTreeWidget_itemChanged(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column)

    QString oldFileName = item->data(0, Qt::UserRole).toString();
    QString newFileName = item->text(0);
    const QSignalBlocker blocker(ui->fileTreeWidget);
    Q_UNUSED(blocker)
    qDebug() << __func__ << " - 'oldFileName': " << oldFileName;
    qDebug() << __func__ << " - 'newFileName': " << newFileName;

    const QString oldFilePath =
        NoteFolder::currentAttachmentsPath() + QDir::separator() + oldFileName;
    QFile oldFile(oldFilePath);
    if (!oldFile.exists()) {
        QMessageBox::warning(this, tr("File doesn't exist"),
                             tr("The file <strong>%1</strong> doesn't exist, "
                                "you cannot rename it!")
                                 .arg(oldFilePath));
        item->setText(0, oldFileName);

        return;
    }

    const QString newFilePath =
        NoteFolder::currentAttachmentsPath() + QDir::separator() + newFileName;
    QFile newFile(newFilePath);

    if (newFile.exists()) {
        QMessageBox::warning(this, tr("File exists"),
                             tr("File <strong>%1</strong> already exists, "
                                "you need to remove it before choosing "
                                "<strong>%2</strong> as new filename!")
                                 .arg(newFilePath, newFileName));
        item->setText(0, oldFileName);

        return;
    }

    if (!oldFile.rename(newFilePath)) {
        QMessageBox::warning(this, tr("File renaming failed"),
                             tr("Renaming of file <strong>%1</strong> failed!").arg(oldFilePath));
        item->setText(0, oldFileName);

        return;
    }

    const auto affectedNotes = _fileNoteList[oldFileName];
    const int affectedNotesCount = affectedNotes.count();

    if (affectedNotesCount == 0) {
        refreshAndJumpToFileName(newFileName);
        return;
    }

    if (Utils::Gui::questionNoSkipOverride(
            nullptr, QObject::tr("File name changed"),
            QObject::tr("%n note(s) are using this attachment. Would you also "
                        "like to rename those attachments in the note(s)?",
                        "", affectedNotesCount),
            QStringLiteral("note-replace-attachments")) != QMessageBox::Yes) {
        refreshAndJumpToFileName(newFileName);
        return;
    }

    // search for attachment with oldFileName in notes and rename attachment links
    for (Note note : affectedNotes) {
        // re-fetch note to get the note text
        note.refetch();

        QString text = note.getNoteText();
        text.replace(QRegularExpression(R"((\[.*?\])\((.*?attachments\/.*?)()" +
                                        QRegularExpression::escape(oldFileName) + ")\\)"),
                     "\\1(\\2" + newFileName + ")");

        note.storeNewText(text);
    }

    refreshAndJumpToFileName(newFileName);
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != nullptr) {
        // update the current note in case attachment paths were updated in it
        mainWindow->reloadCurrentNoteByNoteId(true);
    }
}

void StoredAttachmentsDialog::on_noteTreeWidget_customContextMenuRequested(const QPoint &pos) {
    // don't open the most of the context menu if no tags are selected
    const bool hasSelected = ui->noteTreeWidget->selectedItems().count() > 0;

    const QPoint globalPos = ui->noteTreeWidget->mapToGlobal(pos);
    QMenu menu;

    // allow these actions only if items are selected
    QAction *openAction = nullptr;
    if (hasSelected) {
        openAction = menu.addAction(tr("&Open note"));
    }

    QAction *selectedItem = menu.exec(globalPos);

    if (selectedItem == nullptr) {
        return;
    }

    if (selectedItem == openAction) {
        openCurrentNote();
    }
}

void StoredAttachmentsDialog::on_orphanedCheckBox_toggled(bool checked) {
    if (checked) {
        const QSignalBlocker blocker(ui->fileTreeWidget);
        Q_UNUSED(blocker)
        ui->currentNoteCheckBox->setChecked(false);
    }

    _orphanedAttachmentsOnly = checked;
    refreshAttachmentFiles();
}

void StoredAttachmentsDialog::on_currentNoteCheckBox_toggled(bool checked) {
    if (checked) {
        const QSignalBlocker blocker(ui->fileTreeWidget);
        Q_UNUSED(blocker)
        ui->orphanedCheckBox->setChecked(false);
    }

    _currentNoteOnly = checked;
    refreshAttachmentFiles();
}
