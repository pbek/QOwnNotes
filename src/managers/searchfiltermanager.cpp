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

#include "searchfiltermanager.h"

#include <entities/note.h>
#include <entities/notefolder.h>
#include <entities/notesubfolder.h>
#include <entities/tag.h>
#include <services/scriptingservice.h>
#include <services/settingsservice.h>
#include <utils/gui.h>
#include <utils/misc.h>
#include <widgets/htmlpreviewwidget.h>
#include <widgets/qownnotesmarkdowntextedit.h>

#include <QCompleter>
#include <QCoreApplication>
#include <QDebug>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QRegularExpression>
#include <QTreeWidgetItem>

#include "mainwindow.h"
#include "ui_mainwindow.h"

SearchFilterManager::SearchFilterManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                                         QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui) {}

/**
 * Does the note filtering
 */
void SearchFilterManager::filterNotes(bool searchForText) {
    _ui->noteTreeWidget->scrollToTop();

    // filter the notes by text in the search line edit
    filterNotesBySearchLineEditText(searchForText);

    if (NoteFolder::isCurrentShowSubfolders() && !_mainWindow->_showNotesFromAllNoteSubFolders) {
        // filter the notes by note sub folder
        filterNotesByNoteSubFolders();
    }

    // moved condition whether to filter notes by tag at all into
    // filterNotesByTag() -- it can now be used as a slot at startup
    filterNotesByTag();

    if (searchForText) {
        // let's highlight the text from the search line edit
        searchForSearchLineTextInNoteTextEdit();

        // prevent that the last occurrence of the search term is found
        // first, instead the first occurrence should be found first
        _ui->noteTextEdit->searchWidget()->doSearchDown();
    }
}

/**
 * Does the note filtering by text in the search line edit
 */
void SearchFilterManager::filterNotesBySearchLineEditText(bool searchInNote) {
    const QString searchText = _ui->searchLineEdit->text();

    QTreeWidgetItemIterator it(_ui->noteTreeWidget);
    _ui->noteTreeWidget->setColumnCount(1);

    // search notes when at least 2 characters were entered
    if (searchText.size() >= 2) {
        if (searchInNote) {
            // open search dialog
            doSearchInNote(searchText);
        }

        const bool searchAllFolders = NoteFolder::isCurrentNoteTreeEnabled() ||
                                      _mainWindow->_showNotesFromAllNoteSubFolders ||
                                      NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively();
        QVector<int> noteIdList = Note::searchInNotes(searchText, searchAllFolders);

        int columnWidth = _ui->noteTreeWidget->columnWidth(0);
        _ui->noteTreeWidget->setColumnCount(2);
        int maxWidth = 0;
        const QStringList searchTextTerms = Note::buildQueryStringList(searchText);
        const SettingsService settings;
        const bool showMatches = settings.value(QStringLiteral("showMatches"), true).toBool();
        const auto folderMatchesSearch = [&searchText,
                                          &searchTextTerms](const QString &folderName) {
            if (folderName.contains(searchText, Qt::CaseInsensitive)) {
                return true;
            }

            for (QString word : searchTextTerms) {
                if (Note::isNameSearch(word)) {
                    word = Note::removeNameSearchPrefix(word);
                }

                word.remove(QStringLiteral("\""));
                if (!word.isEmpty() && folderName.contains(word, Qt::CaseInsensitive)) {
                    return true;
                }
            }

            return false;
        };

        while (*it) {
            QTreeWidgetItem *item = *it;

            // skip note folders (if they are also shown in the note list)
            if (item->data(0, Qt::UserRole + 1) != MainWindow::NoteType) {
                ++it;
                continue;
            }

            const int noteId = item->data(0, Qt::UserRole).toInt();
            bool isHidden = noteIdList.indexOf(noteId) < 0;

            // hide all filtered notes
            item->setHidden(isHidden);

            // count occurrences of search terms in notes
            if (!isHidden && showMatches) {
                const Note note = Note::fetch(noteId);
                item->setForeground(1, QColor(Qt::gray));
                int count = 0;

                for (QString word : searchTextTerms) {
                    if (Note::isNameSearch(word)) {
                        word = Note::removeNameSearchPrefix(word);
                    }

                    count += note.countSearchTextInNote(word);
                }

                const QString text = QString::number(count);
                item->setText(1, text);

                const QString &toolTipText =
                    searchTextTerms.count() == 1
                        ? tr("Found <strong>%n</strong> occurrence(s) of "
                             "<strong>%1</strong>",
                             "", count)
                              .arg(searchText)
                        : tr("Found <strong>%n</strong> occurrence(s) of any "
                             "term of <strong>%1</strong>",
                             "", count)
                              .arg(searchText);
                item->setToolTip(1, toolTipText);

                // calculate the size of the search count column
                QFontMetrics fm(item->font(1));

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
                maxWidth = std::max(maxWidth, fm.width(text));
#else
                maxWidth = std::max(maxWidth, fm.horizontalAdvance(text));
#endif
            }

            ++it;
        }

        if (NoteFolder::isCurrentNoteTreeEnabled()) {
            QVector<QTreeWidgetItem *> folderItems;
            QTreeWidgetItemIterator folderIt(_ui->noteTreeWidget);

            while (*folderIt) {
                if ((*folderIt)->data(0, Qt::UserRole + 1) == MainWindow::FolderType) {
                    folderItems << *folderIt;
                }

                ++folderIt;
            }

            for (int i = folderItems.count() - 1; i >= 0; --i) {
                auto *folderItem = folderItems.at(i);
                bool hasVisibleChildren = false;

                for (int childIndex = 0; childIndex < folderItem->childCount(); ++childIndex) {
                    if (!folderItem->child(childIndex)->isHidden()) {
                        hasVisibleChildren = true;
                        break;
                    }
                }

                const bool shouldShow =
                    hasVisibleChildren || folderMatchesSearch(folderItem->text(0));
                folderItem->setHidden(!shouldShow);

                if (shouldShow) {
                    folderItem->setExpanded(true);
                }
            }
        }

        // resize the column 0, so we can see the search counts
        columnWidth = std::max(10, columnWidth - maxWidth - 5);
        //        _ui->noteTreeWidget->resizeColumnToContents(1);
        _ui->noteTreeWidget->setColumnWidth(0, columnWidth);
        _ui->noteTreeWidget->setColumnWidth(1, maxWidth);
    } else {
        // otherwise show all items
        while (*it) {
            (*it)->setHidden(false);
            ++it;
        }
    }
}

/**
 * Opens the search widget in the current note and searches for all
 * occurrences of the words in the search text
 *
 * @param searchText
 */
void SearchFilterManager::doSearchInNote(QString searchText) {
    const QStringList searchTextTerms = Note::buildQueryStringList(searchText, true, true);

    if (searchTextTerms.count() > 1) {
        QString localSearchTerm =
            QStringLiteral("(") + searchTextTerms.join(QStringLiteral("|")) + QStringLiteral(")");
        _mainWindow->activeNoteTextEdit()->doSearch(
            localSearchTerm, QPlainTextEditSearchWidget::RegularExpressionMode);
    } else {
        if (Note::isNameSearch(searchText)) {
            searchText = Note::removeNameSearchPrefix(searchText);
        }

        _mainWindow->activeNoteTextEdit()->doSearch(searchText.remove(QStringLiteral("\"")));
    }
}

/**
 * Does the note filtering by tags
 */
void SearchFilterManager::filterNotesByTag() {
    if (!_mainWindow->isTagsEnabled()) {
        return;    // do nothing
    }

    const int tagId = Tag::activeTagId();
    QVector<int> noteIdList;

    switch (tagId) {
        case Tag::AllNotesId:
            // don't do any additional filtering here
            return;
        case Tag::AllUntaggedNotesId:
            // get all note names that are not tagged
            noteIdList = Note::fetchAllNotTaggedIds();
            break;
        default:
            // check for multiple active;
            const auto selectedItems = _ui->tagTreeWidget->selectedItems();
            QVector<int> tagIds;
            Tag activeTag;

            if (selectedItems.count() > 1) {
                tagIds.reserve(selectedItems.count());
                for (auto *i : selectedItems) {
                    const int id = i->data(0, Qt::UserRole).toInt();
                    tagIds << id;
                }
            } else {
                // check if there is an active tag
                activeTag = Tag::activeTag();
                if (!activeTag.isFetched()) {
                    return;
                }
                tagIds << activeTag.getId();
            }

            QVector<int> tagIdList;
            if (Tag::isTaggingShowNotesRecursively()) {
                tagIdList.reserve(tagIds.count());
                for (const int tId : Utils::asConst(tagIds)) {
                    tagIdList << Tag::fetchTagIdsRecursivelyByParentId(tId);
                }
            } else {
                tagIdList = std::move(tagIds);
            }

            qDebug() << __func__ << " - 'tags': " << tagIds;

            const auto selectedFolderItems = _ui->noteSubFolderTreeWidget->selectedItems();

            const bool showNotesFromAllNoteSubFolders =
                _mainWindow->_showNotesFromAllNoteSubFolders;
            noteIdList.reserve(tagIdList.count() * 2);
            if (selectedFolderItems.count() > 1) {
                for (const int tagId_ : Utils::asConst(tagIdList)) {
                    for (const QTreeWidgetItem *i : selectedFolderItems) {
                        const int id = i->data(0, Qt::UserRole).toInt();
                        const NoteSubFolder folder = NoteSubFolder::fetch(id);

                        noteIdList << Tag::fetchAllLinkedNoteIdsForFolder(
                            tagId_, folder, showNotesFromAllNoteSubFolders);
                    }
                }
            } else {
                for (const int tagId_ : Utils::asConst(tagIdList)) {
                    noteIdList << Tag::fetchAllLinkedNoteIds(tagId_,
                                                             showNotesFromAllNoteSubFolders);
                }
            }
            break;
    }

    qDebug() << __func__ << " - 'noteIdList': " << noteIdList;

    // omit the already hidden notes
    QTreeWidgetItemIterator it(_ui->noteTreeWidget, QTreeWidgetItemIterator::NotHidden);

    // loop through all visible notes
    while (*it) {
        if ((*it)->data(0, Qt::UserRole + 1) != MainWindow::NoteType) {
            ++it;
            continue;
        }

        // hide all notes that are not linked to the active tag
        // note subfolder are not taken into account here (note names are now
        // not unique), but it should be ok because they are filtered by
        // filterNotesByNoteSubFolders
        if (!noteIdList.contains((*it)->data(0, Qt::UserRole).toInt())) {
            (*it)->setHidden(true);
        }

        ++it;
    }
}

/**
 * Does the note filtering by note sub folders
 */
void SearchFilterManager::filterNotesByNoteSubFolders() {
    const auto selectedItems = _ui->noteSubFolderTreeWidget->selectedItems();

    // get all the folder ids
    QVector<int> selectedNoteSubFolderIds;
    selectedNoteSubFolderIds.reserve(selectedItems.count());
    if (selectedItems.count() > 1) {
        for (QTreeWidgetItem *i : selectedItems) {
            selectedNoteSubFolderIds << i->data(0, Qt::UserRole).toInt();
        }
    } else {
        selectedNoteSubFolderIds << NoteSubFolder::activeNoteSubFolderId();
    }

    QVector<int> noteSubFolderIds;
    noteSubFolderIds.reserve(selectedNoteSubFolderIds.count());
    // check if the notes should be viewed recursively
    if (NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively()) {
        for (int subFolId : Utils::asConst(selectedNoteSubFolderIds)) {
            noteSubFolderIds << NoteSubFolder::fetchIdsRecursivelyByParentId(subFolId);
        }
    } else {
        noteSubFolderIds << selectedNoteSubFolderIds;
    }

    qDebug() << __func__ << " - 'noteSubFolderIds': " << noteSubFolderIds;

    // get the notes from the subfolders
    QVector<int> noteIdList;
    noteIdList.reserve(noteSubFolderIds.count());
    for (int noteSubFolderId : Utils::asConst(noteSubFolderIds)) {
        // get all notes of a note sub folder
        noteIdList << Note::fetchAllIdsByNoteSubFolderId(noteSubFolderId);
    }

    // omit the already hidden notes
    QTreeWidgetItemIterator it(_ui->noteTreeWidget, QTreeWidgetItemIterator::NotHidden);

    // loop through all visible notes
    while (*it) {
        // hide all notes that are not in the note sub folder
        if (!noteIdList.contains((*it)->data(0, Qt::UserRole).toInt())) {
            (*it)->setHidden(true);
        }
        ++it;
    }
}

/**
 * Highlights all occurrences of str in the note text edit and does a "in note
 * search"
 */
void SearchFilterManager::searchInNoteTextEdit(QString str) {
    QList<QTextEdit::ExtraSelection> extraSelections;
    QList<QTextEdit::ExtraSelection> extraSelections2;
    QList<QTextEdit::ExtraSelection> extraSelections3;

    if (str.size() >= 2) {
        // do an in-note search
        doSearchInNote(str);
        _ui->noteTextEdit->moveCursor(QTextCursor::Start);
#ifndef USE_QLITEHTML
        _ui->noteTextView->moveCursor(QTextCursor::Start);
#endif
        _ui->encryptedNoteTextEdit->moveCursor(QTextCursor::Start);
        const QColor color = QColor(0, 180, 0, 100);

        // build the string list of the search string
        const QString queryStr = str.replace(QLatin1String("|"), QLatin1String("\\|"));
        const QStringList queryStrings = Note::buildQueryStringList(queryStr, true);

        if (queryStrings.count() > 0) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
            const QRegularExpression regExp(
                QLatin1Char('(') + queryStrings.join(QLatin1String("|")) + QLatin1Char(')'),
                QRegularExpression::CaseInsensitiveOption);
#else
            const QRegExp regExp(
                QLatin1String("(") + queryStrings.join(QLatin1String("|")) + QLatin1String(")"),
                Qt::CaseInsensitive);
#endif
            while (_ui->noteTextEdit->find(regExp)) {
                QTextEdit::ExtraSelection extra = QTextEdit::ExtraSelection();
                extra.format.setBackground(color);

                extra.cursor = _ui->noteTextEdit->textCursor();
                extraSelections.append(extra);
            }

            // TODO:
#ifdef USE_QLITEHTML
            _mainWindow->_notePreviewWidget->findText(str, QTextDocument::FindFlag::FindWholeWords,
                                                      true);
#else
            while (_ui->noteTextView->find(regExp)) {
                QTextEdit::ExtraSelection extra = QTextEdit::ExtraSelection();
                extra.format.setBackground(color);

                extra.cursor = _ui->noteTextView->textCursor();
                extraSelections2.append(extra);
            }
#endif
            while (_ui->encryptedNoteTextEdit->find(regExp)) {
                QTextEdit::ExtraSelection extra = QTextEdit::ExtraSelection();
                extra.format.setBackground(color);

                extra.cursor = _ui->encryptedNoteTextEdit->textCursor();
                extraSelections3.append(extra);
            }
        }
    }

    _ui->noteTextEdit->setExtraSelections(extraSelections);
#ifndef USE_QLITEHTML
    _ui->noteTextView->setExtraSelections(extraSelections2);
#endif
    _ui->encryptedNoteTextEdit->setExtraSelections(extraSelections3);
}

/**
 * highlights all occurrences of the search line text in the note text edit
 */
void SearchFilterManager::searchForSearchLineTextInNoteTextEdit() {
    QString searchString = _ui->searchLineEdit->text();

    if (searchString.isEmpty()) {
        _mainWindow->activeNoteTextEdit()->searchWidget()->close();
    } else {
        searchInNoteTextEdit(std::move(searchString));
    }
}

/**
 * Jumps to found note or create a new one if not found
 */
void SearchFilterManager::jumpToNoteOrCreateNew(bool disableLoadNoteDirectoryList) {
    // ignore if `return` was pressed in the completer
    if (_mainWindow->_searchLineEditFromCompleter) {
        _mainWindow->_searchLineEditFromCompleter = false;
        return;
    }

    const QString text = _ui->searchLineEdit->text().trimmed();

    // prevent creation of broken note text files
    if (text.isEmpty()) {
        return;
    }

    // this doesn't seem to work with note sub folders
    const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
    Q_UNUSED(blocker)

    // add the current search text to the saved searches
    storeSavedSearch();

    // clear search line edit so all notes will be viewed again and to prevent
    // a brief appearing of the note search widget when creating a new note
    // with action_New_note
    _ui->searchLineEdit->clear();

    // first let us search for the entered text
    Note note = Note::fetchByName(text);

    // if we can't find a note we create a new one
    if (note.getId() == 0) {
        // Allow note editing if it was disabled
        _mainWindow->allowNoteEditing();

        // check if a hook wants to set the text
        QString noteText = ScriptingService::instance()->callHandleNewNoteHeadlineHook(text);

        // check if a hook changed the text
        if (noteText.isEmpty()) {
            // fallback to the old text if no hook changed the text
            noteText = Note::createNoteHeader(text);
        } else {
            noteText.append(QLatin1String("\n\n"));
        }

        const NoteSubFolder noteSubFolder = NoteSubFolder::activeNoteSubFolder();
        const QString noteSubFolderPath = noteSubFolder.fullPath();

        note = Note();
        note.setName(text);
        note.setNoteText(noteText);
        note.setNoteSubFolderId(noteSubFolder.getId());
        note.store();

        // workaround when signal block doesn't work correctly
        _mainWindow->directoryWatcherWorkaround(true);

        // we even need a 2nd workaround because something triggers that the
        // note folder was modified
        _mainWindow->noteDirectoryWatcher.removePath(_mainWindow->notesPath);
        _mainWindow->noteDirectoryWatcher.removePath(noteSubFolderPath);

        // store the note to disk
        // if a tag is selected add the tag to the just created note
        const Tag tag = Tag::activeTag();
        if (tag.isFetched()) {
            tag.linkToNote(note);
        }

        const bool noteWasStored = note.storeNoteTextFileToDisk();
        _mainWindow->showStatusBarMessage(
            noteWasStored ? tr("Stored current note to disk")
                          : tr("Current note could not be stored to disk"),
            noteWasStored ? QStringLiteral("💾") : QStringLiteral("❌"), 3000);

        // Check if a name was set in a script
        // We need to do that in the end or the changed name will leak into the note filename
        const QString hookName = ScriptingService::instance()->callHandleNoteNameHook(&note);

        if (!hookName.isEmpty()) {
            note.setName(hookName);
            note.store();
        }

        {
            const QSignalBlocker blocker2(_ui->noteTreeWidget);
            Q_UNUSED(blocker2)

            // adds the note to the note tree widget
            _mainWindow->addNoteToNoteTreeWidget(note);
        }

        //        buildNotesIndex();
        if (!disableLoadNoteDirectoryList) {
            _mainWindow->loadNoteDirectoryList();
        }

        // fetch note new (because all the IDs have changed after
        // the buildNotesIndex()
        //        note.refetch();

        // add the file to the note directory watcher
        _mainWindow->noteDirectoryWatcherAddPath(note.fullNoteFilePath());

        // add the paths from the workaround
        _mainWindow->noteDirectoryWatcherAddPath(_mainWindow->notesPath);
        _mainWindow->noteDirectoryWatcherAddPath(noteSubFolderPath);

        // turn on the method again
        _mainWindow->directoryWatcherWorkaround(false);
    }

    // jump to the found or created note
    _mainWindow->setCurrentNote(std::move(note));

    // hide the search widget after creating a new note
    _mainWindow->activeNoteTextEdit()->hideSearchWidget(true);

    // focus the note text edit and set the cursor correctly
    _mainWindow->focusNoteTextEdit();
}

/**
 * Adds the current search text to the saved searches
 */
void SearchFilterManager::storeSavedSearch() {
    SettingsService settings;

    if (settings.value(QStringLiteral("disableSavedSearchesAutoCompletion")).toBool()) {
        return;
    }

    const QString text = _ui->searchLineEdit->text();
    // Only store searches with less than 30 characters to prevent clogging the settings
    if (!text.isEmpty() && text.length() < 30) {
        int noteFolderId = NoteFolder::currentNoteFolderId();
        QString settingsKey =
            QStringLiteral("savedSearches/noteFolder-") + QString::number(noteFolderId);
        QStringList savedSearches = settings.value(settingsKey).toStringList();

        // add the text to the saved searches
        savedSearches.prepend(text);

        // remove duplicate entries, `text` will remain at the top
        savedSearches.removeDuplicates();

        // only keep 100 searches
        while (savedSearches.count() > 100) {
            savedSearches.removeLast();
        }

        settings.setValue(settingsKey, savedSearches);

        // init the saved searches completer
        initSavedSearchesCompleter();
    }
}

/**
 * Initializes the saved searches completer
 */
void SearchFilterManager::initSavedSearchesCompleter() {
    const int noteFolderId = NoteFolder::currentNoteFolderId();
    QStringList savedSearches;
    SettingsService settings;

    if (!settings.value(QStringLiteral("disableSavedSearchesAutoCompletion")).toBool()) {
        QString settingsKey =
            QStringLiteral("savedSearches/noteFolder-") + QString::number(noteFolderId);
        savedSearches = settings.value(settingsKey).toStringList();
    }

    // release the old completer
    auto *completer = _ui->searchLineEdit->completer();
    delete completer;

    // add the completer
    completer = new QCompleter(savedSearches, _ui->searchLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    _ui->searchLineEdit->setCompleter(completer);

    // install event filter for the popup
    completer->popup()->installEventFilter(_mainWindow);
}

/**
 * Jumps to "All notes" in the note subfolder and tag tree widget and triggers
 * a "Find note"
 */
void SearchFilterManager::on_actionFind_notes_in_all_subfolders_triggered() {
    // send an event to jump to "All notes" in the note subfolder tree widget
    _mainWindow->selectAllNotesInNoteSubFolderTreeWidget();

    // send an event to jump to "All notes" in the tag tree widget
    _mainWindow->selectAllNotesInTagTreeWidget();

    // trigger a "Find note"
    on_action_Find_note_triggered();
}

/**
 * Opens the widget to replace text in the current note
 */
void SearchFilterManager::on_actionReplace_in_current_note_triggered() {
    QOwnNotesMarkdownTextEdit *textEdit = _mainWindow->activeNoteTextEdit();
    textEdit->searchWidget()->activateReplace();
}

//
// set focus on search line edit if Ctrl + Shift + F was pressed
//
void SearchFilterManager::on_action_Find_note_triggered() {
    if (!Utils::Gui::enableDockWidgetQuestion(_mainWindow->_noteSearchDockWidget)) {
        return;
    }

    // Search for the selected text if there is any
    const auto selectedText = _mainWindow->activeNoteTextEdit()->textCursor().selectedText();
    if (!selectedText.isEmpty()) {
        _ui->searchLineEdit->setText(selectedText);
    }

    _mainWindow->changeDistractionFreeMode(false);
    _ui->searchLineEdit->setFocus();
    _ui->searchLineEdit->selectAll();
}

void SearchFilterManager::on_searchLineEdit_textChanged(const QString &arg1) {
    Q_UNUSED(arg1)
    filterNotes();
}

//
// jump to found note or create a new one if not found
//
void SearchFilterManager::on_searchLineEdit_returnPressed() { jumpToNoteOrCreateNew(); }

void SearchFilterManager::on_action_Find_text_in_note_triggered() {
    QOwnNotesMarkdownTextEdit *textEdit = _mainWindow->activeNoteTextEdit();
    textEdit->searchWidget()->activate();
}
