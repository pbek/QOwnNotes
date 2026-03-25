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

#include "tagmanager.h"

#include <dialogs/tagadddialog.h>
#include <entities/note.h>
#include <entities/notefolder.h>
#include <entities/notesubfolder.h>
#include <entities/tag.h>
#include <services/scriptingservice.h>
#include <services/settingsservice.h>
#include <utils/gui.h>
#include <utils/misc.h>
#include <widgets/htmlpreviewwidget.h>
#include <widgets/notesubfoldertree.h>

#include <QColorDialog>
#include <QCompleter>
#include <QDockWidget>
#include <QMenu>
#include <QPushButton>
#include <QSpacerItem>
#include <QTreeWidgetItem>

#include "mainwindow.h"
#include "ui_mainwindow.h"

TagManager::TagManager(MainWindow *mainWindow, Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui) {}

/**
 * Reloads the tag tree
 */
void TagManager::reloadTagTree() {
    // take care that the tags are synced from the notes to the internal db
    handleScriptingNotesTagUpdating();

    SettingsService settings;

    // remove all broken note tag links
    if (!_mainWindow->_brokenTagNoteLinksRemoved) {
        Tag::removeBrokenLinks();
        _mainWindow->_brokenTagNoteLinksRemoved = true;
    }

    _ui->tagTreeWidget->clear();

    QVector<int> noteSubFolderIds;

    auto noteSubFolderWidgetItems = _ui->noteSubFolderTreeWidget->selectedItems();
    // if only one item is selected, then take current Item otherwise we will get
    // the item that was selected previously
    if (noteSubFolderWidgetItems.count() == 1) {
        auto *currentItem = _ui->noteSubFolderTreeWidget->currentItem();
        if (currentItem != nullptr) {
            noteSubFolderWidgetItems[0] = currentItem;
        }
    }

    noteSubFolderIds.reserve(noteSubFolderWidgetItems.count());
    // check if the notes should be viewed recursively
    if (NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively()) {
        for (QTreeWidgetItem *i : noteSubFolderWidgetItems) {
            if (i == nullptr) {
                continue;
            }
            const int id = i->data(0, Qt::UserRole).toInt();
            noteSubFolderIds << NoteSubFolder::fetchIdsRecursivelyByParentId(id);
        }
    } else {
        for (QTreeWidgetItem *i : noteSubFolderWidgetItems) {
            if (i == nullptr) {
                continue;
            }
            const int id = i->data(0, Qt::UserRole).toInt();
            noteSubFolderIds << id;
        }
    }

    QVector<int> noteIdList;
    int untaggedNoteCount = 0;

    if (NoteFolder::isCurrentShowSubfolders()) {
        // get the notes from the subfolders
        for (int noteSubFolderId : Utils::asConst(noteSubFolderIds)) {
            // get all notes of a note sub folder
            untaggedNoteCount += Note::countAllNotTagged(noteSubFolderId);
            noteIdList << Note::fetchAllIdsByNoteSubFolderId(noteSubFolderId);
        }
    } else {
        untaggedNoteCount = Note::countAllNotTagged(0);
    }

    // create an item to view all notes
    int linkCount =
        _mainWindow->_showNotesFromAllNoteSubFolders || !NoteFolder::isCurrentShowSubfolders()
            ? Note::countAll()
            : noteIdList.count();
    QString toolTip = tr("Show all notes (%1)").arg(QString::number(linkCount));

    auto *allItem = new QTreeWidgetItem();
    allItem->setText(0, tr("All notes"));
    allItem->setForeground(1, QColor(Qt::gray));
    allItem->setText(1, QString::number(linkCount));
    allItem->setToolTip(0, toolTip);
    allItem->setToolTip(1, toolTip);
    allItem->setData(0, Qt::UserRole, Tag::AllNotesId);
    allItem->setFlags(allItem->flags() & ~Qt::ItemIsSelectable);
    allItem->setIcon(
        0, QIcon::fromTheme(QStringLiteral("edit-copy"),
                            QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/edit-copy.svg"))));

    // this time, the tags come first
    buildTagTreeForParentItem();
    // and get sorted
    if (settings.value(QStringLiteral("tagsPanelSort")).toInt() == SORT_ALPHABETICAL) {
        _ui->tagTreeWidget->sortItems(
            0, Utils::Gui::toQtOrder(settings.value(QStringLiteral("tagsPanelOrder")).toInt()));
    }
    // now add 'All notes' to the top
    _ui->tagTreeWidget->insertTopLevelItem(0, allItem);

    // add an item to view untagged notes if there are any
    linkCount = _mainWindow->_showNotesFromAllNoteSubFolders ? Note::countAllNotTagged()
                                                             : untaggedNoteCount;

    if (linkCount > 0) {
        toolTip = tr("show all untagged notes (%1)").arg(QString::number(linkCount));
        auto *untaggedItem = new QTreeWidgetItem();
        untaggedItem->setText(0, tr("Untagged notes"));
        untaggedItem->setForeground(1, QColor(Qt::gray));
        untaggedItem->setText(1, QString::number(linkCount));
        untaggedItem->setToolTip(0, toolTip);
        untaggedItem->setToolTip(1, toolTip);
        untaggedItem->setData(0, Qt::UserRole, Tag::AllUntaggedNotesId);
        untaggedItem->setFlags(untaggedItem->flags() & ~Qt::ItemIsSelectable);
        untaggedItem->setIcon(
            0,
            QIcon::fromTheme(QStringLiteral("edit-copy"),
                             QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/edit-copy.svg"))));
        _ui->tagTreeWidget->addTopLevelItem(untaggedItem);
    }

    // Decorate root if there are multiple levels to be able to collapse them,
    // because double-clicking will not collapse the first level, but edit
    // the clicked tag
    _ui->tagTreeWidget->setRootIsDecorated(Tag::countAllParentId(0) != Tag::countAll());

    highlightCurrentNoteTagsInTagTree();
}

/**
 * Populates the tag tree recursively with its tags
 */
void TagManager::buildTagTreeForParentItem(QTreeWidgetItem *parent, bool topLevel) {
    const int parentId =
        (parent == nullptr || topLevel) ? 0 : parent->data(0, Qt::UserRole).toInt();
    const int activeTagId = Tag::activeTagId();
    SettingsService settings;
    const QStringList expandedList =
        settings
            .value(QStringLiteral("MainWindow/tagTreeWidgetExpandState-") +
                   QString::number(NoteFolder::currentNoteFolderId()))
            .toStringList();
    const int tagPanelSort = settings.value(QStringLiteral("tagsPanelSort")).toInt();
    const int tagPanelOrder = settings.value(QStringLiteral("tagsPanelOrder")).toInt();
    const QVector<TagHeader> tagList = Tag::fetchAllTagHeadersByParentId(parentId);
    for (const TagHeader &tag : tagList) {
        const int tagId = tag._id;
        QTreeWidgetItem *item = addTagToTagTreeWidget(parent, tag);

        // set the active item
        if (activeTagId == tagId) {
            const QSignalBlocker blocker(_ui->tagTreeWidget);
            Q_UNUSED(blocker)

            _ui->tagTreeWidget->setCurrentItem(item);
        }

        // recursively populate the next level
        buildTagTreeForParentItem(item);

        // set expanded state
        item->setExpanded(expandedList.contains(QString::number(tagId)));

        if (tagPanelSort == SORT_ALPHABETICAL) {
            item->sortChildren(0, Utils::Gui::toQtOrder(tagPanelOrder));
        }
    }

    // update the UI
    // this will crash the app sporadically
    // QCoreApplication::processEvents();
}

/**
 * Ads a tag to the tag tree widget
 */
QTreeWidgetItem *TagManager::addTagToTagTreeWidget(QTreeWidgetItem *parent, const TagHeader &tag) {
    const int parentId = parent == nullptr ? 0 : parent->data(0, Qt::UserRole).toInt();
    const int tagId = tag._id;
    const QString name = tag._name;
    auto hideCount = SettingsService().value("tagsPanelHideNoteCount", false).toBool();

    int linkCount = 0;
    QVector<int> linkedNoteIds;
    bool isMultipleTags = false;

    if (!hideCount) {
        const QVector<int> tagIdListToCount = Tag::isTaggingShowNotesRecursively()
                                                  ? Tag::fetchTagIdsRecursivelyByParentId(tagId)
                                                  : QVector<int>{tag._id};
        isMultipleTags = tagIdListToCount.count() > 1;
        const auto selectedSubFolderItems = _ui->noteSubFolderTreeWidget->selectedItems();
        const bool showNotesFromAllSubFolders = _mainWindow->_showNotesFromAllNoteSubFolders;
        const bool isShowNotesRecursively =
            NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively();

        if (selectedSubFolderItems.count() > 1) {
            linkedNoteIds.reserve(tagIdListToCount.size());

            for (const int tagIdToCount : tagIdListToCount) {
                for (QTreeWidgetItem *folderItem : selectedSubFolderItems) {
                    int id = folderItem->data(0, Qt::UserRole).toInt();
                    const NoteSubFolder folder = NoteSubFolder::fetch(id);

                    if (!folder.isFetched()) {
                        continue;
                    }

                    if (!isMultipleTags) {
                        linkCount = Tag::countLinkedNoteFileNamesForNoteSubFolder(
                            tagIdToCount, folder, showNotesFromAllSubFolders,
                            isShowNotesRecursively);
                    } else {
                        linkedNoteIds << Tag::fetchAllLinkedNoteIdsForFolder(
                            tagIdToCount, folder, showNotesFromAllSubFolders,
                            isShowNotesRecursively);
                    }
                }
            }
        } else {
            for (const int tagToCount : tagIdListToCount) {
                if (!isMultipleTags) {
                    linkCount = Tag::countLinkedNoteFileNames(
                        tagToCount, showNotesFromAllSubFolders, isShowNotesRecursively);
                } else {
                    linkedNoteIds << Tag::fetchAllLinkedNoteIds(
                        tagToCount, showNotesFromAllSubFolders, isShowNotesRecursively);
                }
            }
        }
    }

    if (isMultipleTags) {
        // remove duplicate note ids
        QVector<int> uniqueLinkedNoteIds;
        for (const int &value : linkedNoteIds) {
            if (!uniqueLinkedNoteIds.contains(value)) {
                uniqueLinkedNoteIds.append(value);
            }
        }

        linkCount = uniqueLinkedNoteIds.count();
    }

    QString toolTip =
        tr("Show all notes tagged with '%1' (%2)").arg(name, QString::number(linkCount));

#ifdef QT_DEBUG
    toolTip += QStringLiteral("<br />id: %1").arg(tag._id);
#endif

    auto *item = new QTreeWidgetItem();
    item->setData(0, Qt::UserRole, tagId);
    item->setText(0, name);
    item->setText(1, linkCount > 0 ? QString::number(linkCount) : QString());
    item->setForeground(1, QColor(Qt::gray));
    item->setIcon(0, Utils::Gui::tagIcon());
    item->setToolTip(0, toolTip);
    item->setToolTip(1, toolTip);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    // set the color of the tag tree widget item
    Utils::Gui::handleTreeWidgetItemTagColor(item, tagId);

    if (parentId == 0) {
        // add the item at top level if there was no parent item
        _ui->tagTreeWidget->addTopLevelItem(item);
    } else {
        // add the item as child of the parent
        parent->addChild(item);
    }

    return item;
}

/**
 * Creates a new tag
 */
void TagManager::on_tagLineEdit_returnPressed() {
    const QString name = _ui->tagLineEdit->text();
    if (name.isEmpty()) {
        return;
    }

    const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
    Q_UNUSED(blocker)

    Tag tag;
    tag.setName(name);

    if (tag.store()) {
        const QSignalBlocker blocker2(_ui->tagLineEdit);
        Q_UNUSED(blocker2)

        // clear the line edit if the tag was stored
        _ui->tagLineEdit->clear();
    }

    reloadTagTree();
}

/**
 * Filters tags in the tag tree widget
 */
void TagManager::on_tagLineEdit_textChanged(const QString &arg1) {
    Utils::Gui::searchForTextInTreeWidget(_ui->tagTreeWidget, arg1,
                                          Utils::Gui::TreeWidgetSearchFlag::IntCheck);
}

/**
 * Shows or hides everything for the note tags
 */
void TagManager::setupTags() {
    _ui->newNoteTagLineEdit->setVisible(false);
    _ui->newNoteTagButton->setVisible(true);

#ifdef Q_OS_MAC
    // try to compensate for the different button top margins in OS X
    _ui->noteTagFrame->layout()->setContentsMargins(0, 0, 0, 0);
    _ui->noteTagButtonFrame->layout()->setContentsMargins(0, 8, 0, 0);
#else
    // we want the tag frame as small as possible
    _ui->noteTagFrame->layout()->setContentsMargins(8, 0, 8, 0);
#endif

    reloadTagTree();
    reloadCurrentNoteTags();
    // filter the notes again
    _mainWindow->filterNotes(false);
}

/**
 * Hides the note tag add button and shows the text edit
 */
void TagManager::on_newNoteTagButton_clicked() {
    _mainWindow->_noteTagDockWidget->setVisible(true);
    _ui->newNoteTagLineEdit->setVisible(true);
    _ui->newNoteTagLineEdit->setFocus();
    _ui->newNoteTagLineEdit->selectAll();
    _ui->newNoteTagButton->setVisible(false);

    SettingsService settings;
    // enable the tagging dock widget the first time tagging was used
    if (!settings.value(QStringLiteral("tagWasAddedToNote")).toBool()) {
        _mainWindow->_taggingDockWidget->setVisible(true);
        settings.setValue(QStringLiteral("tagWasAddedToNote"), true);
    }

    // add tag name auto-completion
    const QStringList wordList = Tag::fetchAllNames();
    auto *completer = new QCompleter(wordList, _mainWindow);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    _ui->newNoteTagLineEdit->setCompleter(completer);
    completer->popup()->installEventFilter(_mainWindow);
}

/**
 * Links a note to the tag entered after pressing return
 * in the note tag line edit
 */
void TagManager::on_newNoteTagLineEdit_returnPressed() {
    const QString text = _ui->newNoteTagLineEdit->text();
    linkTagNameToCurrentNote(text, true);
}

/**
 * Links a tag to the current note (or all selected notes)
 *
 * @param tagName
 */
void TagManager::linkTagNameToCurrentNote(const QString &tagName, bool linkToSelectedNotes) {
    if (tagName.isEmpty()) {
        return;
    }

    // workaround when signal block doesn't work correctly
    _mainWindow->directoryWatcherWorkaround(true, true);

    // create a new tag if it doesn't exist
    Tag tag = Tag::fetchByName(tagName);
    if (!tag.isFetched()) {
        const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        tag.setName(tagName);
        tag.store();
    }

    // link the current note to the tag
    if (tag.isFetched()) {
        const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        const int selectedNotesCount = _mainWindow->getSelectedNotesCount();

        if (linkToSelectedNotes && selectedNotesCount > 1) {
            const auto noteList = _mainWindow->selectedNotes();
            for (const Note &note : noteList) {
                if (tag.isLinkedToNote(note)) {
                    continue;
                }

                tag.linkToNote(note);

                // add the tag to the note text if defined via scripting
                // engine
                handleScriptingNoteTagging(note, tag, false, false);
            }
        } else {
            tag.linkToNote(_mainWindow->currentNote);

            // add the tag to the note text if defined via scripting engine
            handleScriptingNoteTagging(_mainWindow->currentNote, tag, false, false);
        }

        reloadCurrentNoteTags();
        reloadTagTree();
        _mainWindow->filterNotes();

        // handle the coloring of the note in the note tree widget
        handleNoteTreeTagColoringForNote(_mainWindow->currentNote);
    }

    // turn off the workaround again
    _mainWindow->directoryWatcherWorkaround(false, true);
}

/**
 * Adds or removes a tag from the note text if defined via scripting engine
 *
 * @param note
 * @param tagName
 * @param doRemove
 * @param triggerPostMethods
 */
void TagManager::handleScriptingNoteTagging(Note note, const Tag &tag, bool doRemove,
                                            bool triggerPostMethods) {
    const QString oldNoteText = note.getNoteText();
    const QString &action = doRemove ? QStringLiteral("remove") : QStringLiteral("add");
    QString noteText =
        ScriptingService::instance()->callNoteTaggingHook(note, action, tag.getName()).toString();

    // try noteTaggingByObjectHook if noteTaggingHook didn't do anything
    if (noteText.isEmpty()) {
        noteText =
            ScriptingService::instance()->callNoteTaggingByObjectHook(note, action, tag).toString();

        if (noteText.isEmpty() || (oldNoteText == noteText)) {
            return;
        }
    }

    // return if note could not be stored
    if (!note.storeNewText(std::move(noteText))) {
        return;
    }

    // do some stuff to get the UI updated
    if (triggerPostMethods) {
        const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        _mainWindow->storeUpdatedNotesToDisk();
        reloadTagTree();
        //        reloadCurrentNoteTags();
    }

    if (note.isSameFile(_mainWindow->currentNote)) {
        //            updateNoteTextFromDisk(note);

        _mainWindow->currentNote.refetch();
        _mainWindow->setNoteTextFromNote(&_mainWindow->currentNote);
    }
}

/**
 * Takes care that the tags are synced from the notes to the internal db
 */
void TagManager::handleScriptingNotesTagUpdating() {
    if (!ScriptingService::instance()->noteTaggingHookExists()) {
        return;
    }

    qDebug() << __func__;

    // workaround when signal blocking doesn't work correctly
    _mainWindow->directoryWatcherWorkaround(true, true);

    const QVector<Note> &notes = Note::fetchAll();
    for (const Note &note : notes) {
        QSet<int> tagIdList;
        const QStringList tagNameList = ScriptingService::instance()
                                            ->callNoteTaggingHook(note, QStringLiteral("list"))
                                            .toStringList();

        if (tagNameList.count() == 0) {
            // if callNoteTaggingHook didn't return anything lets try
            // callNoteTaggingByObjectHook
            const auto variantTagIdList =
                ScriptingService::instance()
                    ->callNoteTaggingByObjectHook(note, QStringLiteral("list"))
                    .toList();

            // get a tagId list from the variant list
            for (const QVariant &tagId : variantTagIdList) {
                tagIdList << tagId.toInt();
            }
        } else {
            // get a tagId list from the tag name list
            for (const QString &tagName : tagNameList) {
                Tag tag = Tag::fetchByName(tagName);

                // add missing tags to the tag database
                if (!tag.isFetched()) {
                    tag.setName(tagName);
                    tag.store();
                }

                tagIdList << tag.getId();
            }
        }

        QSet<int> tagIdList2 = Tag::fetchAllIdsByNote(note);

        // we need to create a copy of tagIdList, because subtract would modify tagIdList
        QSet<int> subtraction = tagIdList;
        subtraction.subtract(tagIdList2);

        // add missing tag links to the note
        for (const int tagId : subtraction) {
            Tag tag = Tag::fetch(tagId);
            tag.linkToNote(note);
            qDebug() << " difference1: " << tag;
        }

        const QSet<int> subtraction1 = tagIdList2.subtract(tagIdList);

        // remove tags from the note that are not in the note text
        for (const int tagId : subtraction1) {
            Tag tag = Tag::fetch(tagId);
            tag.removeLinkToNote(note);
            qDebug() << " difference2: " << tag;
        }
    }

    // disable workaround
    _mainWindow->directoryWatcherWorkaround(false, true);
}

/**
 * Takes care that a tag is renamed in all notes
 *
 * @param oldTagName
 * @param newTagName
 */
void TagManager::handleScriptingNotesTagRenaming(const Tag &tag, const QString &newTagName) {
    if (!ScriptingService::instance()->noteTaggingHookExists()) {
        return;
    }

    qDebug() << __func__;

    // workaround when signal blocking doesn't work correctly
    _mainWindow->directoryWatcherWorkaround(true, true);

    const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
    Q_UNUSED(blocker)

    const auto notes = Note::fetchAll();
    for (Note note : notes) {
        const QString oldNoteText = note.getNoteText();
        QString noteText =
            ScriptingService::instance()
                ->callNoteTaggingHook(note, QStringLiteral("rename"), tag.getName(), newTagName)
                .toString();

        // if nothing came back from callNoteTaggingHook let's try
        // callNoteTaggingByObjectHook
        if (noteText.isEmpty()) {
            noteText =
                ScriptingService::instance()
                    ->callNoteTaggingByObjectHook(note, QStringLiteral("rename"), tag, newTagName)
                    .toString();

            if (noteText.isEmpty() || (oldNoteText == noteText)) {
                continue;
            }
        }

        note.storeNewText(std::move(noteText));
    }

    _mainWindow->storeUpdatedNotesToDisk();

    // disable workaround
    _mainWindow->directoryWatcherWorkaround(false, true);

    reloadTagTree();

    // re-fetch current note to make sure the note text with the tag was updated
    _mainWindow->currentNote.refetch();
    _mainWindow->setNoteTextFromNote(&_mainWindow->currentNote);
}

/**
 * Takes care that a tag is removed from all notes
 *
 * @param tagName
 */
void TagManager::handleScriptingNotesTagRemoving(const Tag &tag, bool forBulkOperation) {
    if (!ScriptingService::instance()->noteTaggingHookExists()) {
        return;
    }

    qDebug() << __func__;

    if (!forBulkOperation) {
        // workaround when signal blocking doesn't work correctly
        _mainWindow->directoryWatcherWorkaround(true, true);
    }

    const QVector<Note> &notes = Note::fetchAll();
    for (const Note &note : notes) {
        handleScriptingNoteTagging(note, tag, true, false);
    }

    if (!forBulkOperation) {
        _mainWindow->storeUpdatedNotesToDisk();

        // disable workaround
        _mainWindow->directoryWatcherWorkaround(false, true);

        reloadTagTree();
    }
}

/**
 * Hides the note tag line edit after editing
 */
void TagManager::on_newNoteTagLineEdit_editingFinished() {
    _ui->newNoteTagLineEdit->setVisible(false);
    _ui->newNoteTagButton->setVisible(true);
}

/**
 * Reloads the note tag buttons for the current note (or the selected notes)
 */
void TagManager::reloadCurrentNoteTags() {
    // remove all remove-tag buttons
    QLayoutItem *child;
    while ((child = _ui->noteTagButtonFrame->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    int selectedNotesCount = _mainWindow->getSelectedNotesCount();
    bool currentNoteOnly = selectedNotesCount <= 1;
    _ui->selectedTagsToolButton->setVisible(!currentNoteOnly);
    _ui->newNoteTagButton->setToolTip(currentNoteOnly ? tr("Add a tag to the current note")
                                                      : tr("Add a tag to the selected notes"));
    QVector<Tag> tagList;

    _ui->multiSelectActionFrame->setVisible(!currentNoteOnly);
    _ui->noteEditorFrame->setVisible(currentNoteOnly);

    if (currentNoteOnly) {
        tagList = Tag::fetchAllOfNote(_mainWindow->currentNote);

        // only refresh the preview if we previously selected multiple notes
        // because we used it for showing note information
        if (_mainWindow->_lastNoteSelectionWasMultiple) {
            _mainWindow->_notePreviewHash.clear();
            _mainWindow->regenerateNotePreview();
        }
    } else {
        const QVector<Note> notes = _mainWindow->selectedNotes();
        tagList = Tag::fetchAllOfNotes(notes);

        // Count notes and folders separately
        const auto selectedItems = _ui->noteTreeWidget->selectedItems();
        int noteCount = 0;
        int folderCount = 0;
        for (const auto *item : selectedItems) {
            const int itemType = item->data(0, Qt::UserRole + 1).toInt();
            if (itemType == MainWindow::NoteType) {
                noteCount++;
            } else if (itemType == MainWindow::FolderType) {
                folderCount++;
            }
        }

        // Build selection text based on what's selected
        QString notesSelectedText;
        if (noteCount > 0 && folderCount > 0) {
            notesSelectedText = tr("%1 and %2")
                                    .arg(tr("%n note(s) selected", "", noteCount))
                                    .arg(tr("%n folder(s) selected", "", folderCount));
        } else if (folderCount > 0) {
            notesSelectedText = tr("%n folder(s) selected", "", folderCount);
        } else {
            notesSelectedText = tr("%n notes selected", "", noteCount);
        }

        _ui->selectedTagsToolButton->setText(QString::number(selectedNotesCount));
        _ui->selectedTagsToolButton->setToolTip(notesSelectedText);

        _ui->notesSelectedLabel->setText(notesSelectedText);

        // overwrite the note preview with a preview of the selected notes
        const QString previewHtml = Note::generateMultipleNotesPreviewText(notes);
#ifdef USE_QLITEHTML
        _mainWindow->_notePreviewWidget->setHtml(previewHtml);
#else
        _ui->noteTextView->setText(previewHtml);
#endif
    }

    _mainWindow->_lastNoteSelectionWasMultiple = !currentNoteOnly;

    // add all new remove-tag buttons
    for (const Tag &tag : Utils::asConst(tagList)) {
        QPushButton *button =
            new QPushButton(Utils::Misc::shorten(tag.getName(), 25), _ui->noteTagButtonFrame);
        button->setIcon(QIcon::fromTheme(
            QStringLiteral("tag-delete"),
            QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/xml-attribute-delete.svg"))));
        button->setToolTip(currentNoteOnly
                               ? tr("Remove tag '%1' from the current note").arg(tag.getName())
                               : tr("Remove tag '%1' from the selected notes").arg(tag.getName()));
        button->setObjectName(QStringLiteral("removeNoteTag") + QString::number(tag.getId()));

        QObject::connect(button, &QPushButton::clicked, this, &TagManager::removeNoteTagClicked);

        _ui->noteTagButtonFrame->layout()->addWidget(button);
    }

    //    // find tags not in common of selected notes
    //    if (selectedNotesCount > 1) {
    //        QLabel *noteTagButtonFrame = new QLabel("+3 tags");
    //        ui->noteTagButtonFrame->layout()->addWidget(noteTagButtonFrame);
    //    }

    // add a spacer to prevent the button items to take the full width
    auto *spacer = new QSpacerItem(0, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);
    _ui->noteTagButtonFrame->layout()->addItem(spacer);

    highlightCurrentNoteTagsInTagTree();
}

/**
 * Highlights the tags of the current note in the tag tree
 */
void TagManager::highlightCurrentNoteTagsInTagTree() {
    const int selectedNotesCount = _mainWindow->getSelectedNotesCount();
    const bool currentNoteOnly = selectedNotesCount <= 1;
    QVector<Tag> tagList;

    if (currentNoteOnly) {
        tagList = Tag::fetchAllOfNote(_mainWindow->currentNote);
    } else {
        const QVector<Note> &notes = _mainWindow->selectedNotes();
        tagList = Tag::fetchAllOfNotes(notes);
    }

    const QSignalBlocker blocker1(_ui->tagTreeWidget);
    Q_UNUSED(blocker1)

    Utils::Gui::resetBoldStateOfAllTreeWidgetItems(_ui->tagTreeWidget);

    for (const Tag &tag : Utils::asConst(tagList)) {
        QTreeWidgetItem *item =
            Utils::Gui::getTreeWidgetItemWithUserData(_ui->tagTreeWidget, tag.getId());

        if (item != nullptr) {
            // set tag item in tag tree widget to bold if note has tag
            auto font = item->font(0);
            if (!font.bold()) {
                font.setBold(true);
                item->setFont(0, font);
            }
        }
    }
}

/**
 * Removes a note tag link
 */
void TagManager::removeNoteTagClicked() {
    QString objectName = sender()->objectName();
    if (objectName.startsWith(QLatin1String("removeNoteTag"))) {
        const int tagId = objectName.remove(QLatin1String("removeNoteTag")).toInt();
        const Tag tag = Tag::fetch(tagId);
        if (!tag.isFetched()) {
            return;
        }

        // workaround when signal blocking doesn't work correctly
        _mainWindow->directoryWatcherWorkaround(true, true);

        const int selectedNotesCount = _mainWindow->getSelectedNotesCount();

        if (selectedNotesCount <= 1) {
            tag.removeLinkToNote(_mainWindow->currentNote);

            // remove the tag from the note text if defined via scripting engine
            handleScriptingNoteTagging(_mainWindow->currentNote, tag, true);
        } else {
            const auto selectedNotesList = _mainWindow->selectedNotes();
            for (const Note &note : selectedNotesList) {
                if (!tag.isLinkedToNote(note)) {
                    continue;
                }

                tag.removeLinkToNote(note);

                // remove the tag from the note text if defined via
                // scripting engine
                handleScriptingNoteTagging(note, tag, true);
            }
        }

        reloadCurrentNoteTags();
        reloadTagTree();
        _mainWindow->filterNotesByTag();

        // handle the coloring of the note in the note tree widget
        handleNoteTreeTagColoringForNote(_mainWindow->currentNote);

        // disable workaround
        _mainWindow->directoryWatcherWorkaround(false, true);
    }
}

/**
 * Allows the user to add a tag to the current note
 */
void TagManager::on_action_new_tag_triggered() { on_newNoteTagButton_clicked(); }

/**
 * Stores the tag after it was edited
 */
void TagManager::on_tagTreeWidget_itemChanged(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column)

    Tag tag = Tag::fetch(item->data(0, Qt::UserRole).toInt());
    if (tag.isFetched()) {
        const QString oldName = tag.getName();
        const QString name = item->text(0);

        // workaround when signal block doesn't work correctly
        _mainWindow->directoryWatcherWorkaround(true, true);

        if (!name.isEmpty()) {
            const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
            Q_UNUSED(blocker)

            // take care that a tag is renamed in all notes
            handleScriptingNotesTagRenaming(tag, name);

            tag.setName(name);
            tag.store();
        }

        // we also have to reload the tag tree if we don't change the tag
        // name to get the old name back
        reloadTagTree();
        reloadCurrentNoteTags();

        // turn off the workaround again
        _mainWindow->directoryWatcherWorkaround(false, true);
    }
}

/**
 * Sets a new active tag
 */
void TagManager::on_tagTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                     QTreeWidgetItem *previous) {
    Q_UNUSED(previous)

    if (current == nullptr) {
        return;
    }

    // set the tag id as active
    const int tagId = current->data(0, Qt::UserRole).toInt();
    Tag::setAsActive(tagId);

    const int count = _ui->tagTreeWidget->selectedItems().count();
    if (count > 1) return;

    const QSignalBlocker blocker(_ui->searchLineEdit);
    Q_UNUSED(blocker)

    _ui->searchLineEdit->clear();
    _mainWindow->filterNotes();
}

/**
 * Jumps to a tag in the tag tree
 *
 * @param tagId
 * @return
 */
bool TagManager::jumpToTag(int tagId) {
    QTreeWidgetItem *item = Utils::Gui::getTreeWidgetItemWithUserData(_ui->tagTreeWidget, tagId);

    if (item != nullptr) {
        // If the selection isn't cleared then the old subfolder is still selected too
        _ui->tagTreeWidget->clearSelection();
        _ui->tagTreeWidget->setCurrentItem(item);

        return true;
    }

    return false;
}

/**
 * Triggers filtering when multiple tags are selected
 */
void TagManager::on_tagTreeWidget_itemSelectionChanged() {
    const int count = _ui->tagTreeWidget->selectedItems().count();

    if (count <= 1) {
        if (count == 1) {
            //           on_tagTreeWidget_currentItemChanged(_ui->tagTreeWidget->selectedItems().first(),
            //                                                nullptr);
        }
        return;
    }

    const QSignalBlocker blocker(_ui->searchLineEdit);
    Q_UNUSED(blocker)

    _ui->searchLineEdit->clear();
    _mainWindow->filterNotes();
}

/**
 * Creates a context menu for the tag tree widget
 */
void TagManager::on_tagTreeWidget_customContextMenuRequested(const QPoint pos) {
    // don't open the most of the context menu if no tags are selected
    const bool hasSelected = _ui->tagTreeWidget->selectedItems().count() > 0;

    const QPoint globalPos = _ui->tagTreeWidget->mapToGlobal(pos);
    QMenu menu;

    QAction *addAction = menu.addAction(tr("&Add tag"));

    // allow these actions only if tags are selected
    QAction *renameAction = nullptr;
    QAction *assignColorAction = nullptr;
    QAction *disableColorAction = nullptr;
    QAction *removeAction = nullptr;
    if (hasSelected) {
        renameAction = menu.addAction(tr("Rename tag"));
        assignColorAction = menu.addAction(tr("Assign color"));
        disableColorAction = menu.addAction(tr("Disable color"));
        removeAction = menu.addAction(tr("&Remove tags"));

        // build the tag moving menu
        QMenu *moveMenu = menu.addMenu(tr("&Move tags to…"));
        buildTagMoveMenuTree(moveMenu);
    }

    QAction *selectedItem = menu.exec(globalPos);

    if (selectedItem == nullptr) {
        return;
    }

    QTreeWidgetItem *item = _ui->tagTreeWidget->currentItem();

    if (selectedItem == addAction) {
        // open the "add new tag" dialog
        auto *dialog = new TagAddDialog(_mainWindow);
        const int dialogResult = dialog->exec();

        // if user pressed ok take the name
        if (dialogResult == QDialog::Accepted) {
            const QString name = dialog->name();
            if (!name.isEmpty()) {
                const int parentId = item->data(0, Qt::UserRole).toInt() < 0
                                         ? 0
                                         : item->data(0, Qt::UserRole).toInt();

                // create a new tag with the name
                Tag tag;
                tag.setParentId(parentId);
                tag.setName(name);
                tag.store();

                if (tag.isFetched()) {
                    reloadTagTree();
                } else {
                    _mainWindow->showStatusBarMessage(tr("Tag could not be created!"),
                                                      QStringLiteral("🏷️"), 3000);
                }
            }
        }

        delete (dialog);
        return;
    }

    if (selectedItem == assignColorAction) {
        // assign and store a color to all selected tags in the tag tree widget
        assignColorToSelectedTagItems();
        return;
    } else if (selectedItem == disableColorAction) {
        // disable the color of all selected tags
        const auto selectedTagItems = _ui->tagTreeWidget->selectedItems();
        for (QTreeWidgetItem *tagItem : selectedTagItems) {
            // disable the color of the tag
            disableColorOfTagItem(tagItem);
        }

        // reload the notes in the note tree widget to update the colors
        _mainWindow->loadNoteDirectoryList();
        return;
    }

    // don't allow clicking on non-tag items for removing, editing and colors
    if (item->data(0, Qt::UserRole).toInt() <= 0) {
        return;
    }

    if (selectedItem == removeAction) {
        // remove selected tag
        removeSelectedTags();
    } else if (selectedItem == renameAction) {
        _ui->tagTreeWidget->editItem(item);
    }
}

/**
 * Assigns and stores a color to a tag from the tag tree widget
 *
 * @param item
 */
void TagManager::assignColorToTagItem(QTreeWidgetItem *item) {
    const int tagId = item->data(0, Qt::UserRole).toInt();

    if (tagId <= 0) {
        return;
    }

    Tag tag = Tag::fetch(tagId);

    if (!tag.isFetched()) {
        return;
    }

    QColor color = tag.getColor();
    color = QColorDialog::getColor(color.isValid() ? color : QColor(Qt::white));

    if (color.isValid()) {
        tag.setColor(color);
        tag.store();

        // set the color of the tag tree widget item
        Utils::Gui::handleTreeWidgetItemTagColor(item, tag);

        // reload the notes in the note tree widget to update the colors
        _mainWindow->loadNoteDirectoryList();
    }
}

/**
 * Assigns and stores a color to all selected tags from the tag tree widget
 */
void TagManager::assignColorToSelectedTagItems() {
    QColor color;
    bool hasTags = false;

    // get the color of a selected tag
    const auto selectedItems = _ui->tagTreeWidget->selectedItems();
    for (QTreeWidgetItem *item : selectedItems) {
        const int tagId = item->data(0, Qt::UserRole).toInt();
        if (tagId > 0) {
            const Tag tag = Tag::fetch(tagId);

            if (!tag.isFetched()) {
                continue;
            }

            color = tag.getColor();
            hasTags = true;
            break;
        }
    }

    if (!hasTags) {
        return;
    }

    color = QColorDialog::getColor(color.isValid() ? color : QColor(Qt::white));

    // store the color to all selected tags
    if (color.isValid()) {
        const auto selectedItems = _ui->tagTreeWidget->selectedItems();
        for (QTreeWidgetItem *item : selectedItems) {
            const int tagId = item->data(0, Qt::UserRole).toInt();
            if (tagId <= 0) {
                continue;
            }

            Tag tag = Tag::fetch(tagId);
            if (!tag.isFetched()) {
                continue;
            }

            tag.setColor(color);
            tag.store();

            // set the color of the tag tree widget item
            Utils::Gui::handleTreeWidgetItemTagColor(item, tag);
        }
    }

    // reload the notes in the note tree widget to update the colors
    _mainWindow->loadNoteDirectoryList();
}

/**
 * Disables a color of a tag from the tag tree widget
 *
 * @param item
 */
void TagManager::disableColorOfTagItem(QTreeWidgetItem *item) {
    const int tagId = item->data(0, Qt::UserRole).toInt();
    Tag tag = Tag::fetch(tagId);

    if (!tag.isFetched()) {
        return;
    }

    tag.setColor(QColor());
    tag.store();

    // set the color of the tag tree widget item
    Utils::Gui::handleTreeWidgetItemTagColor(item, tag);
}

/**
 * Populates a tag menu tree for moving tags
 */
void TagManager::buildTagMoveMenuTree(QMenu *parentMenu, int parentTagId) {
    const auto tagList = Tag::fetchAllByParentId(parentTagId, QStringLiteral("t.name ASC"));

    for (const Tag &tag : tagList) {
        const int tagId = tag.getId();
        const QString name = tag.getName();

        const int count = Tag::countAllParentId(tagId);
        if (count > 0) {
            // if there are sub-tag build a new menu level
            auto *tagMenu = parentMenu->addMenu(name);
            buildTagMoveMenuTree(tagMenu, tagId);
        } else {
            // if there are no sub-tags just create a named action
            auto *action = parentMenu->addAction(name);

            connect(action, &QAction::triggered, this,
                    [this, tagId]() { moveSelectedTagsToTagId(tagId); });
        }
    }

    // add an action to move to this tag
    parentMenu->addSeparator();
    QAction *action = parentMenu->addAction(
        parentTagId == 0
            ? tr("Move to the root", "to move a tag to the current tag in the tag context menu")
            : tr("Move to this tag"));
    action->setData(parentTagId);

    connect(action, &QAction::triggered, this,
            [this, parentTagId]() { moveSelectedTagsToTagId(parentTagId); });
}

/**
 * Populates a tag menu tree for bulk note tagging
 */
void TagManager::buildBulkNoteTagMenuTree(QMenu *parentMenu, int parentTagId) {
    const auto tagList = Tag::fetchAllByParentId(parentTagId, QStringLiteral("t.name ASC"));

    for (const Tag &tag : tagList) {
        const int tagId = tag.getId();
        const QString name = tag.getName();

        const int count = Tag::countAllParentId(tagId);
        if (count > 0) {
            // if there are sub-tag build a new menu level
            QMenu *tagMenu = parentMenu->addMenu(name);
            buildBulkNoteTagMenuTree(tagMenu, tagId);
        } else {
            // if there are no sub-tags just create a named action
            QAction *action = parentMenu->addAction(name);

            connect(action, &QAction::triggered, this,
                    [this, tagId]() { tagSelectedNotesToTagId(tagId); });
        }
    }

    if (parentTagId > 0) {
        // add an action to tag this
        parentMenu->addSeparator();
        QAction *action = parentMenu->addAction(tr("Tag this"));
        action->setData(parentTagId);

        connect(action, &QAction::triggered, this,
                [this, parentTagId]() { tagSelectedNotesToTagId(parentTagId); });
    }
}

/**
 * Moves selected tags to tagId
 */
void TagManager::moveSelectedTagsToTagId(int tagId) {
    qDebug() << __func__ << " - 'tagId': " << tagId;
    QVector<Tag> tagList;

    // gather tags to move (since we can't be sure the tag tree will not get
    // reloaded when we are actually moving the first tag)
    const auto selectedItems = _ui->tagTreeWidget->selectedItems();
    for (QTreeWidgetItem *item : selectedItems) {
        const int id = item->data(0, Qt::UserRole).toInt();
        Tag tag = Tag::fetch(id);
        if (tag.isFetched()) {
            if (tag.hasChild(tagId) || (id == tagId)) {
                _mainWindow->showStatusBarMessage(
                    tr("Cannot move tag '%1' to this tag").arg(tag.getName()), QStringLiteral("🏷️"),
                    3000);
            } else {
                tagList << tag;
            }
        }
    }

    if (tagList.count() > 0) {
        const bool useScriptingEngine = ScriptingService::instance()->noteTaggingHookExists();

        // workaround when signal block doesn't work correctly
        _mainWindow->directoryWatcherWorkaround(true, true);

        // move tags
        for (Tag tag : Utils::asConst(tagList)) {
            if (useScriptingEngine) {
                const QVector<Tag> tagsToHandle = Tag::fetchRecursivelyByParentId(tag.getId());

                // check all tags we need to handle
                for (const Tag &tagToHandle : tagsToHandle) {
                    // remove tag from all notes
                    for (const Note &note : tagToHandle.fetchAllLinkedNotes()) {
                        handleScriptingNoteTagging(note, tagToHandle, true, false);
                    }
                }
            }

            tag.setParentId(tagId);
            tag.store();

            if (useScriptingEngine) {
                const QVector<Tag> tagsToHandle = Tag::fetchRecursivelyByParentId(tag.getId());

                // check all tags we need to handle
                for (const Tag &tagToHandle : tagsToHandle) {
                    // add tag to all notes
                    for (const Note &note : tagToHandle.fetchAllLinkedNotes()) {
                        handleScriptingNoteTagging(note, tagToHandle, false, false);
                    }
                }
            }

            _mainWindow->showStatusBarMessage(tr("Moved tag '%1' to new tag").arg(tag.getName()),
                                              QStringLiteral("🏷️"), 3000);
        }

        reloadCurrentNoteTags();
        reloadTagTree();

        // turn off the workaround again
        _mainWindow->directoryWatcherWorkaround(false, true);
    }
}

/**
 * Tag selected notes to tagId
 */
void TagManager::tagSelectedNotesToTagId(int tagId) {
    qDebug() << __func__ << " - 'tagId': " << tagId;
    const Tag tag = Tag::fetch(tagId);

    // tag notes
    if (tag.isFetched()) {
        tagSelectedNotes(tag);
    }
}

/**
 * Tags selected notes
 */
void TagManager::tagSelectedNotes(const Tag &tag) {
    // Count only note items (not folders) for the confirmation dialog
    const auto selItems = _ui->noteTreeWidget->selectedItems();
    int noteCount = 0;
    for (const auto *item : selItems) {
        if (item->data(0, Qt::UserRole + 1).toInt() == MainWindow::NoteType) {
            noteCount++;
        }
    }

    if (noteCount == 0) {
        return;
    }

    if (Utils::Gui::question(_mainWindow, tr("Tag selected notes"),
                             tr("Tag %n selected note(s) with <strong>%2</strong>?", "", noteCount)
                                 .arg(tag.getName()),
                             QStringLiteral("tag-notes")) == QMessageBox::Yes) {
        int tagCount = 0;
        const bool useScriptingEngine = ScriptingService::instance()->noteTaggingHookExists();

        // workaround when signal block doesn't work correctly
        _mainWindow->directoryWatcherWorkaround(true, true);

        const auto selectedItems = _ui->noteTreeWidget->selectedItems();
        for (QTreeWidgetItem *item : selectedItems) {
            if (item->data(0, Qt::UserRole + 1) != MainWindow::NoteType) {
                continue;
            }

            const int noteId = item->data(0, Qt::UserRole).toInt();
            const Note note = Note::fetch(noteId);

            if (!note.isFetched()) {
                continue;
            }

            const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
            Q_UNUSED(blocker)

            if (useScriptingEngine) {
                // add the tag to the note text if defined via
                // scripting engine
                handleScriptingNoteTagging(note, tag, false, false);
            }

            // tag note
            const bool result = tag.linkToNote(note);

            if (result) {
                tagCount++;
                qDebug() << "Note was tagged:" << note.getName();

                // handle the coloring of the note in the note tree widget
                handleNoteTreeTagColoringForNote(note);
            } else {
                qWarning() << "Could not tag note:" << note.getName();
            }
        }

        if (useScriptingEngine) {
            const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
            Q_UNUSED(blocker)

            _mainWindow->storeUpdatedNotesToDisk();
        }

        reloadCurrentNoteTags();
        reloadTagTree();

        _mainWindow->showStatusBarMessage(
            tr("%n note(s) were tagged with \"%2\"", "", tagCount).arg(tag.getName()),
            QStringLiteral("🏷️"), 5000);

        // turn off the workaround again
        _mainWindow->directoryWatcherWorkaround(false, true);
    }
}

/**
 * Removes a tag from the selected notes
 */
void TagManager::removeTagFromSelectedNotes(const Tag &tag) {
    // Count only note items (not folders) for the confirmation dialog
    const auto selItems = _ui->noteTreeWidget->selectedItems();
    int noteCount = 0;
    for (const auto *item : selItems) {
        if (item->data(0, Qt::UserRole + 1).toInt() == MainWindow::NoteType) {
            noteCount++;
        }
    }

    if (noteCount == 0) {
        return;
    }

    if (Utils::Gui::question(
            _mainWindow, tr("Remove tag from selected notes"),
            tr("Remove tag <strong>%1</strong> from %n selected note(s)?", "", noteCount)
                .arg(tag.getName()),
            QStringLiteral("remove-tag-from-notes")) == QMessageBox::Yes) {
        int tagCount = 0;
        const bool useScriptingEngine = ScriptingService::instance()->noteTaggingHookExists();

        // workaround when signal blocking doesn't work correctly
        _mainWindow->directoryWatcherWorkaround(true, true);

        const auto selectedItems = _ui->noteTreeWidget->selectedItems();
        for (auto *item : selectedItems) {
            if (item->data(0, Qt::UserRole + 1) != MainWindow::NoteType) {
                continue;
            }

            const int noteId = item->data(0, Qt::UserRole).toInt();
            const Note note = Note::fetch(noteId);

            if (!note.isFetched()) {
                continue;
            }

            const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
            Q_UNUSED(blocker)

            if (useScriptingEngine) {
                // take care that the tag is removed from the note
                handleScriptingNoteTagging(note, tag, true, false);
            }

            // tag note
            const bool result = tag.removeLinkToNote(note);

            if (result) {
                tagCount++;
                qDebug() << "Tag was removed from note:" << note.getName();

                // handle the coloring of the note in the note tree widget
                handleNoteTreeTagColoringForNote(note);
            } else {
                qWarning() << "Could not remove tag from note:" << note.getName();
            }
        }

        if (useScriptingEngine) {
            const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
            Q_UNUSED(blocker)

            _mainWindow->storeUpdatedNotesToDisk();
        }

        reloadCurrentNoteTags();
        reloadTagTree();
        _mainWindow->filterNotesByTag();

        Utils::Gui::information(
            _mainWindow, tr("Done"),
            tr("Tag <strong>%1</strong> was removed from %n note(s)", "", tagCount)
                .arg(tag.getName()),
            QStringLiteral("tag-removed-from-notes"));

        // turn off the workaround again
        _mainWindow->directoryWatcherWorkaround(false, true);
    }
}

/**
 * Removes selected tags after a confirmation
 */
void TagManager::removeSelectedTags() {
    const int selectedItemsCount = _ui->tagTreeWidget->selectedItems().size();

    if (selectedItemsCount == 0) {
        return;
    }

    if (Utils::Gui::question(_mainWindow, tr("Remove selected tags"),
                             tr("Remove <strong>%n</strong> selected tag(s)? No notes will "
                                "be removed in this process.",
                                "", selectedItemsCount),
                             QStringLiteral("remove-tags")) == QMessageBox::Yes) {
        const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        const QSignalBlocker blocker1(_ui->tagTreeWidget);
        Q_UNUSED(blocker1)

        // workaround when signal blocking doesn't work correctly
        _mainWindow->directoryWatcherWorkaround(true, true);

        const auto selItems = _ui->tagTreeWidget->selectedItems();
        for (QTreeWidgetItem *item : selItems) {
            const int tagId = item->data(0, Qt::UserRole).toInt();
            const Tag tag = Tag::fetch(tagId);

            // take care that the tag is removed from all notes
            handleScriptingNotesTagRemoving(tag, true);

            // remove tag after handled by scripts so it still can be accessed by them
            tag.remove();
            qDebug() << "Removed tag " << tag.getName();
        }

        if (ScriptingService::instance()->noteTaggingHookExists()) {
            _mainWindow->storeUpdatedNotesToDisk();
        }

        // disable workaround
        _mainWindow->directoryWatcherWorkaround(false, true);

        reloadCurrentNoteTags();
        reloadTagTree();
    }
}

/**
 * Handle the coloring of the note in the note tree widget
 *
 * @param note
 */
void TagManager::handleNoteTreeTagColoringForNote(const Note &note) {
    const Tag colorTag = Tag::fetchOneOfNoteWithColor(note);
    QTreeWidgetItem *noteItem = _mainWindow->findNoteInNoteTreeWidget(note);
    Utils::Gui::handleTreeWidgetItemTagColor(noteItem, colorTag);
}

/**
 * Stores the note tag tree expand state when an tree widget item was collapsed
 */
void TagManager::on_tagTreeWidget_itemCollapsed(QTreeWidgetItem *item) {
    on_tagTreeWidget_itemExpanded(item);
}

/**
 * Stores the note tag tree expand state when an tree widget item was expanded
 */
void TagManager::on_tagTreeWidget_itemExpanded(QTreeWidgetItem *item) {
    Q_UNUSED(item)
    storeTagTreeWidgetExpandState();
}

/**
 * Stores the note tag tree expand state
 */
void TagManager::storeTagTreeWidgetExpandState() const {
    // get all items
    const auto allItems =
        _ui->tagTreeWidget->findItems(QLatin1String(""), Qt::MatchContains | Qt::MatchRecursive);

    QStringList expandedList;
    for (QTreeWidgetItem *item : allItems) {
        if (Utils::Gui::isOneTreeWidgetItemChildVisible(item)) {
            if (item->isExpanded()) {
                expandedList << item->data(0, Qt::UserRole).toString();
            }
        }
    }

    SettingsService settings;
    settings.setValue(QStringLiteral("MainWindow/tagTreeWidgetExpandState-") +
                          QString::number(NoteFolder::currentNoteFolderId()),
                      expandedList);
}

/**
 * Double-clicking a tag assigns the tag to the current note
 */
void TagManager::on_tagTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column)
    Tag tag = Tag::fetch(item->data(0, Qt::UserRole).toInt());

    if (tag.isFetched()) {
        // workaround when signal block doesn't work correctly
        _mainWindow->directoryWatcherWorkaround(true, true);

        const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        if (tag.isLinkedToNote(_mainWindow->currentNote)) {
            tag.removeLinkToNote(_mainWindow->currentNote);
            handleScriptingNoteTagging(_mainWindow->currentNote, tag, true, false);
        } else {
            tag.linkToNote(_mainWindow->currentNote);
            handleScriptingNoteTagging(_mainWindow->currentNote, tag, false, false);
        }

        if (!NoteFolder::isCurrentNoteTreeEnabled()) {
            _mainWindow->filterNotes();
        }

        reloadCurrentNoteTags();
        reloadTagTree();

        // turn off the workaround again
        _mainWindow->directoryWatcherWorkaround(false, true);
    }
}

void TagManager::clearTagFilteringColumn() {
    QTreeWidgetItemIterator it(_ui->noteTreeWidget);
    while (*it) {
        // if the item wasn't filtered by the searchLineEdit
        if ((*it)->data(4, Qt::UserRole).toBool()) {
            (*it)->setData(4, Qt::UserRole, false);
        }
        // reset the value for searchLineEdit
        ++it;
    }
}

void TagManager::resizeTagTreeWidgetColumnToContents() const {
    auto header = _ui->tagTreeWidget->header();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
}
