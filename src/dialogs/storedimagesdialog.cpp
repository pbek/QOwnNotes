#include "storedimagesdialog.h"

#include <entities/note.h>
#include <entities/notefolder.h>
#include <mainwindow.h>
#include <utils/gui.h>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QTreeWidgetItem>
#include <QtWidgets/QMessageBox>

#include "ui_storedimagesdialog.h"
#include "widgets/qownnotesmarkdowntextedit.h"

StoredImagesDialog::StoredImagesDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::StoredImagesDialog) {
    ui->setupUi(this);
    ui->fileTreeWidget->installEventFilter(this);

    QDir mediaDir(NoteFolder::currentMediaPath());

    if (!mediaDir.exists()) {
        ui->progressBar->setValue(ui->progressBar->maximum());
        return;
    }

    refreshMediaFiles();
}

StoredImagesDialog::~StoredImagesDialog() { delete ui; }

void StoredImagesDialog::refreshMediaFiles() {
    QDir mediaDir(NoteFolder::currentMediaPath());

    if (!mediaDir.exists()) {
        ui->progressBar->setValue(ui->progressBar->maximum());
        return;
    }

    QStringList mediaFiles = mediaDir.entryList(
        QStringList(QStringLiteral("*")), QDir::Files, QDir::Time);
    mediaFiles.removeDuplicates();

    QVector<Note> noteList = Note::fetchAll();
    int noteListCount = noteList.count();

    if (_orphanedImagesOnly) {
        ui->progressBar->setMaximum(noteListCount);
        ui->progressBar->show();

        Q_FOREACH (Note note, noteList) {
                QStringList mediaFileList = note.getMediaFileList();

                // remove all found images from the orphaned files list
                Q_FOREACH (QString fileName, mediaFileList) {
                        mediaFiles.removeAll(fileName);
                    }

                ui->progressBar->setValue(ui->progressBar->value() + 1);
            }
    }

    ui->progressBar->hide();
    ui->fileTreeWidget->clear();

    Q_FOREACH (QString fileName, mediaFiles) {
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
    if (mediaFiles.count() > 0) {
        auto *event =
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Home, Qt::NoModifier);
        QApplication::postEvent(ui->fileTreeWidget, event);
    }
}

/**
 * Shows the currently selected image
 *
 * @param current
 * @param previous
 */
void StoredImagesDialog::on_fileTreeWidget_currentItemChanged(
    QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous);

    auto *scene = new QGraphicsScene(this);
    QString filePath = getFilePath(current);

    if (!filePath.isEmpty()) {
        scene->addPixmap(QPixmap(filePath));
    }

    ui->graphicsView->setScene(scene);
}

/**
 * Gets the file path of a tree widget item
 *
 * @param item
 * @return
 */
QString StoredImagesDialog::getFilePath(QTreeWidgetItem *item) {
    if (item == Q_NULLPTR) {
        return QString();
    }

    QString fileName = NoteFolder::currentMediaPath() + QDir::separator() +
                       item->data(0, Qt::UserRole).toString();
    return fileName;
}

/**
 * Deletes selected images
 */
void StoredImagesDialog::on_deleteButton_clicked() {
    int selectedItemsCount = ui->fileTreeWidget->selectedItems().count();

    if (selectedItemsCount == 0) {
        return;
    }

    if (Utils::Gui::question(this, tr("Delete selected files"),
                             tr("Delete <strong>%n</strong> selected file(s)?",
                                "", selectedItemsCount),
                             QStringLiteral("delete-files")) !=
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
bool StoredImagesDialog::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (obj == ui->fileTreeWidget) {
            // delete the currently selected images
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

void StoredImagesDialog::on_insertButton_clicked() {
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

    // insert all selected images
    Q_FOREACH (QTreeWidgetItem *item, ui->fileTreeWidget->selectedItems()) {
        QString filePath = getFilePath(item);
        QFileInfo fileInfo(filePath);
        QString mediaUrlString =
            note.mediaUrlStringForFileName(fileInfo.fileName());
        QString imageLink =
            "![" + fileInfo.baseName() + "](" + mediaUrlString + ")\n";
        textEdit->insertPlainText(imageLink);
        delete item;
    }
}

void StoredImagesDialog::on_checkBox_toggled(bool checked) {
    _orphanedImagesOnly = checked;
    refreshMediaFiles();
}
