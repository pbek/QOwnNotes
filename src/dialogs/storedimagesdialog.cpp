#include "storedimagesdialog.h"

#include <entities/note.h>
#include <entities/notefolder.h>
#include <entities/notesubfolder.h>
#include <mainwindow.h>
#include <utils/gui.h>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QLocale>
#include <QMenu>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QtWidgets/QMessageBox>

#include "ui_storedimagesdialog.h"
#include "widgets/qownnotesmarkdowntextedit.h"

StoredImagesDialog::StoredImagesDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::StoredImagesDialog) {
    ui->setupUi(this);
    afterSetupUI();
    ui->fileTreeWidget->installEventFilter(this);
    ui->noteTreeWidget->installEventFilter(this);

    Utils::Gui::setSectionHeaderFont(ui->previewHeaderLabel);
    Utils::Gui::setMutedTextColor(ui->previewDetailLabel);
    Utils::Gui::setMutedTextColor(ui->fileCountLabel);
    ui->graphicsView->setAlignment(Qt::AlignCenter);
    ui->graphicsView->setRenderHint(QPainter::SmoothPixmapTransform);

    connect(ui->fileTreeWidget, &QTreeWidget::itemSelectionChanged, this,
            &StoredImagesDialog::updateActionButtons);

    refreshMediaFiles();
}

StoredImagesDialog::~StoredImagesDialog() { delete ui; }

void StoredImagesDialog::refreshMediaFiles() {
    QDir mediaDir(NoteFolder::currentMediaPath());

    if (!mediaDir.exists()) {
        ui->progressBar->hide();
        ui->fileTreeWidget->clear();
        loadCurrentFileDetails();
        updateFileCountLabel();
        return;
    }

    QStringList mediaFiles;
    QVector<Note> noteList;

    if (_currentNoteOnly) {
        MainWindow *mainWindow = MainWindow::instance();
        if (mainWindow == nullptr) {
            return;
        }

        const auto note = mainWindow->getCurrentNote();
        if (note.isFetched()) {
            mediaFiles = note.getMediaFileList();
            noteList = {note};
        }
    } else {
        mediaFiles = mediaDir.entryList(QStringList(QStringLiteral("*")), QDir::Files, QDir::Time);
        noteList = Note::fetchAll();
    }

    mediaFiles.removeDuplicates();
    int noteListCount = noteList.count();
    _fileNoteList.clear();

    ui->progressBar->setMaximum(noteListCount);
    ui->progressBar->show();

    Q_FOREACH (Note note, noteList) {
        QStringList mediaFileList = note.getMediaFileList();
        // we don't want to keep the whole note text
        note.setNoteText("");

        Q_FOREACH (QString fileName, mediaFileList) {
            // remove all found images from the files list if _orphanedImagesOnly is enabled
            if (_orphanedImagesOnly) {
                mediaFiles.removeAll(fileName);
            }

            if (!_fileNoteList[fileName].contains(note)) {
                _fileNoteList[fileName].append(note);
            }
        }

        ui->progressBar->setValue(ui->progressBar->value() + 1);
    }

    ui->progressBar->hide();
    ui->fileTreeWidget->clear();
    const QColor mutedColor =
        ui->fileTreeWidget->palette().color(QPalette::Disabled, QPalette::Text);

    Q_FOREACH (QString fileName, mediaFiles) {
        auto *item = new QTreeWidgetItem();
        item->setText(0, fileName);
        item->setData(0, Qt::UserRole, fileName);
        item->setFlags(item->flags() | Qt::ItemIsEditable);

        QString filePath = getFilePath(item);
        QFileInfo info(filePath);
        item->setToolTip(
            0, tr("Last modified at %1")
                       .arg(QLocale().toString(info.lastModified(), QLocale::ShortFormat)) +
                   QStringLiteral("\n") + Utils::Misc::toHumanReadableByteSize(info.size()));

        // Show images that are not used in any note muted
        if (!_fileNoteList.contains(fileName)) {
            item->setForeground(0, QBrush(mutedColor));
        }

        ui->fileTreeWidget->addTopLevelItem(item);
    }

    // Re-apply search text
    on_searchLineEdit_textChanged(ui->searchLineEdit->text());

    ui->fileTreeWidget->sortItems(0, Qt::AscendingOrder);

    // jump to the first item
    if (mediaFiles.count() > 0) {
        auto *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Home, Qt::NoModifier);
        QApplication::postEvent(ui->fileTreeWidget, event);
    } else {
        // Clear the image preview when the list is empty
        loadCurrentFileDetails();
    }

    updateActionButtons();
}

/**
 * Shows the currently selected image
 *
 * @param current
 * @param previous
 */
void StoredImagesDialog::on_fileTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                              QTreeWidgetItem *previous) {
    Q_UNUSED(current);
    Q_UNUSED(previous);
    loadCurrentFileDetails();
}

void StoredImagesDialog::loadCurrentFileDetails() {
    QTreeWidgetItem *current = ui->fileTreeWidget->currentItem();

    // Replace the previous scene, so the old images don't pile up in memory
    QGraphicsScene *oldScene = ui->graphicsView->scene();

    if (current == nullptr || current->isHidden()) {
        // Clear the graphics view when no item is selected or list is empty
        ui->graphicsView->setScene(new QGraphicsScene(this));
        delete oldScene;
        ui->notesFrame->hide();

        const int total = ui->fileTreeWidget->topLevelItemCount();
        ui->previewHeaderLabel->setText(total == 0 ? (_orphanedImagesOnly ? tr("No orphaned images")
                                                      : _currentNoteOnly
                                                          ? tr("No images in the current note")
                                                          : tr("No images"))
                                                   : tr("No image selected"));
        ui->previewDetailLabel->clear();
        ui->previewDetailLabel->hide();

        return;
    }

    auto *scene = new QGraphicsScene(this);
    QString filePath = getFilePath(current);
    QPixmap pixmap;

    if (!filePath.isEmpty()) {
        pixmap.load(filePath);
        scene->addPixmap(pixmap);
    }

    ui->graphicsView->setScene(scene);
    delete oldScene;
    fitImageInView();

    // Show the file name and details of the image
    const QString fileName = current->text(0);
    const QFileInfo fileInfo(filePath);
    QStringList details;

    if (!pixmap.isNull()) {
        details << tr("%1 × %2 pixels").arg(pixmap.width()).arg(pixmap.height());
    } else {
        details << tr("Image can't be shown");
    }

    details << Utils::Misc::toHumanReadableByteSize(fileInfo.size());
    details << tr("Last modified at %1")
                   .arg(QLocale().toString(fileInfo.lastModified(), QLocale::ShortFormat));

    if (!_fileNoteList.contains(fileName)) {
        details << tr("Not used in any note");
    }

    ui->previewHeaderLabel->setText(fileName);
    ui->previewDetailLabel->setText(details.join(QStringLiteral(" · ")));
    ui->previewDetailLabel->show();

    // Show which notes use the current file
    if (_fileNoteList.contains(fileName)) {
        auto notes = _fileNoteList[fileName];
        ui->noteTreeWidget->clear();
        ui->usedInNotesLabel->setText(tr("Used in %n note(s):", "", notes.count()));

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
QString StoredImagesDialog::getFilePath(QTreeWidgetItem *item) {
    if (item == nullptr) {
        return QString();
    }

    QString fileName =
        NoteFolder::currentMediaPath() + QDir::separator() + item->data(0, Qt::UserRole).toString();
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

    if (Utils::Gui::question(
            this, tr("Delete selected files"),
            tr("Delete <strong>%n</strong> selected file(s)?", "", selectedItemsCount),
            QStringLiteral("delete-image-files")) != QMessageBox::Yes) {
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

    loadCurrentFileDetails();
    updateFileCountLabel();
    updateActionButtons();
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
            if ((keyEvent->key() == Qt::Key_Delete) || (keyEvent->key() == Qt::Key_Backspace)) {
                on_deleteButton_clicked();
                return true;
            }
            return false;
        }
    }

    return MasterDialog::eventFilter(obj, event);
}

void StoredImagesDialog::on_insertButton_clicked() {
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

    // insert all selected images
    Q_FOREACH (QTreeWidgetItem *item, ui->fileTreeWidget->selectedItems()) {
        QString filePath = getFilePath(item);
        QFileInfo fileInfo(filePath);
        QString mediaUrlString = note.mediaUrlStringForFileName(fileInfo.fileName());
        QString imageLink = "![" + fileInfo.baseName() + "](" + mediaUrlString + ")\n";
        textEdit->insertPlainText(imageLink);
    }

    refreshMediaFiles();
}

void StoredImagesDialog::on_searchLineEdit_textChanged(const QString &arg1) {
    Utils::Gui::searchForTextInTreeWidget(
        ui->fileTreeWidget, arg1,
        Utils::Gui::TreeWidgetSearchFlags(Utils::Gui::TreeWidgetSearchFlag::EveryWordSearch));
    updateFileCountLabel();
}

/**
 * Shows how many images are in the list, and how many of them match the search
 */
void StoredImagesDialog::updateFileCountLabel() {
    const int total = ui->fileTreeWidget->topLevelItemCount();

    if (ui->searchLineEdit->text().isEmpty()) {
        ui->fileCountLabel->setText(tr("%n image(s)", "", total));
        return;
    }

    int visible = 0;
    for (int i = 0; i < total; ++i) {
        if (!ui->fileTreeWidget->topLevelItem(i)->isHidden()) {
            ++visible;
        }
    }

    ui->fileCountLabel->setText(tr("%1 of %n image(s)", "", total).arg(visible));
}

/**
 * Only enables the actions that can be used with the current selection
 */
void StoredImagesDialog::updateActionButtons() {
    const bool hasSelection = !ui->fileTreeWidget->selectedItems().isEmpty();
    const bool hasCurrentItem = ui->fileTreeWidget->currentItem() != nullptr;

    ui->deleteButton->setEnabled(hasSelection);
    ui->insertButton->setEnabled(hasSelection);
    ui->openFileButton->setEnabled(hasCurrentItem);
    ui->openFolderButton->setEnabled(hasCurrentItem);
}

/**
 * Scales images down that are bigger than the view, small images are shown in their real size
 */
void StoredImagesDialog::fitImageInView() {
    QGraphicsScene *scene = ui->graphicsView->scene();
    ui->graphicsView->resetTransform();

    if (scene == nullptr || scene->items().isEmpty()) {
        return;
    }

    const QRectF rect = scene->itemsBoundingRect();
    const QSize viewportSize = ui->graphicsView->viewport()->size();

    if (rect.width() > viewportSize.width() || rect.height() > viewportSize.height()) {
        ui->graphicsView->fitInView(rect, Qt::KeepAspectRatio);
    }
}

void StoredImagesDialog::resizeEvent(QResizeEvent *event) {
    MasterDialog::resizeEvent(event);
    fitImageInView();
}

/**
 * Insert the image into the current note on double click on file list
 *
 * @param item
 * @param column
 */
void StoredImagesDialog::on_fileTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    Q_UNUSED(item)
    Q_UNUSED(column)
    on_insertButton_clicked();
}

void StoredImagesDialog::openCurrentNote() {
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

/**
 * Open the selected note on double click on note list
 *
 * @param item
 * @param column
 */
void StoredImagesDialog::on_noteTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    Q_UNUSED(item)
    Q_UNUSED(column)
    openCurrentNote();
}

void StoredImagesDialog::on_refreshButton_clicked() { refreshMediaFiles(); }

void StoredImagesDialog::on_fileTreeWidget_itemChanged(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column)

    QString oldFileName = item->data(0, Qt::UserRole).toString();
    QString newFileName = item->text(0);
    const QSignalBlocker blocker(ui->fileTreeWidget);
    Q_UNUSED(blocker)
    qDebug() << __func__ << " - 'oldFileName': " << oldFileName;
    qDebug() << __func__ << " - 'newFileName': " << newFileName;

    const QString oldFilePath = NoteFolder::currentMediaPath() + QDir::separator() + oldFileName;
    QFile oldFile(oldFilePath);
    if (!oldFile.exists()) {
        QMessageBox::warning(this, tr("File doesn't exist"),
                             tr("The file <strong>%1</strong> doesn't exist, "
                                "you cannot rename it!")
                                 .arg(oldFilePath));
        item->setText(0, oldFileName);

        return;
    }

    const QString newFilePath = NoteFolder::currentMediaPath() + QDir::separator() + newFileName;
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
            QObject::tr("%n note(s) are using this image. Would you also "
                        "like to rename those images in the note(s)?",
                        "", affectedNotesCount),
            QStringLiteral("note-replace-images")) != QMessageBox::Yes) {
        refreshAndJumpToFileName(newFileName);
        return;
    }

    // search for image with oldFileName in notes and rename image links
    for (Note note : affectedNotes) {
        // re-fetch note to get the note text
        note.refetch();

        QString text = note.getNoteText();
        text.replace(QRegularExpression(R"((!\[.*?\])\((.*?media\/.*?)()" +
                                        QRegularExpression::escape(oldFileName) + ")\\)"),
                     "\\1(\\2" + newFileName + ")");

        note.storeNewText(text);
    }

    refreshAndJumpToFileName(newFileName);
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != nullptr) {
        // update the current note in case image paths were updated in it
        mainWindow->reloadCurrentNoteByNoteId(true);
    }
}

void StoredImagesDialog::refreshAndJumpToFileName(const QString &fileName) {
    // refresh the media files to get the new data
    refreshMediaFiles();

    // look for the item to jump back to
    auto item = Utils::Gui::getTreeWidgetItemWithUserData(ui->fileTreeWidget, fileName);

    // set the previous item with a timer (didn't work without timer)
    QTimer::singleShot(0, this, [this, item]() { ui->fileTreeWidget->setCurrentItem(item); });
}

void StoredImagesDialog::on_fileTreeWidget_customContextMenuRequested(const QPoint &pos) {
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
        openAction = menu.addAction(tr("&Open image externally"));
        renameAction = menu.addAction(tr("&Rename image"));
        removeAction = menu.addAction(tr("&Delete images"));
        addAction = menu.addAction(tr("&Add images to current note"));
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

void StoredImagesDialog::on_noteTreeWidget_customContextMenuRequested(const QPoint &pos) {
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

void StoredImagesDialog::on_openFileButton_clicked() {
    QTreeWidgetItem *item = ui->fileTreeWidget->currentItem();

    if (item == nullptr) {
        return;
    }

    QString filePath = getFilePath(item);
    Utils::Misc::openPath(filePath);
}

void StoredImagesDialog::on_openFolderButton_clicked() {
    QTreeWidgetItem *item = ui->fileTreeWidget->currentItem();

    if (item == nullptr) {
        return;
    }

    QString filePath = getFilePath(item);
    Utils::Misc::openFolderSelect(filePath, QStringLiteral("show-stored-image-in-file-manager"));
}

void StoredImagesDialog::on_orphanedCheckBox_toggled(bool checked) {
    if (checked) {
        const QSignalBlocker blocker(ui->fileTreeWidget);
        Q_UNUSED(blocker)
        ui->currentNoteCheckBox->setChecked(false);
    }

    _orphanedImagesOnly = checked;
    refreshMediaFiles();
}

void StoredImagesDialog::on_currentNoteCheckBox_toggled(bool checked) {
    if (checked) {
        const QSignalBlocker blocker(ui->fileTreeWidget);
        Q_UNUSED(blocker)
        ui->orphanedCheckBox->setChecked(false);
    }

    _currentNoteOnly = checked;
    refreshMediaFiles();
}
