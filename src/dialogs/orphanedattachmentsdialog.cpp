#include <QDebug>
#include <QKeyEvent>
#include <entities/notefolder.h>
#include <entities/note.h>
#include <QTreeWidgetItem>
#include <QGraphicsPixmapItem>
#include <QtWidgets/QMessageBox>
#include <utils/gui.h>
#include <mainwindow.h>
#include <QtCore/QMimeDatabase>
#include <QtCore/QMimeType>
#include <utils/misc.h>
#include "orphanedattachmentsdialog.h"
#include "ui_orphanedattachmentsdialog.h"

OrphanedAttachmentsDialog::OrphanedAttachmentsDialog(QWidget *parent) :
        MasterDialog(parent),
    ui(new Ui::OrphanedAttachmentsDialog) {
    ui->setupUi(this);
    ui->infoFrame->setEnabled(false);
    ui->fileTreeWidget->installEventFilter(this);

    QDir attachmentsDir(NoteFolder::currentAttachmentsPath());

    if (!attachmentsDir.exists()) {
        ui->progressBar->setValue(ui->progressBar->maximum());
        return;
    }

    QStringList orphanedFiles = attachmentsDir.entryList(
            QStringList("*"), QDir::Files, QDir::Time);
    orphanedFiles.removeDuplicates();

    QList<Note> noteList = Note::fetchAll();
    int noteListCount = noteList.count();

    ui->progressBar->setMaximum(noteListCount);
    ui->progressBar->show();

    Q_FOREACH(Note note, noteList) {
            QStringList attachmentsFileList = note.getAttachmentsFileList();

            // remove all found attachments from the orphaned files list
            Q_FOREACH(QString fileName, attachmentsFileList) {
                orphanedFiles.removeAll(fileName);
            }

            ui->progressBar->setValue(ui->progressBar->value() + 1);
        }

    ui->progressBar->hide();

    Q_FOREACH(QString fileName, orphanedFiles) {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, fileName);
            item->setData(0, Qt::UserRole, fileName);

            QString filePath = getFilePath(item);
            QFileInfo info(filePath);
            item->setToolTip(0, tr("Last modified at %1").arg(
                    info.lastModified().toString()));

            ui->fileTreeWidget->addTopLevelItem(item);
        }

    // jump to the first item
    if (orphanedFiles.count() > 0) {
        QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Home,
                                         Qt::NoModifier);
        QApplication::postEvent(ui->fileTreeWidget, event);
    }
}

OrphanedAttachmentsDialog::~OrphanedAttachmentsDialog() {
    delete ui;
}

/**
 * Shows the currently selected attachment
 * 
 * @param current
 * @param previous
 */
void OrphanedAttachmentsDialog::on_fileTreeWidget_currentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous);

    if (current == nullptr) {
        ui->infoFrame->setEnabled(false);
        ui->pathLabel->clear();
        ui->typeLabel->clear();
        ui->sizeLabel->clear();

        return;
    }

    QString filePath = getFilePath(current);
    ui->pathLabel->setText(filePath);

    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(filePath);
    ui->typeLabel->setText(type.comment());

    QFileInfo fileInfo(filePath);
    QString fileSizeText = Utils::Misc::toHumanReadableByteSize(
            fileInfo.size());
    ui->sizeLabel->setText(fileSizeText);

    ui->infoFrame->setEnabled(true);
}

/**
 * Gets the file path of a tree widget item
 *
 * @param item
 * @return
 */
QString OrphanedAttachmentsDialog::getFilePath(QTreeWidgetItem *item) {
    if (item == Q_NULLPTR) {
        return "";
    }

    QString fileName = NoteFolder::currentAttachmentsPath() + QDir::separator() +
                       item->data(0, Qt::UserRole).toString();
    return fileName;
}

/**
 * Deletes selected attachments
 */
void OrphanedAttachmentsDialog::on_deleteButton_clicked() {
    int selectedItemsCount = ui->fileTreeWidget->selectedItems().count();

    if (selectedItemsCount == 0) {
        return;
    }

    if (Utils::Gui::question(
            this,
            tr("Delete selected files"),
            tr("Delete <strong>%n</strong> selected files(s)?",
               "", selectedItemsCount),
            "delete-files") != QMessageBox::Yes) {
        return;
    }

    // delete all selected files
    Q_FOREACH(QTreeWidgetItem *item, ui->fileTreeWidget->selectedItems()) {
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
bool OrphanedAttachmentsDialog::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (obj == ui->fileTreeWidget) {
            // delete the currently selected attachments
            if ((keyEvent->key() == Qt::Key_Delete) ||
                       (keyEvent->key() == Qt::Key_Backspace)) {
                on_deleteButton_clicked();
                return true;
            }
            return false;
        }
    }

    return MasterDialog::eventFilter(obj, event);
}

void OrphanedAttachmentsDialog::on_insertButton_clicked() {
#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == Q_NULLPTR) {
        return;
    }
#else
    return;
#endif

    int selectedItemsCount = ui->fileTreeWidget->selectedItems().count();

    if (selectedItemsCount == 0) {
        return;
    }

    QOwnNotesMarkdownTextEdit *textEdit = mainWindow->activeNoteTextEdit();

    // insert all selected attachments
    Q_FOREACH(QTreeWidgetItem *item, ui->fileTreeWidget->selectedItems()) {
            QString filePath = getFilePath(item);
            QFileInfo fileInfo(filePath);
            QString attachmentsUrlString = "file://attachments/" +
                    fileInfo.fileName();
            QString attachmentLink = "![" + fileInfo.baseName() + "](" +
                             attachmentsUrlString + ")\n";
            textEdit->insertPlainText(attachmentLink);
            delete item;
    }
}

void OrphanedAttachmentsDialog::on_openFileButton_clicked() {
    QTreeWidgetItem *item = ui->fileTreeWidget->currentItem();

    if (item == nullptr) {
        return;
    }

    QString filePath = getFilePath(item);
    Utils::Misc::openPath(filePath);
}

void OrphanedAttachmentsDialog::on_openFolderButton_clicked() {
    QTreeWidgetItem *item = ui->fileTreeWidget->currentItem();

    if (item == nullptr) {
        return;
    }

    QString filePath = getFilePath(item);
    Utils::Misc::openFolderSelect(filePath);
}
