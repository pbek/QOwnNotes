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

#include "mediainsertionmanager.h"

#include <dialogs/attachmentdialog.h>
#include <dialogs/imagedialog.h>
#include <entities/note.h>
#include <entities/notesubfolder.h>
#include <services/scriptingservice.h>
#include <utils/misc.h>

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QInputDialog>
#include <QMenu>
#include <QMimeData>
#include <QRegularExpression>
#include <QTemporaryFile>
#include <QTextCursor>
#include <QUrl>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widgets/qownnotesmarkdowntextedit.h"

MediaInsertionManager::MediaInsertionManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                                             QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui) {}

/**
 * @brief Inserts a chosen image at the current cursor position in the note text
 * edit
 */
void MediaInsertionManager::on_actionInsert_image_triggered() {
    auto *dialog = new ImageDialog(_mainWindow);
    const int ret = dialog->exec();

    if (ret == QDialog::Accepted) {
        QString title = dialog->getImageTitle();

        if (dialog->isDisableCopying()) {
            QString pathOrUrl = dialog->getFilePathOrUrl();
            auto url = QUrl(pathOrUrl);

            if (!url.isValid()) {
                return;
            }

            if (url.scheme() == QStringLiteral("file")) {
                pathOrUrl = url.toLocalFile();
            }

            if (!url.scheme().startsWith(QStringLiteral("http"))) {
                pathOrUrl = _mainWindow->currentNote.relativeFilePath(pathOrUrl);
            }

#ifdef Q_OS_WIN32
            // make sure a local path on a different drive really works
            if (Utils::Misc::fileExists(pathOrUrl)) {
                pathOrUrl = QUrl::toPercentEncoding(pathOrUrl).prepend("file:///");
            }
#endif

            // title must not be empty
            if (title.isEmpty()) {
                title = QStringLiteral("img");
            }

            insertNoteText(QStringLiteral("![") + title + QStringLiteral("](") + pathOrUrl +
                           QStringLiteral(")"));
        } else {
            QFile *file = dialog->getImageFile();

            if (file->size() > 0) {
                insertMedia(file, title);
            }
        }
    }

    delete (dialog);
}

/**
 * Inserts a media file into the current note
 */
bool MediaInsertionManager::insertMedia(QFile *file, QString title) {
    QString text =
        _mainWindow->currentNote.getInsertMediaMarkdown(file, true, false, std::move(title));

    if (!text.isEmpty()) {
        ScriptingService *scriptingService = ScriptingService::instance();
        // attempts to ask a script for another Markdown text
        text = scriptingService->callInsertMediaHook(file, text);
        qDebug() << __func__ << " - 'text': " << text;

        insertNoteText(text);

        return true;
    }

    return false;
}

void MediaInsertionManager::insertNoteText(const QString &text) {
    QOwnNotesMarkdownTextEdit *textEdit = _mainWindow->activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();

    // if we try to insert media in the first line of the note (aka.
    // note name) move the cursor to the last line
    if (c.block() == textEdit->document()->firstBlock()) {
        c.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        textEdit->setTextCursor(c);
    }

    // insert the image link
    c.insertText(text);
}

/**
 * Inserts text as a file attachment into the current note
 */
bool MediaInsertionManager::insertTextAsAttachment(const QString &text) {
    if (text.isEmpty()) {
        return false;
    }

    // Select the file extension
    QStringList fileExtensions;
    fileExtensions << QStringLiteral("txt") << QStringLiteral("json") << QStringLiteral("xml")
                   << QStringLiteral("log") << QStringLiteral("csv") << QStringLiteral("md")
                   << QStringLiteral("cpp") << QStringLiteral("nix") << QStringLiteral("html")
                   << QStringLiteral("yaml") << QStringLiteral("sql") << QStringLiteral("py")
                   << QStringLiteral("js") << QStringLiteral("ini");

    QString extension = Utils::Misc::detectFileFormat(text);
    int index = fileExtensions.indexOf(extension);

    bool ok;
    QString fileExtension = QInputDialog::getItem(_mainWindow, tr("File extension"),
                                                  tr("Extension of file attachment:"),
                                                  fileExtensions, index, true, &ok);

    if (!ok) {
        return false;
    }

    QString fileBaseName = QInputDialog::getText(
        _mainWindow, tr("File base name"),
        tr("Base name of file attachment:") + QStringLiteral("<br><i>") +
            tr("Leave empty for automatic name", "Leave the base name empty for automatic name") +
            QStringLiteral("</i>"),
        QLineEdit::Normal, QString(), &ok);

    if (!ok) {
        return false;
    }

    QString templateName = QDir::tempPath() + QDir::separator() + QStringLiteral("text-XXXXXX");

    if (!fileExtension.isEmpty()) {
        templateName += QStringLiteral(".") + fileExtension;
    }

    // Create a temporary file for the attachment
    auto *tempFile = new QTemporaryFile(templateName);

    if (!tempFile->open()) {
        _mainWindow->showStatusBarMessage(tr("Temporary file can't be opened"),
                                          QStringLiteral("❌"), 3000);

        return false;
    }

    // write file data to the temporary file
    tempFile->write(text.toUtf8());
    tempFile->flush();
    tempFile->close();

    // We need a reference to tempFile, or else it will be gone before inserted
    auto *file = new QFile(tempFile->fileName());

    // Determine the filename for the attachment
    QString fileName;
    if (fileBaseName.isEmpty()) {
        auto currentDate = QDateTime::currentDateTime();
        fileName = QStringLiteral("file-") +
                   currentDate.toString(QStringLiteral("yyyy-MM-dd-HH'h'mm's'ss"));
    } else {
        fileName = fileBaseName;
    }

    // If there is a file extension, append it to the filename
    if (!fileExtension.isEmpty()) {
        fileName += QStringLiteral(".") + fileExtension;
    }

    bool result = insertAttachment(file, fileName, fileName);

    if (result) {
        _mainWindow->showStatusBarMessage(tr("Inserted text as text attachment file"),
                                          QStringLiteral("📄"), 3000);
    }

    // For some reason the temp file on disk will not be removed automatically
    // without deleting the pointer manually
    delete tempFile;

    return result;
}

/**
 * Inserts a file attachment into the current note
 */
bool MediaInsertionManager::insertAttachment(QFile *file, const QString &title,
                                             const QString &fileName) {
    QString text =
        _mainWindow->currentNote.getInsertAttachmentMarkdown(file, title, false, fileName);

    if (!text.isEmpty()) {
        ScriptingService *scriptingService = ScriptingService::instance();
        // attempts to ask a script for another Markdown text
        text = scriptingService->callInsertAttachmentHook(file, text);
        qDebug() << __func__ << " - 'text': " << text;

        QOwnNotesMarkdownTextEdit *textEdit = _mainWindow->activeNoteTextEdit();
        QTextCursor c = textEdit->textCursor();

        // if we try to insert the attachment in the first line of the note
        // (aka. note name) move the cursor to the last line
        if (c.block() == textEdit->document()->firstBlock()) {
            c.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
            textEdit->setTextCursor(c);
        }

        // add a space if we are not at the start of a line or if there is no
        // space in front of the current cursor position
        c.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        if (!c.atBlockStart() && c.selectedText() != QStringLiteral(" ")) {
            text = QStringLiteral(" ") + text;
        }

        // insert the attachment link
        c = textEdit->textCursor();
        c.insertText(text);

        return true;
    }

    return false;
}

/**
 * Handles the inserting of media files and notes from a mime data, for example
 * produced by a drop event or a paste action
 */
void MediaInsertionManager::handleInsertingFromMimeData(const QMimeData *mimeData) {
    //    qDebug() << __func__ << " - 'mimeData->hasText()': " << mimeData->hasText();
    //    qDebug() << __func__ << " - 'mimeData->hasHtml()': " << mimeData->hasHtml();
    //    qDebug() << __func__ << " - 'mimeData->hasImage()': " << mimeData->hasImage();
    //    qDebug() << __func__ << " - 'mimeData->hasUrls()': " << mimeData->hasUrls();

    // prevent pasting if note editor is in read-only mode
    if (_ui->noteTextEdit->isReadOnly()) {
        return;
    }

    // check if a QML wants to set the inserted text
    if (mimeData->hasText() || mimeData->hasHtml()) {
        ScriptingService *scriptingService = ScriptingService::instance();
        QString text = scriptingService->callInsertingFromMimeDataHook(mimeData);

        if (!text.isEmpty()) {
            QOwnNotesMarkdownTextEdit *textEdit = _mainWindow->activeNoteTextEdit();
            QTextCursor c = textEdit->textCursor();

            // insert text from QML
            c.insertText(text);

            return;
        }
    }

    if (mimeData->hasUrls()) {
        int successCount = 0;
        int failureCount = 0;
        int skipCount = 0;

        const auto urls = mimeData->urls();
        for (const QUrl &url : urls) {
            const QString path(url.toLocalFile());
            const QFileInfo fileInfo(path);
            qDebug() << __func__ << " - 'path': " << path;

            if (fileInfo.isReadable()) {
                auto *file = new QFile(path);

                // only allow Markdown and text files to be copied as note
                if (isValidNoteFile(file)) {
                    const NoteSubFolder noteSubFolder = NoteSubFolder::activeNoteSubFolder();
                    const QString noteSubFolderPath = noteSubFolder.fullPath();

                    // copy file to notes path
                    const bool success =
                        file->copy(noteSubFolderPath + QDir::separator() + fileInfo.fileName());

                    if (success) {
                        successCount++;
                    } else {
                        failureCount++;
                    }
                    // only allow image files to be inserted as image
                } else if (isValidMediaFile(file)) {
                    _mainWindow->showStatusBarMessage(tr("Inserting image"), QStringLiteral("🖼️"),
                                                      0);

                    // insert the image
                    insertMedia(file);

                    _mainWindow->showStatusBarMessage(tr("Done inserting image"),
                                                      QStringLiteral("🖼️"), 3000);
                } else {
                    _mainWindow->showStatusBarMessage(tr("Inserting attachment"),
                                                      QStringLiteral("🖼️"), 0);

                    // inserting the attachment
                    insertAttachment(file);

                    _mainWindow->showStatusBarMessage(tr("Done inserting attachment"),
                                                      QStringLiteral("📥️"), 3000);
                }

                delete file;
            } else {
                skipCount++;
            }
        }

        QString message;
        if (successCount > 0) {
            message += tr("Copied %n note(s) to %1", "", successCount).arg(_mainWindow->notesPath);
            _mainWindow->on_action_Reload_note_folder_triggered();
        }

        if (failureCount > 0) {
            if (!message.isEmpty()) {
                message += QStringLiteral(", ");
            }

            message +=
                tr("Failed to copy %n note(s) (most likely already existing)", "", failureCount);
        }

        if (skipCount > 0) {
            if (!message.isEmpty()) {
                message += QStringLiteral(", ");
            }

            message +=
                tr("Skipped copying of %n note(s) "
                   "(no Markdown or text file or not readable)",
                   "", skipCount);
        }

        if (!message.isEmpty()) {
            _mainWindow->showStatusBarMessage(message, QStringLiteral("⤵️️"), 5000);
        }
    } else if (mimeData->hasImage()) {
        // get the image from mime data
        QImage image = mimeData->imageData().value<QImage>();

        if (!image.isNull()) {
            _mainWindow->showStatusBarMessage(tr("Saving temporary image"), QStringLiteral("🖼️"),
                                              0);

            QTemporaryFile tempFile(QDir::tempPath() + QDir::separator() +
                                    QStringLiteral("qownnotes-media-XXXXXX.png"));

            if (tempFile.open()) {
                // save temporary png image
                image.save(tempFile.fileName(), "PNG");

                // insert media into note
                auto *file = new QFile(tempFile.fileName());

                _mainWindow->showStatusBarMessage(tr("Inserting image"), QStringLiteral("🖼️"), 0);
                insertMedia(file);
                delete file;

                _mainWindow->showStatusBarMessage(tr("Done inserting image"), QStringLiteral("🖼️"),
                                                  3000);
            } else {
                _mainWindow->showStatusBarMessage(tr("Temporary file can't be opened"),
                                                  QStringLiteral("❌️"), 3000);
            }
        }
    } else if (mimeData->hasText() || mimeData->hasHtml()) {
        const auto textEdit = _mainWindow->activeNoteTextEdit();
        auto rect = textEdit->cursorRect(textEdit->textCursor());
        //    const auto pos = textEdit->cursor().pos();
        const QPoint globalPos = textEdit->mapToGlobal(rect.bottomRight());
        QMenu menu;

        // We need to fetch the text and html from the mime data here, because the mimeData object
        // may not be available anymore after the menu was closed and accessing it may cause a crash
        const auto text = mimeData->text();
        const auto html = mimeData->html();

        QAction *htmlAction = menu.addAction(tr("Paste &HTML as Markdown"));
        QAction *textAttachmentAction = menu.addAction(tr("Paste as &text file attachment"));
        QAction *selectedItem = menu.exec(globalPos);

        if (selectedItem == htmlAction) {
            insertHtmlAsMarkdownIntoCurrentNote(html);
        } else if (selectedItem == textAttachmentAction) {
            // Insert text as an attachment file
            insertTextAsAttachment(text);
        }
    }
}

/**
 * Inserts html as Markdown in the current note
 * Images are also downloaded
 */
void MediaInsertionManager::insertHtmlAsMarkdownIntoCurrentNote(QString html) {
    // convert html tags to Markdown
    html = Utils::Misc::htmlToMarkdown(std::move(html));

    // match image tags
    static const QRegularExpression re(QStringLiteral("<img.+?src=[\"'](.+?)[\"'].*?>"),
                                       QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = re.globalMatch(html);

    // find, download locally and replace all images
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        const QString imageTag = match.captured(0);
        const QString imageUrlText = match.captured(1).trimmed();
        // try to import a media file into the current note
        QString markdownCode = _mainWindow->currentNote.importMediaFromDataUrl(imageUrlText);

        // if no inline-image was detected try to download the url
        if (markdownCode.isEmpty()) {
            const QUrl imageUrl = QUrl(imageUrlText);

            qDebug() << __func__ << " - 'imageUrl': " << imageUrl;

            if (!imageUrl.isValid()) {
                continue;
            }

            _mainWindow->showStatusBarMessage(tr("Downloading %1").arg(imageUrl.toString()),
                                              QStringLiteral("⬇️️"), 0);

            // download the image and get the media Markdown code for it
            markdownCode = _mainWindow->currentNote.downloadUrlToMedia(imageUrl);
        }

        if (!markdownCode.isEmpty()) {
            // replace the image tag with Markdown code
            html.replace(imageTag, markdownCode);
        }
    }

    _mainWindow->showStatusBarMessage(tr("Downloading images finished"), QStringLiteral("🖼️"),
                                      3000);

    // remove all html tags
    static const QRegularExpression tagRE(QStringLiteral("<.+?>"));
    html.remove(tagRE);

    // unescape some html special characters
    html = Utils::Misc::unescapeHtml(std::move(html)).trimmed();

    QOwnNotesMarkdownTextEdit *textEdit = _mainWindow->activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();

    c.insertText(html);
}

/**
 * Evaluates if file is a media file
 */
bool MediaInsertionManager::isValidMediaFile(QFile *file) {
    const QStringList mediaExtensions = QStringList({"jpg", "png", "gif", "webp"});
    const QFileInfo fileInfo(file->fileName());
    const QString extension = fileInfo.suffix();
    return mediaExtensions.contains(extension, Qt::CaseInsensitive);
}

/**
 * Evaluates if file is a note file
 */
bool MediaInsertionManager::isValidNoteFile(QFile *file) {
    auto mediaExtensions = Note::noteFileExtensionList();
    const QFileInfo fileInfo(file->fileName());
    const QString extension = fileInfo.suffix();
    return mediaExtensions.contains(extension, Qt::CaseInsensitive);
}

void MediaInsertionManager::on_actionPaste_image_triggered() { pasteMediaIntoNote(); }

/**
 * Handles the pasting of media into notes
 */
void MediaInsertionManager::pasteMediaIntoNote() {
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData(QClipboard::Clipboard);
    handleInsertingFromMimeData(mimeData);
}

/**
 * Imports a DataUrl as file into QOwnNotes and inserts it into the current note
 * This currently only supports images
 * @param dataUrl
 * @return
 */
bool MediaInsertionManager::insertDataUrlAsFileIntoCurrentNote(const QString &dataUrl) {
    QString markdownCode = _mainWindow->currentNote.importMediaFromDataUrl(dataUrl);

    if (markdownCode.isEmpty()) {
        return false;
    }

    insertNoteText(markdownCode);

    return true;
}

/**
 * Inserts a file as attachment
 */
void MediaInsertionManager::on_actionInsert_attachment_triggered() {
    auto *dialog = new AttachmentDialog(_mainWindow);
    dialog->exec();

    if (dialog->result() == QDialog::Accepted) {
        insertAttachment(dialog->getFile(), dialog->getTitle());
    }

    delete (dialog);
}
