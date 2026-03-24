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

#include "navigationmanager.h"

#include <entities/note.h>
#include <entities/notefolder.h>
#include <entities/notehistory.h>
#include <services/databaseservice.h>
#include <services/settingsservice.h>
#include <utils/gui.h>
#include <widgets/backlinkwidget.h>
#include <widgets/filenavigationwidget.h>
#include <widgets/navigationwidget.h>
#include <widgets/qownnotesmarkdowntextedit.h>

#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <QScrollBar>
#include <QSqlDatabase>
#include <QTabWidget>
#include <QTextCursor>
#include <QUrl>
#include <QtConcurrent/QtConcurrent>

#include "mainwindow.h"
#include "ui_mainwindow.h"

NavigationManager::NavigationManager(MainWindow *mainWindow, Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui) {}

/**
 * Starts the parsing for the navigation widget
 */
void NavigationManager::startNavigationParser() {
    const QTextCursor cursor = _mainWindow->activeNoteTextEdit()->textCursor();
    const int navigationSelectionPosition = NavigationWidget::headingPositionForCursor(cursor);

    _ui->navigationWidget->parse(_mainWindow->activeNoteTextEdit()->document(),
                                 navigationSelectionPosition);

    updateFileNavigationTab();
    updateBacklinkNavigationTab();

    if (_ui->navigationTabWidget->count() == 0) {
        return;
    }

    if (_ui->navigationTabWidget->currentIndex() < 0) {
        _ui->navigationTabWidget->setCurrentIndex(0);
    }
}

void NavigationManager::updateFileNavigationTab() {
    auto *noteTextEdit = _mainWindow->activeNoteTextEdit();
    if (noteTextEdit == nullptr) {
        return;
    }

    const QString noteText = noteTextEdit->document()->toPlainText();
    const int cursorPosition = noteTextEdit->textCursor().position();
    const QString mediaPath = NoteFolder::currentMediaPath();
    const QString attachmentsPath = NoteFolder::currentAttachmentsPath();
    const quint64 requestId = ++_mainWindow->_fileNavigationUpdateRequestId;
    QPointer<MainWindow> window(_mainWindow);
    NavigationManager *navManager = this;

    QFuture<void> future = QtConcurrent::run([window, navManager, noteText, cursorPosition,
                                              mediaPath, attachmentsPath, requestId]() {
        const auto nodes = FileNavigationWidget::parseText(noteText, mediaPath, attachmentsPath);

        if (window.isNull()) {
            return;
        }

        auto updateFileNavigation = [window, navManager, requestId, cursorPosition, nodes]() {
            if (window.isNull() || (requestId != window->_fileNavigationUpdateRequestId)) {
                return;
            }

            window->ui->fileNavigationWidget->setFileLinkNodes(nodes, cursorPosition);
            navManager->setOptionalNavigationTabVisible(
                window->ui->fileNavigationTab, MainWindow::tr("Files"), 1,
                window->ui->fileNavigationWidget->hasFileLinks());
        };

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        QMetaObject::invokeMethod(window, updateFileNavigation, Qt::QueuedConnection);
#else
        QTimer::singleShot(0, window, updateFileNavigation);
#endif
    });
    Q_UNUSED(future)
}

void NavigationManager::updateBacklinkNavigationTab() {
    const Note note = _mainWindow->currentNote;
    Note noteCopy(note);
    const quint64 requestId = ++_mainWindow->_backlinkNavigationUpdateRequestId;
    QPointer<MainWindow> window(_mainWindow);
    NavigationManager *navManager = this;

    QFuture<void> future = QtConcurrent::run([window, navManager, noteCopy, requestId]() mutable {
        const QString connectionName = DatabaseService::generateConnectionName();
        QHash<Note, QSet<LinkHit>> backlinks;

        {
            QSqlDatabase db = DatabaseService::createSharedMemoryDatabase(connectionName);
            db.open();
            DatabaseService::applySharedMemoryDatabasePragmas(db);
            backlinks = noteCopy.findReverseLinkNotes(connectionName);
            db.close();
        }

        QSqlDatabase::removeDatabase(connectionName);

        if (window.isNull()) {
            return;
        }

        auto updateBacklinks = [window, navManager, requestId, backlinks]() {
            if (window.isNull() || (requestId != window->_backlinkNavigationUpdateRequestId)) {
                return;
            }

            window->ui->backlinkWidget->setBacklinks(backlinks);
            navManager->setOptionalNavigationTabVisible(window->ui->backlinkTab,
                                                        MainWindow::tr("Backlinks"), 2,
                                                        window->ui->backlinkWidget->hasBacklinks());
        };

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        QMetaObject::invokeMethod(window, updateBacklinks, Qt::QueuedConnection);
#else
        QTimer::singleShot(0, window, updateBacklinks);
#endif
    });
    Q_UNUSED(future)
}

void NavigationManager::selectNavigationItemAtPosition(int position) {
    if (_ui->navigationWidget->isVisible()) {
        _ui->navigationWidget->selectItemForCursorPosition(position);
    } else if (_ui->fileNavigationWidget->isVisible()) {
        _ui->fileNavigationWidget->selectItemForCursorPosition(position);
    }
}

void NavigationManager::setOptionalNavigationTabVisible(QWidget *tab, const QString &title,
                                                        int preferredIndex, bool visible) {
    const int existingIndex = _ui->navigationTabWidget->indexOf(tab);

    if (visible) {
        if (existingIndex >= 0) {
            return;
        }

        _ui->navigationTabWidget->insertTab(preferredIndex, tab, title);
        return;
    }

    if (existingIndex < 0) {
        return;
    }

    const bool wasCurrentTab = _ui->navigationTabWidget->currentWidget() == tab;
    _ui->navigationTabWidget->removeTab(existingIndex);

    if (wasCurrentTab && (_ui->navigationTabWidget->count() > 0)) {
        _ui->navigationTabWidget->setCurrentIndex(0);
    }
}

/**
 * Jumps to the position that was clicked in the navigation widget
 */
void NavigationManager::onNavigationWidgetPositionClicked(int position) {
    QOwnNotesMarkdownTextEdit *textEdit = _mainWindow->activeNoteTextEdit();

    // set the focus first so the preview also scrolls to the headline
    textEdit->setFocus();

    QTextCursor c = textEdit->textCursor();

    // if the current position of the cursor is smaller than the position
    // where we want to jump to set the cursor to the end of the note to make
    // sure it scrolls up, not down
    // everything is visible that way
    if (c.position() < position) {
        c.movePosition(QTextCursor::End);
        textEdit->setTextCursor(c);
    }

    c.setPosition(position);

    // select the text of the headline
    c.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

    textEdit->setTextCursor(c);

    // update the preview-slider
    _mainWindow->noteTextSliderValueChanged(textEdit->verticalScrollBar()->value(), true);

    // set focus back to the navigation widget, so you can use the
    // keyboard to navigate
    _ui->navigationWidget->setFocus();
}

void NavigationManager::onFileNavigationWidgetPositionClicked(int position) {
    QOwnNotesMarkdownTextEdit *textEdit = _mainWindow->activeNoteTextEdit();

    textEdit->setFocus();

    QTextCursor c = textEdit->textCursor();

    if (c.position() < position) {
        c.movePosition(QTextCursor::End);
        textEdit->setTextCursor(c);
    }

    c.setPosition(position);
    textEdit->setTextCursor(c);

    _mainWindow->noteTextSliderValueChanged(textEdit->verticalScrollBar()->value(), true);

    _ui->fileNavigationWidget->setFocus();
}

/**
 * Handles renaming of headings from the navigation widget
 * Updates the heading text in the note
 */
void NavigationManager::onNavigationWidgetHeadingRenamed(int position, const QString &oldText,
                                                         const QString &newText) {
    // Allow note editing if it is currently disabled, otherwise the note won't be updated
    _mainWindow->allowNoteEditing();

    QOwnNotesMarkdownTextEdit *textEdit = _mainWindow->activeNoteTextEdit();
    QTextDocument *doc = textEdit->document();

    // Find the block at the given position
    QTextBlock block = doc->findBlock(position);
    if (!block.isValid()) {
        return;
    }

    // Get the current text of the block
    QString blockText = block.text();

    // Extract the heading level (number of # symbols)
    static const QRegularExpression headingRegex(QStringLiteral("^(#+)\\s+"));
    QRegularExpressionMatch match = headingRegex.match(blockText);

    if (!match.hasMatch()) {
        return;
    }

    QString headingPrefix = match.captured(1);    // The "###" part
    QString newBlockText = headingPrefix + " " + newText;

    // Check for backlinks that reference this heading and ask user if they want to update them
    // We need to do this BEFORE we update the text in the document, otherwise the QMessageBox
    // dialog that confirms this change will crash the app
    updateBacklinksAfterHeadingRename(oldText, newText);

    // Update the text in the document
    // We need to be careful to only replace the text content, not the block delimiters
    QTextCursor cursor(block);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.insertText(newBlockText);
}

/**
 * Updates backlinks after a heading has been renamed
 * Scans all notes that link to the current note and updates heading references
 * Also checks the current note itself for TOC links (e.g., [text](#old-heading))
 */
void NavigationManager::updateBacklinksAfterHeadingRename(const QString &oldHeading,
                                                          const QString &newHeading) {
    // Generate the old and new heading fragments (URL encoded)
    // Heading fragments in links are URL-encoded (spaces become %20, etc.)
    QString oldFragment = QString(QUrl::toPercentEncoding(oldHeading));
    QString newFragment = QString(QUrl::toPercentEncoding(newHeading));

    // Get the relative file path of the current note for precise link matching
    QString currentNoteRelativePath = _mainWindow->currentNote.relativeNoteFilePath();

    // Collect notes that actually contain links with the old heading fragment
    QVector<Note> notesWithHeadingLinks;

    // First, check the current note itself for links to its own headings (e.g., TOC)
    // NOTE: We check the current text from the UI since this function is called BEFORE
    // the heading is actually updated in the document (to avoid QMessageBox crashes)
    QString currentNoteText = _mainWindow->activeNoteTextEdit()->toPlainText();
    if (currentNoteText.contains(QStringLiteral("#") + oldFragment)) {
        notesWithHeadingLinks.append(_mainWindow->currentNote);
    }

    // Find all notes that have backlinks to the current note
    QVector<int> backlinkNoteIds = _mainWindow->currentNote.findBacklinkedNoteIds();

    for (int noteId : backlinkNoteIds) {
        Note backlinkNote = Note::fetch(noteId);
        QString noteText = backlinkNote.getNoteText();

        // Check if the note contains a link with the old heading fragment
        // Links can be in the form:
        // - [text](relative/path.md#old-heading)
        // - <relative/path.md#old-heading>
        // - [text](note://note-name#old-heading)
        // - <note://note-name#old-heading>

        if (noteText.contains(QStringLiteral("#") + oldFragment)) {
            notesWithHeadingLinks.append(backlinkNote);
        }
    }

    if (notesWithHeadingLinks.isEmpty()) {
        return;
    }

    // Ask the user if they want to update the backlinks
    if (Utils::Gui::question(_mainWindow, tr("Update backlinks"),
                             tr("The heading \"%1\" is referenced in %n note(s). "
                                "Do you want to update the link to use the new heading \"%2\"?",
                                "", notesWithHeadingLinks.size())
                                 .arg(oldHeading, newHeading),
                             QStringLiteral("update-heading-backlinks")) != QMessageBox::Yes) {
        return;
    }

    // User clicked "Yes"
    int updatedCount = 0;

    for (Note &backlinkNote : notesWithHeadingLinks) {
        QString noteText = backlinkNote.getNoteText();
        QString originalNoteText = noteText;

        // Build patterns to match the full link including the note path
        // We need to handle both URL-encoded and unencoded paths since links can be either
        // For example: "My Note.md" or "My%20Note.md"

        // Get both encoded and unencoded versions of the path
        QString currentNoteRelativePathEncoded =
            QString(QUrl::toPercentEncoding(currentNoteRelativePath));

        // Escape both versions for use in regex (dots, slashes, etc.)
        QString escapedRelativePath = QRegularExpression::escape(currentNoteRelativePath);
        QString escapedRelativePathEncoded =
            QRegularExpression::escape(currentNoteRelativePathEncoded);
        QString oldFragmentEscaped = QRegularExpression::escape(oldFragment);

        // Pattern 1: Markdown links with unencoded path [text](path#old-heading)
        QRegularExpression markdownLinkPattern(QStringLiteral(R"(\[([^\]]*)\]\(%1#%2\))")
                                                   .arg(escapedRelativePath, oldFragmentEscaped));
        QString markdownLinkReplacement =
            QStringLiteral(R"([\1](%1#%2))").arg(currentNoteRelativePathEncoded, newFragment);
        noteText.replace(markdownLinkPattern, markdownLinkReplacement);

        // Pattern 2: Markdown links with encoded path [text](path%20with%20spaces#old-heading)
        QRegularExpression markdownLinkPatternEncoded(
            QStringLiteral(R"(\[([^\]]*)\]\(%1#%2\))")
                .arg(escapedRelativePathEncoded, oldFragmentEscaped));
        noteText.replace(markdownLinkPatternEncoded, markdownLinkReplacement);

        // Pattern 3: Autolinks with unencoded path <path#old-heading>
        QRegularExpression autolinkPattern(
            QStringLiteral(R"(<(%1#%2)>)").arg(escapedRelativePath, oldFragmentEscaped));
        QString autolinkReplacement =
            QStringLiteral(R"(<%1#%2>)").arg(currentNoteRelativePathEncoded, newFragment);
        noteText.replace(autolinkPattern, autolinkReplacement);

        // Pattern 4: Autolinks with encoded path <path%20with%20spaces#old-heading>
        QRegularExpression autolinkPatternEncoded(
            QStringLiteral(R"(<(%1#%2)>)").arg(escapedRelativePathEncoded, oldFragmentEscaped));
        noteText.replace(autolinkPatternEncoded, autolinkReplacement);

        // Pattern 5: Bare markdown links without path (same note references)
        // For links within the same note: [text](#old-heading)
        if (backlinkNote.getId() == _mainWindow->currentNote.getId()) {
            QRegularExpression sameNoteLinkPattern(
                QStringLiteral(R"(\[([^\]]*)\]\(#%1\))").arg(oldFragmentEscaped));
            QString sameNoteLinkReplacement = QStringLiteral(R"([\1](#%1))").arg(newFragment);
            noteText.replace(sameNoteLinkPattern, sameNoteLinkReplacement);

            // Also handle autolinks within the same note: <#old-heading>
            QRegularExpression sameNoteAutolinkPattern(
                QStringLiteral(R"(<#%1>)").arg(oldFragmentEscaped));
            QString sameNoteAutolinkReplacement = QStringLiteral(R"(<#%1>)").arg(newFragment);
            noteText.replace(sameNoteAutolinkPattern, sameNoteAutolinkReplacement);
        }

        // Only update the note if changes were made
        if (noteText != originalNoteText) {
            // If this is the current note, update the text edit directly
            // (since the heading rename hasn't been applied yet in the document)
            if (backlinkNote.getId() == _mainWindow->currentNote.getId()) {
                // Update the text in the editor
                _mainWindow->activeNoteTextEdit()->setPlainText(noteText);
                // Store the changes
                _mainWindow->currentNote.storeNewText(std::move(noteText));
            } else {
                // For other notes, just store the text to disk
                backlinkNote.storeNewText(std::move(noteText));
            }
            updatedCount++;
        }
    }

    // Show a confirmation message
    if (updatedCount > 0) {
        _mainWindow->showStatusBarMessage(
            tr("Updated heading links in %n note(s)", "", updatedCount), 5000);
    }
}

/**
 * Filters navigation entries in the navigation tree widget
 */
void NavigationManager::on_navigationLineEdit_textChanged(const QString &arg1) {
    Utils::Gui::searchForTextInTreeWidget(_ui->navigationWidget, arg1,
                                          Utils::Gui::TreeWidgetSearchFlag::IntCheck);
    if (_ui->navigationTabWidget->indexOf(_ui->fileNavigationTab) >= 0) {
        Utils::Gui::searchForTextInTreeWidget(_ui->fileNavigationWidget, arg1,
                                              Utils::Gui::TreeWidgetSearchFlag::IntCheck);
    }
    if (_ui->navigationTabWidget->indexOf(_ui->backlinkTab) >= 0) {
        Utils::Gui::searchForTextInTreeWidget(_ui->backlinkWidget, arg1,
                                              Utils::Gui::TreeWidgetSearchFlag::IntCheck);
    }
}

void NavigationManager::on_navigationTabWidget_currentChanged(int index) {
    Q_UNUSED(index)

    startNavigationParser();
}

/**
 * Updates the line number label and the selected navigation item, if required
 */
void NavigationManager::noteEditCursorPositionChanged() {
    if (!_mainWindow->_noteEditLineNumberLabel->isVisible()) return;
    QOwnNotesMarkdownTextEdit *textEdit = _mainWindow->activeNoteTextEdit();
    QTextCursor cursor = textEdit->textCursor();
    QString selectedText = cursor.selectedText();

    _mainWindow->noteHistory.updateCursorPositionOfNote(_mainWindow->currentNote, textEdit);

    QString text = tr("Ln %1, Col %2", "Line / Column")
                       .arg(QString::number(cursor.block().blockNumber() + 1),
                            QString::number(cursor.positionInBlock() + 1));
    QString toolTip = tr("Line %1, Column %2")
                          .arg(QString::number(cursor.block().blockNumber() + 1),
                               QString::number(cursor.positionInBlock() + 1));

    if (!selectedText.isEmpty()) {
        const QString textAdd = QStringLiteral(" (") +
                                tr("%n selected", "Characters selected", selectedText.size()) +
                                QStringLiteral(")");
        text += textAdd;
        toolTip += textAdd;
    }

    _mainWindow->_noteEditLineNumberLabel->setText(text);
    _mainWindow->_noteEditLineNumberLabel->setToolTip(toolTip);

    const bool autoSelect =
        SettingsService().value(QStringLiteral("navigationPanelAutoSelect"), true).toBool();
    if (autoSelect) {
        selectNavigationItemAtPosition(NavigationWidget::headingPositionForCursor(cursor));
    }
}

void NavigationManager::on_actionJump_to_navigation_panel_triggered() {
    if (_ui->navigationLineEdit->isVisible()) {
        _ui->navigationLineEdit->setFocus();
    } else if (_ui->navigationWidget->isVisible()) {
        _ui->navigationWidget->setFocus();
    } else if (_ui->fileNavigationWidget->isVisible()) {
        _ui->fileNavigationWidget->setFocus();
    } else if (_ui->backlinkWidget->isVisible()) {
        _ui->backlinkWidget->setFocus();
    }
}
