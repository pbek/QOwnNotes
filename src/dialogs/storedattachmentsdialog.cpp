#include "storedattachmentsdialog.h"

#include <entities/note.h>
#include <entities/notefolder.h>
#include <mainwindow.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QTreeWidgetItem>
#include <QtCore/QMimeDatabase>
#include <QtCore/QMimeType>
#include <QtWidgets/QMessageBox>

#include "ui_storedattachmentsdialog.h"
#include "widgets/qownnotesmarkdowntextedit.h"

StoredAttachmentsDialog::StoredAttachmentsDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::StoredAttachmentsDialog) {
    ui->setupUi(this);
    ui->infoFrame->setEnabled(false);
    ui->fileTreeWidget->installEventFilter(this);

    QDir attachmentsDir(NoteFolder::currentAttachmentsPath());

    if (!attachmentsDir.exists()) {
        ui->progressBar->setValue(ui->progressBar->maximum());
        return;
    }

    QStringList orphanedFiles = attachmentsDir.entryList(
        QStringList(QStringLiteral("*")), QDir::Files, QDir::Time);
    orphanedFiles.removeDuplicates();

    QVector<Note> noteList = Note::fetchAll();
    int noteListCount = noteList.count();

    ui->progressBar->setMaximum(noteListCount);
    ui->progressBar->show();

    Q_FOREACH (const Note &note, noteList) {
        QStringList attachmentsFileList = note.getAttachmentsFileList();

        // remove all found attachments from the orphaned files list
        Q_FOREACH (const QString &fileName, attachmentsFileList) {
            orphanedFiles.removeAll(fileName);
        }

        ui->progressBar->setValue(ui->progressBar->value() + 1);
    }

    ui->progressBar->hide();

    Q_FOREACH (QString fileName, orphanedFiles) {
        auto *item = new QTreeWidgetItem();
        item->setText(0, fileName);
        item->setData(0, Qt::UserRole, fileName);

        QString filePath = getFilePath(item);
        QFileInfo info(filePath);
        item->setToolTip(
            0, tr("Last modified at %1").arg(info.lastModified().toString()));

        ui->fileTreeWidget->addTopLevelItem(item);
    }

    // jump to the first item
    if (orphanedFiles.count() > 0) {
        auto *event =
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Home, Qt::NoModifier);
        QApplication::postEvent(ui->fileTreeWidget, event);
    }
}

StoredAttachmentsDialog::~StoredAttachmentsDialog() { delete ui; }

/**
 * Shows the currently selected attachment
 *
 * @param current
 * @param previous
 */
void StoredAttachmentsDialog::on_fileTreeWidget_currentItemChanged(
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
    QString fileSizeText =
        Utils::Misc::toHumanReadableByteSize(fileInfo.size());
    ui->sizeLabel->setText(fileSizeText);

    ui->infoFrame->setEnabled(true);
}

/**
 * Gets the file path of a tree widget item
 *
 * @param item
 * @return
 */
QString StoredAttachmentsDialog::getFilePath(QTreeWidgetItem *item) {
    if (item == Q_NULLPTR) {
        return QString();
    }

    QString fileName = NoteFolder::currentAttachmentsPath() +
                       QDir::separator() +
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

    if (Utils::Gui::question(this, tr("Delete selected files"),
                             tr("Delete <strong>%n</strong> selected file(s)?",
                                "", selectedItemsCount),
                             QStringLiteral("delete-attachment-files")) !=
        QMessageBox::Yes) {
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

void StoredAttachmentsDialog::on_insertButton_clicked() {
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
    Note note = mainWindow->getCurrentNote();

    // insert all selected attachments
    Q_FOREACH (QTreeWidgetItem *item, ui->fileTreeWidget->selectedItems()) {
        QString filePath = getFilePath(item);
        QFileInfo fileInfo(filePath);
        QString attachmentsUrlString =
            note.attachmentUrlStringForFileName(fileInfo.fileName());
        QString attachmentLink =
            "[" + fileInfo.baseName() + "](" + attachmentsUrlString + ")\n";
        textEdit->insertPlainText(attachmentLink);
        delete item;
    }
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
