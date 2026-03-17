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
 */

#include "filenavigationwidget.h"

#include <entities/notefolder.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSet>
#include <QSignalBlocker>
#include <QTextCursor>
#include <QTextDocument>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QUrl>
#include <algorithm>
#include <limits>

#include "entities/note.h"
#include "mainwindow.h"
#include "qownnotesmarkdowntextedit.h"

namespace {
constexpr int PositionRole = Qt::UserRole;
constexpr int FilePathRole = Qt::UserRole + 1;
constexpr int FileTypeRole = Qt::UserRole + 2;
constexpr int FileNameRole = Qt::UserRole + 3;
constexpr int InvalidPosition = -1;

QString linkedFolderName(FileNavigationWidget::FileItemType type) {
    return type == FileNavigationWidget::FileItemType::MediaFile ? QStringLiteral("media")
                                                                 : QStringLiteral("attachments");
}

QString linkedFileLabel(FileNavigationWidget::FileItemType type) {
    return type == FileNavigationWidget::FileItemType::MediaFile ? QObject::tr("media file")
                                                                 : QObject::tr("attachment");
}

QString encodedFileName(const QString &fileName) {
    return QString(QUrl::toPercentEncoding(fileName));
}

struct SelectedLinkedFile {
    QString filePath;
    QString fileName;
    FileNavigationWidget::FileItemType type;
};

QRegularExpression linkedFileNameRegex(const QString &folderName, const QString &fileName) {
    return QRegularExpression(
        QStringLiteral(R"(((?:!\[.*?\]|\[.*?\])\((?:.*?%1\/.*?))%2((?:#[^)]+)?\)))")
            .arg(QRegularExpression::escape(folderName), QRegularExpression::escape(fileName)));
}

bool noteUsesLinkedFile(const QString &text, const QString &folderName, const QString &fileName) {
    const QString encodedPath = encodedFileName(fileName);

    return text.contains(linkedFileNameRegex(folderName, fileName)) ||
           ((encodedPath != fileName) &&
            text.contains(linkedFileNameRegex(folderName, encodedPath)));
}

bool replaceLinkedFileNameInText(QString &text, const QString &folderName,
                                 const QString &oldFileName, const QString &newFileName) {
    const QString oldEncodedFileName = encodedFileName(oldFileName);
    const QString newEncodedFileName = encodedFileName(newFileName);

    const QString originalText = text;
    text.replace(linkedFileNameRegex(folderName, oldFileName),
                 QStringLiteral("\\1%1\\2").arg(newFileName));

    if (oldEncodedFileName != oldFileName) {
        text.replace(linkedFileNameRegex(folderName, oldEncodedFileName),
                     QStringLiteral("\\1%1\\2").arg(newEncodedFileName));
    }

    return text != originalText;
}

bool removeLinkedFileLinksFromText(QString &text, const QString &folderName,
                                   const QString &fileName) {
    const QString encodedPath = encodedFileName(fileName);
    const QString originalText = text;

    text.replace(linkedFileNameRegex(folderName, fileName), QString());

    if (encodedPath != fileName) {
        text.replace(linkedFileNameRegex(folderName, encodedPath), QString());
    }

    return text != originalText;
}

QVector<int> findAffectedNoteIds(const QString &fileName, FileNavigationWidget::FileItemType type) {
    QVector<int> affectedNoteIds;
    const QString folderName = linkedFolderName(type);
    MainWindow *mainWindow = MainWindow::instance();
    const int currentNoteId = mainWindow == nullptr ? -1 : mainWindow->getCurrentNote().getId();
    QOwnNotesMarkdownTextEdit *textEdit =
        mainWindow == nullptr ? nullptr : mainWindow->activeNoteTextEdit();

    for (const Note &note : Note::fetchAll()) {
        QString noteText = note.getNoteText();

        if ((note.getId() == currentNoteId) && (textEdit != nullptr)) {
            noteText = textEdit->toPlainText();
        }

        if (noteUsesLinkedFile(noteText, folderName, fileName)) {
            affectedNoteIds.append(note.getId());
        }
    }

    return affectedNoteIds;
}

QVector<SelectedLinkedFile> selectedLinkedFiles(const QList<QTreeWidgetItem *> &items) {
    QVector<SelectedLinkedFile> linkedFiles;
    QSet<QString> seenFilePaths;

    for (const auto *item : items) {
        if (item == nullptr) {
            continue;
        }

        const QString filePath = item->data(0, FilePathRole).toString();
        const QString fileName = item->data(0, FileNameRole).toString();

        if (filePath.isEmpty() || fileName.isEmpty() || seenFilePaths.contains(filePath)) {
            continue;
        }

        seenFilePaths.insert(filePath);
        linkedFiles.append(
            {filePath, fileName,
             static_cast<FileNavigationWidget::FileItemType>(item->data(0, FileTypeRole).toInt())});
    }

    return linkedFiles;
}

QVector<int> findAffectedNoteIds(const QVector<SelectedLinkedFile> &linkedFiles) {
    QVector<int> affectedNoteIds;
    QSet<int> seenNoteIds;

    for (const auto &linkedFile : linkedFiles) {
        for (const int noteId : findAffectedNoteIds(linkedFile.fileName, linkedFile.type)) {
            if (seenNoteIds.contains(noteId)) {
                continue;
            }

            seenNoteIds.insert(noteId);
            affectedNoteIds.append(noteId);
        }
    }

    return affectedNoteIds;
}

bool removeLinkedFileLinksFromNotes(const QVector<int> &affectedNoteIds,
                                    const QVector<SelectedLinkedFile> &linkedFiles) {
    if (affectedNoteIds.isEmpty() || linkedFiles.isEmpty()) {
        return false;
    }

    MainWindow *mainWindow = MainWindow::instance();
    const int currentNoteId = mainWindow == nullptr ? -1 : mainWindow->getCurrentNote().getId();
    QOwnNotesMarkdownTextEdit *textEdit =
        mainWindow == nullptr ? nullptr : mainWindow->activeNoteTextEdit();
    QString updatedCurrentNoteText;
    bool currentNoteUpdated = false;
    bool anyNoteUpdated = false;

    for (const int noteId : affectedNoteIds) {
        Note note = Note::fetch(noteId);

        if (!note.isFetched()) {
            continue;
        }

        QString text = ((noteId == currentNoteId) && (textEdit != nullptr))
                           ? textEdit->toPlainText()
                           : note.getNoteText();
        bool noteChanged = false;

        for (const auto &linkedFile : linkedFiles) {
            noteChanged = removeLinkedFileLinksFromText(text, linkedFolderName(linkedFile.type),
                                                        linkedFile.fileName) ||
                          noteChanged;
        }

        if (!noteChanged || !note.storeNewText(text)) {
            continue;
        }

        anyNoteUpdated = true;

        if (noteId == currentNoteId) {
            updatedCurrentNoteText = text;
            currentNoteUpdated = true;
        }
    }

    if (currentNoteUpdated && (mainWindow != nullptr)) {
        mainWindow->setCurrentNoteText(updatedCurrentNoteText);
    }

    return anyNoteUpdated;
}
}    // namespace

FileNavigationWidget::FileNavigationWidget(QWidget *parent) : QTreeWidget(parent) {
    QObject::connect(this, &FileNavigationWidget::currentItemChanged, this,
                     &FileNavigationWidget::onCurrentItemChanged);
    QObject::connect(this, &FileNavigationWidget::itemClicked, this,
                     &FileNavigationWidget::onItemClicked);
    QObject::connect(this, &FileNavigationWidget::itemChanged, this,
                     &FileNavigationWidget::onItemChanged);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTreeWidget::customContextMenuRequested, this,
            &FileNavigationWidget::showContextMenu);
    setEditTriggers(QAbstractItemView::EditKeyPressed);
}

void FileNavigationWidget::parse(const QTextDocument *document, int textCursorPosition) {
    const QSignalBlocker blocker(this);
    Q_UNUSED(blocker)

    const QString text = document == nullptr ? QString() : document->toPlainText();
    setFileLinkNodes(
        parseText(text, NoteFolder::currentMediaPath(), NoteFolder::currentAttachmentsPath()),
        textCursorPosition);
}

void FileNavigationWidget::setFileLinkNodes(const QVector<FileLinkNode> &nodes,
                                            int textCursorPosition) {
    const QSignalBlocker blocker(this);
    Q_UNUSED(blocker)

    _cursorPosition = textCursorPosition;
    buildTree(nodes);
}

QVector<FileNavigationWidget::FileLinkNode> FileNavigationWidget::parseText(
    const QString &text, const QString &mediaPath, const QString &attachmentsPath) {
    QVector<FileLinkNode> nodes;

    struct FilePattern {
        QRegularExpression regex;
        FileItemType type;
        QString basePath;
    };

    const QVector<FilePattern> patterns = {
        {QRegularExpression(QStringLiteral(R"(!\[.*?\]\(.*media/(.+?)\))")),
         FileItemType::MediaFile, mediaPath},
        {QRegularExpression(QStringLiteral(R"(\[.*?\]\(.*attachments/(.+?)\))")),
         FileItemType::AttachmentFile, attachmentsPath},
    };

    for (const auto &pattern : patterns) {
        auto it = pattern.regex.globalMatch(text);

        while (it.hasNext()) {
            const auto match = it.next();
            const QString fileName = QUrl::fromPercentEncoding(match.captured(1).toUtf8());
            const int matchPosition = match.capturedStart(0);

            if (matchPosition < 0) {
                continue;
            }

            nodes.append({fileName, static_cast<int>(matchPosition), pattern.type,
                          pattern.basePath + QDir::separator() + fileName});
        }
    }

    std::sort(nodes.begin(), nodes.end(), [](const FileLinkNode &left, const FileLinkNode &right) {
        return left.pos < right.pos;
    });

    return nodes;
}

void FileNavigationWidget::buildTree(const QVector<FileLinkNode> &nodes) {
    if (_fileLinkNodes == nodes) {
        selectItemForCursorPosition(_cursorPosition);
        return;
    }

    _fileLinkNodes = nodes;

    clear();

    auto *mediaRootItem = new QTreeWidgetItem();
    mediaRootItem->setText(0, tr("Media files"));
    mediaRootItem->setData(0, PositionRole, InvalidPosition);
    mediaRootItem->setFlags(mediaRootItem->flags() & ~Qt::ItemIsSelectable);
    addTopLevelItem(mediaRootItem);

    auto *attachmentsRootItem = new QTreeWidgetItem();
    attachmentsRootItem->setText(0, tr("Attachments"));
    attachmentsRootItem->setData(0, PositionRole, InvalidPosition);
    attachmentsRootItem->setFlags(attachmentsRootItem->flags() & ~Qt::ItemIsSelectable);
    addTopLevelItem(attachmentsRootItem);

    for (const auto &node : _fileLinkNodes) {
        auto *item = new QTreeWidgetItem();
        item->setText(0, node.text);
        item->setData(0, PositionRole, node.pos);
        item->setData(0, FilePathRole, node.filePath);
        item->setData(0, FileTypeRole, static_cast<int>(node.type));
        item->setData(0, FileNameRole, node.text);
        item->setToolTip(0, node.filePath);
        item->setFlags(item->flags() | Qt::ItemIsEditable);

        if (node.type == FileItemType::MediaFile) {
            mediaRootItem->addChild(item);
        } else {
            attachmentsRootItem->addChild(item);
        }
    }

    expandAll();
    selectItemForCursorPosition(_cursorPosition);
}

void FileNavigationWidget::selectItemForCursorPosition(int position) {
    QTreeWidgetItem *itemToSelect = nullptr;
    int matchedPosition = InvalidPosition;

    QTreeWidgetItemIterator it(this);
    while (*it != nullptr) {
        auto *item = *it;
        const int itemPosition = item->data(0, PositionRole).toInt();

        if ((itemPosition != InvalidPosition) && (itemPosition <= position) &&
            (itemPosition >= matchedPosition)) {
            matchedPosition = itemPosition;
            itemToSelect = item;
        }

        ++it;
    }

    QSignalBlocker blocker(this);
    setCurrentItem(itemToSelect);
}

bool FileNavigationWidget::hasFileLinks() const { return !_fileLinkNodes.isEmpty(); }

void FileNavigationWidget::emitPositionForItem(const QTreeWidgetItem *item) {
    if (item == nullptr) {
        return;
    }

    const int position = item->data(0, PositionRole).toInt();
    if (position != InvalidPosition) {
        emit positionClicked(position);
    }
}

void FileNavigationWidget::onCurrentItemChanged(QTreeWidgetItem *current,
                                                QTreeWidgetItem *previous) {
    Q_UNUSED(previous)

    emitPositionForItem(current);
}

void FileNavigationWidget::onItemClicked(QTreeWidgetItem *current, int column) {
    Q_UNUSED(column)

    emitPositionForItem(current);
}

void FileNavigationWidget::onItemChanged(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column)

    if (item == nullptr) {
        return;
    }

    const QString oldFileName = item->data(0, FileNameRole).toString();
    const QString newFileName = item->text(0).trimmed();
    const auto type = static_cast<FileItemType>(item->data(0, FileTypeRole).toInt());

    if (oldFileName.isEmpty()) {
        return;
    }

    if (newFileName.isEmpty() || (newFileName == oldFileName)) {
        const QSignalBlocker blocker(this);
        Q_UNUSED(blocker)
        item->setText(0, oldFileName);
        return;
    }

    if (newFileName.contains(QDir::separator()) || newFileName.contains(QLatin1Char('/')) ||
        newFileName.contains(QLatin1Char('\\')) || (newFileName == QStringLiteral(".")) ||
        (newFileName == QStringLiteral(".."))) {
        QMessageBox::warning(
            this, tr("Invalid file name"),
            tr("The file name <strong>%1</strong> is invalid.").arg(newFileName.toHtmlEscaped()));

        const QSignalBlocker blocker(this);
        Q_UNUSED(blocker)
        item->setText(0, oldFileName);
        return;
    }

    if (!renameLinkedFile(item, oldFileName, newFileName, type)) {
        const QSignalBlocker blocker(this);
        Q_UNUSED(blocker)
        item->setText(0, oldFileName);
    }
}

void FileNavigationWidget::keyPressEvent(QKeyEvent *event) {
    if ((event != nullptr) && (state() != QAbstractItemView::EditingState) &&
        ((event->key() == Qt::Key_Delete) || (event->key() == Qt::Key_Backspace))) {
        auto *item = currentItem();

        if ((item != nullptr) && !item->data(0, FilePathRole).toString().isEmpty()) {
            deleteSelectedLinkedFiles();
            event->accept();
            return;
        }
    }

    QTreeWidget::keyPressEvent(event);
}

bool FileNavigationWidget::deleteSelectedLinkedFiles() {
    QVector<SelectedLinkedFile> linkedFiles = selectedLinkedFiles(selectedItems());

    if (linkedFiles.isEmpty()) {
        auto *item = currentItem();

        if ((item == nullptr) || item->data(0, FilePathRole).toString().isEmpty()) {
            return false;
        }

        QList<QTreeWidgetItem *> items;
        items.append(item);
        linkedFiles = selectedLinkedFiles(items);
    }

    if (linkedFiles.isEmpty()) {
        return false;
    }

    const int selectedFileCount = linkedFiles.count();
    const QVector<int> affectedNoteIds = findAffectedNoteIds(linkedFiles);
    const int affectedNoteCount = affectedNoteIds.count();

    if (Utils::Gui::question(
            this, tr("Delete linked files"),
            tr("Delete <strong>%n</strong> selected linked file(s)? You can also remove "
               "their links from <strong>%1</strong> note file(s) afterwards.",
               "", selectedFileCount)
                .arg(affectedNoteCount),
            QStringLiteral("delete-linked-files")) != QMessageBox::Yes) {
        return true;
    }

    QVector<SelectedLinkedFile> deletedLinkedFiles;
    QVector<QString> failedFilePaths;

    for (const auto &linkedFile : linkedFiles) {
        QFile file(linkedFile.filePath);

        if (!file.exists() || !file.remove()) {
            failedFilePaths.append(linkedFile.filePath);
            continue;
        }

        deletedLinkedFiles.append(linkedFile);
    }

    if (!failedFilePaths.isEmpty()) {
        const QString message = failedFilePaths.count() == 1
                                    ? tr("Deleting the file <strong>%1</strong> failed!")
                                          .arg(failedFilePaths.first().toHtmlEscaped())
                                    : tr("Deleting <strong>%n</strong> linked file(s) failed!", "",
                                         failedFilePaths.count());
        QMessageBox::warning(this, tr("File deleting failed"), message);
    }

    if (!deletedLinkedFiles.isEmpty()) {
        const QVector<int> affectedDeletedNoteIds = findAffectedNoteIds(deletedLinkedFiles);
        const int affectedDeletedNoteCount = affectedDeletedNoteIds.count();

        if ((affectedDeletedNoteCount > 0) &&
            (Utils::Gui::questionNoSkipOverride(
                 this, tr("Remove linked file references"),
                 tr("The deleted file(s) are used in <strong>%n</strong> note file(s). "
                    "Would you like to remove those media and attachment links "
                    "everywhere? This will update <strong>%n</strong> note file(s).",
                    "", affectedDeletedNoteCount),
                 QStringLiteral("note-remove-linked-file-references")) == QMessageBox::Yes)) {
            removeLinkedFileLinksFromNotes(affectedDeletedNoteIds, deletedLinkedFiles);
        }
    }

    refreshFromCurrentNote();
    return true;
}

bool FileNavigationWidget::renameLinkedFile(QTreeWidgetItem *item, const QString &oldFileName,
                                            const QString &newFileName, FileItemType type) {
    const QString oldFilePath = item->data(0, FilePathRole).toString();
    QFile oldFile(oldFilePath);

    if (!oldFile.exists()) {
        QMessageBox::warning(this, tr("File doesn't exist"),
                             tr("The file <strong>%1</strong> doesn't exist, you cannot rename it!")
                                 .arg(oldFilePath.toHtmlEscaped()));
        return false;
    }

    const QFileInfo oldFileInfo(oldFilePath);
    const QString newFilePath = oldFileInfo.dir().filePath(newFileName);
    QFile newFile(newFilePath);

    if (newFile.exists()) {
        QMessageBox::warning(
            this, tr("File exists"),
            tr("File <strong>%1</strong> already exists, you need to remove it before choosing "
               "<strong>%2</strong> as new filename!")
                .arg(newFilePath.toHtmlEscaped(), newFileName.toHtmlEscaped()));
        return false;
    }

    if (!oldFile.rename(newFilePath)) {
        QMessageBox::warning(
            this, tr("File renaming failed"),
            tr("Renaming of file <strong>%1</strong> failed!").arg(oldFilePath.toHtmlEscaped()));
        return false;
    }

    const QVector<int> affectedNoteIds = findAffectedNoteIds(oldFileName, type);
    const int affectedNoteCount = affectedNoteIds.count();

    if (affectedNoteCount > 0) {
        const QString fileLabel = linkedFileLabel(type);
        const QString dialogIdentifier =
            type == FileItemType::MediaFile
                ? QStringLiteral("note-replace-linked-media-file-name")
                : QStringLiteral("note-replace-linked-attachment-file-name");
        const QMessageBox::StandardButton answer = Utils::Gui::questionNoSkipOverride(
            this, tr("File name changed"),
            tr("The %1 <strong>%2</strong> is used in <strong>%n</strong> note file(s). "
               "Would you like to refactor that filename everywhere? This will update "
               "<strong>%n</strong> note file(s).",
               "", affectedNoteCount)
                .arg(fileLabel, oldFileName.toHtmlEscaped()),
            dialogIdentifier);

        if (answer == QMessageBox::Yes) {
            MainWindow *mainWindow = MainWindow::instance();
            const int currentNoteId =
                mainWindow == nullptr ? -1 : mainWindow->getCurrentNote().getId();
            QOwnNotesMarkdownTextEdit *textEdit =
                mainWindow == nullptr ? nullptr : mainWindow->activeNoteTextEdit();
            QString updatedCurrentNoteText;
            bool currentNoteUpdated = false;

            for (const int noteId : affectedNoteIds) {
                Note note = Note::fetch(noteId);

                if (!note.isFetched()) {
                    continue;
                }

                QString text = ((noteId == currentNoteId) && (textEdit != nullptr))
                                   ? textEdit->toPlainText()
                                   : note.getNoteText();

                if (!replaceLinkedFileNameInText(text, linkedFolderName(type), oldFileName,
                                                 newFileName)) {
                    continue;
                }

                if (!note.storeNewText(text)) {
                    continue;
                }

                if (noteId == currentNoteId) {
                    updatedCurrentNoteText = text;
                    currentNoteUpdated = true;
                }
            }

            if (currentNoteUpdated && (mainWindow != nullptr)) {
                mainWindow->setCurrentNoteText(updatedCurrentNoteText);
            }
        }
    }

    refreshFromCurrentNote();

    return true;
}

void FileNavigationWidget::refreshFromCurrentNote() {
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == nullptr) {
        return;
    }

    auto *textEdit = mainWindow->activeNoteTextEdit();
    if (textEdit != nullptr) {
        _fileLinkNodes.clear();
        parse(textEdit->document(), textEdit->textCursor().position());
    }
}

void FileNavigationWidget::showContextMenu(const QPoint &pos) {
    auto *item = itemAt(pos);
    if ((item == nullptr) || item->data(0, FilePathRole).toString().isEmpty()) {
        return;
    }

    setCurrentItem(item);

    QMenu menu(this);
    auto *openAction = menu.addAction(tr("&Open file externally"));
    auto *deleteAction = menu.addAction(tr("&Delete file"));

    QAction *selectedAction = menu.exec(mapToGlobal(pos));
    if (selectedAction == deleteAction) {
        deleteSelectedLinkedFiles();
        return;
    }

    if (selectedAction != openAction) {
        return;
    }

    const QString filePath = item->data(0, FilePathRole).toString();
    if (!filePath.isEmpty()) {
        Utils::Misc::openPath(filePath);
    }
}
