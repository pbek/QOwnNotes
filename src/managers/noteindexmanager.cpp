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

#include "noteindexmanager.h"

#include <dialogs/notediffdialog.h>
#include <entities/calendaritem.h>
#include <entities/note.h>
#include <entities/notefolder.h>
#include <entities/notesubfolder.h>
#include <entities/tag.h>
#include <entities/trashitem.h>
#include <services/databaseservice.h>
#include <services/metricsservice.h>
#include <services/owncloudservice.h>
#include <services/scriptingservice.h>
#include <services/settingsservice.h>
#include <services/updateservice.h>
#include <utils/gui.h>
#include <utils/misc.h>
#include <widgets/notesubfoldertree.h>
#include <widgets/notetreewidgetitem.h>

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileSystemWatcher>
#include <QProgressDialog>
#include <QTimer>
#include <QTreeWidgetItem>

#include "mainwindow.h"
#include "ui_mainwindow.h"

struct FileWatchDisabler {
    FileWatchDisabler(MainWindow *mw) : _mainWindow(mw) {
        Q_ASSERT(mw);
        QObject::disconnect(&mw->noteDirectoryWatcher, nullptr, nullptr, nullptr);
    }

    ~FileWatchDisabler() {
        Q_ASSERT(_mainWindow);
        _mainWindow->connectFileWatcher(/*delayed = */ true);
    }

   private:
    MainWindow *const _mainWindow = nullptr;
};

NoteIndexManager::NoteIndexManager(MainWindow *mainWindow, Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui) {}

/**
 * Builds the index of notes and note sub folders
 */
bool NoteIndexManager::buildNotesIndex(int noteSubFolderId, bool forceRebuild) {
    QString notePath = Utils::Misc::removeIfEndsWith(_mainWindow->notesPath, QDir::separator());
    NoteSubFolder noteSubFolder;
    bool hasNoteSubFolder = false;
    bool wasModified = false;

    if (noteSubFolderId == 0) {
        qDebug() << __func__ << " - 'noteSubFolderId': " << noteSubFolderId;

        // make sure we destroy nothing
        storeUpdatedNotesToDisk();

        // init the lists to check for removed items
        _mainWindow->_buildNotesIndexBeforeNoteIdList = Note::fetchAllIds();
        _mainWindow->_buildNotesIndexBeforeNoteSubFolderIdList = NoteSubFolder::fetchAllIds();
        _mainWindow->_buildNotesIndexAfterNoteIdList.clear();
        _mainWindow->_buildNotesIndexAfterNoteSubFolderIdList.clear();
    } else {
        noteSubFolder = NoteSubFolder::fetch(noteSubFolderId);
        hasNoteSubFolder = noteSubFolder.isFetched();

        if (!hasNoteSubFolder) {
            return false;
        }

        notePath += QDir::separator() + noteSubFolder.relativePath();
    }

    //    qDebug() << __func__ << " - 'notePath': " << notePath;

    QDir notesDir(notePath);

    // only show certain files
    auto filters = Note::noteFileExtensionList(QStringLiteral("*."));

    // show the newest entry first
    QStringList files = notesDir.entryList(filters, QDir::Files, QDir::Time);
    qDebug() << __func__ << " - 'files': " << files;

    Note::applyIgnoredNotesSetting(files);
    //    qDebug() << __func__ << " - 'files': " << files;

    bool createDemoNotes = (files.count() == 0) && !hasNoteSubFolder;

    if (createDemoNotes) {
        SettingsService settings;
        // check if we already have created the demo notes once
        createDemoNotes = !settings.value(QStringLiteral("demoNotesCreated")).toBool();

        if (createDemoNotes) {
            // we don't want to create the demo notes again
            settings.setValue(QStringLiteral("demoNotesCreated"), true);
        }
    }

    // add some notes if there aren't any, and we haven't already created them once
    if (createDemoNotes) {
        qDebug() << "No notes! We will add some...";
        const QStringList filenames =
            QStringList({"Markdown Cheatsheet.md", "Welcome to QOwnNotes.md"});

        // copy note files to the notes path
        for (const auto &filename : filenames) {
            const QString destinationFile = _mainWindow->notesPath + QDir::separator() + filename;
            QFile sourceFile(QStringLiteral(":/demonotes/") + filename);
            sourceFile.copy(destinationFile);
            // set read/write permissions for the owner and user
            QFile::setPermissions(destinationFile, QFile::ReadOwner | QFile::WriteOwner |
                                                       QFile::ReadUser | QFile::WriteUser);
        }

        // copy the shortcuts file and handle its file permissions
        //        destinationFile = this->notesPath + QDir::separator() +
        //              "Important Shortcuts.txt";
        //        QFile::copy( ":/shortcuts", destinationFile );
        //        QFile::setPermissions( destinationFile, QFile::ReadOwner |
        //                  QFile::WriteOwner | QFile::ReadUser |
        //                  QFile::WriteUser );

        // fetch all files again
        files = notesDir.entryList(filters, QDir::Files, QDir::Time);

        // jump to the welcome note in the note selector in 500ms
        QTimer::singleShot(500, _mainWindow, SLOT(jumpToWelcomeNote()));
    }

    // get the current crypto key to set it again
    // after all notes were read again
    const qint64 cryptoKey = _mainWindow->currentNote.getCryptoKey();
    const QString cryptoPassword = _mainWindow->currentNote.getCryptoPassword();

    if (!hasNoteSubFolder && forceRebuild) {
        // first delete all notes and note sub folders in the database if a
        // rebuild was forced
        Note::deleteAll();
        NoteSubFolder::deleteAll();
    }

    const bool withNoteNameHook = ScriptingService::instance()->handleNoteNameHookExists();
    const int numFiles = files.count();
    QProgressDialog progress(tr("Loading notes…"), tr("Abort"), 0, numFiles, _mainWindow);
    progress.setWindowModality(Qt::WindowModal);
    int currentCount = 0;

    _mainWindow->_buildNotesIndexAfterNoteIdList.reserve(files.size());
    const int maxNoteFileSize = Utils::Misc::getMaximumNoteFileSize();
    // create all notes from the files
    for (QString fileName : Utils::asConst(files)) {
        if (progress.wasCanceled()) {
            break;
        }

        if (hasNoteSubFolder) {
            fileName.prepend(noteSubFolder.relativePath() + QDir::separator());
        }

        // fetching the content of the file
        QFile file(Note::getFullFilePathForFile(fileName));

        if (file.size() > maxNoteFileSize) {
            qDebug() << "Note file '" << fileName << "' is too large: " << file.size() << " > "
                     << maxNoteFileSize;
            continue;
        }

        // update or create a note from the file
        bool noteWasUpdated = false;
        const Note note =
            Note::updateOrCreateFromFile(file, noteSubFolder, withNoteNameHook, &noteWasUpdated);

        // add the note id to in the end check if notes need to be removed
        _mainWindow->_buildNotesIndexAfterNoteIdList << note.getId();

        if (!_mainWindow->_buildNotesIndexBeforeNoteIdList.contains(note.getId()) ||
            noteWasUpdated) {
            wasModified = true;
        }

        // update the UI
        // this causes to show notes twice in the ui->noteTreeWidget if a
        // not selected note is modified externally
        // https://github.com/pbek/QOwnNotes/issues/242
        // using a blocker on noteTreeWidget or just processing every 10th
        // time doesn't work neither
        //            QCoreApplication::processEvents();

        // we try these two instead to update the UI
        // QCoreApplication::flush() is obsolete since Qt 5.9
        //            QCoreApplication::flush();

        // this still causes double entries on OS X and maybe Windows
#ifdef Q_OS_LINUX
        QCoreApplication::sendPostedEvents();
#endif
        progress.setValue(++currentCount);
    }

    progress.setValue(numFiles);

    // update the UI and get user input after all the notes were loaded
    // this still can cause duplicate note subfolders to be viewed
    //    QCoreApplication::processEvents();

    // re-fetch current note (because all the IDs have changed after the
    // buildNotesIndex()
    _mainWindow->currentNote.refetch();

    if (cryptoKey != 0) {
        // reset the old crypto key for the current note
        _mainWindow->currentNote.setCryptoKey(cryptoKey);
        _mainWindow->currentNote.setCryptoPassword(cryptoPassword);
        _mainWindow->currentNote.store();
    }

    // build the note sub folders
    const bool showSubfolders = NoteFolder::isCurrentHasSubfolders();
    if (showSubfolders) {
        const QStringList folders = notesDir.entryList(QDir::Dirs | QDir::Hidden, QDir::Time);

        for (const QString &folder : folders) {
            if (NoteSubFolder::willFolderBeIgnored(folder)) {
                continue;
            }

            // fetch or create the parent note sub folder
            NoteSubFolder parentNoteSubFolder =
                NoteSubFolder::fetchByNameAndParentId(folder, noteSubFolderId);
            if (!parentNoteSubFolder.isFetched()) {
                parentNoteSubFolder.setName(folder);
                parentNoteSubFolder.setParentId(noteSubFolderId);
                parentNoteSubFolder.store();

                wasModified = true;
            }

            if (parentNoteSubFolder.isFetched()) {
                // add the note id to in the end check if notes need to
                // be removed
                _mainWindow->_buildNotesIndexAfterNoteSubFolderIdList
                    << parentNoteSubFolder.getId();

                // build the notes index for the note subfolder
                const bool result = buildNotesIndex(parentNoteSubFolder.getId());
                if (result) {
                    wasModified = true;
                }

                // update the UI
                // this causes to show sub note folders twice in the
                // ui->noteSubFolderTreeWidget if a
                // not selected note is modified externally
                //                    QCoreApplication::processEvents();

                // we try these two instead to update the UI
                // QCoreApplication::flush() is obsolete since Qt 5.9
                //                    QCoreApplication::flush();

                // this still causes double entries on OS X and maybe
                // Windows
#ifdef Q_OS_LINUX
                QCoreApplication::sendPostedEvents();
#endif
            }
        }
    }

    if (!hasNoteSubFolder) {
        // check for removed notes
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        const QList<int> removedNoteIdList =
            QSet<int>(_mainWindow->_buildNotesIndexBeforeNoteIdList.begin(),
                      _mainWindow->_buildNotesIndexBeforeNoteIdList.end())
                .subtract(QSet<int>(_mainWindow->_buildNotesIndexAfterNoteIdList.begin(),
                                    _mainWindow->_buildNotesIndexAfterNoteIdList.end()))
                .values();
#else
        const QList<int> removedNoteIdList =
            _mainWindow->_buildNotesIndexBeforeNoteIdList.toList()
                .toSet()
                .subtract(_mainWindow->_buildNotesIndexAfterNoteIdList.toSet())
                .toList();
#endif

        // remove all missing notes
        for (const int noteId : removedNoteIdList) {
            Note note = Note::fetch(noteId);
            if (note.isFetched()) {
                note.remove();
                wasModified = true;
            }
        }

        // check for removed note subfolders
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        QList<int> removedNoteSubFolderIdList =
            QSet<int>(_mainWindow->_buildNotesIndexBeforeNoteSubFolderIdList.begin(),
                      _mainWindow->_buildNotesIndexBeforeNoteSubFolderIdList.end())
                .subtract(QSet<int>(_mainWindow->_buildNotesIndexAfterNoteSubFolderIdList.begin(),
                                    _mainWindow->_buildNotesIndexAfterNoteSubFolderIdList.end()))
                .values();
#else
        const QList<int> removedNoteSubFolderIdList =
            _mainWindow->_buildNotesIndexBeforeNoteSubFolderIdList.toList()
                .toSet()
                .subtract(_mainWindow->_buildNotesIndexAfterNoteSubFolderIdList.toSet())
                .toList();
#endif

        // remove all missing note subfolders
        for (const int _noteSubFolderId : removedNoteSubFolderIdList) {
            NoteSubFolder _noteSubFolder = NoteSubFolder::fetch(_noteSubFolderId);
            if (_noteSubFolder.isFetched()) {
                _noteSubFolder.remove();
                wasModified = true;
            }
        }

        // setup the note folder database
        DatabaseService::createNoteFolderConnection();
        DatabaseService::setupNoteFolderTables();

        // update the note directory watcher
        updateNoteDirectoryWatcher();

        // update the information about shared notes
        OwnCloudService *ownCloud = OwnCloudService::instance();
        ownCloud->fetchShares();
    }

    if (noteSubFolderId == 0) {
        removeConflictedNotesDatabaseCopies();

        // Log checksum calculation statistics for the entire note folder
        qint64 totalTime = 0;
        int callCount = 0;
        Note::getChecksumStats(totalTime, callCount);
        qDebug() << "====================================================";
        qDebug() << "Note folder loading complete - Checksum statistics:";
        qDebug() << "  Total calculateChecksum() calls:" << callCount;
        qDebug() << "  Total time spent:" << (totalTime / 1000000.0) << "ms";
        if (callCount > 0) {
            qDebug() << "  Average time per call:" << (totalTime / callCount / 1000.0)
                     << "microseconds";
        }
        qDebug() << "====================================================";
    }

    qDebug() << __func__ << " - 'noteSubFolderId': " << noteSubFolderId;
    return wasModified;
}

/**
 * Builds the note index and loads the note directory list
 *
 * @param forceBuild
 * @param forceLoad
 * @param reloadTabs
 */
void NoteIndexManager::buildNotesIndexAndLoadNoteDirectoryList(bool forceBuild, bool forceLoad,
                                                               bool reloadTabs) {
    const bool wasBuilt = buildNotesIndex(0, forceBuild);

    if (wasBuilt || forceLoad) {
        loadNoteDirectoryList();

        // Expire trashed items
        // It is safer to do that here than in MainWindow::frequentPeriodicChecker(), to avoid
        // sync errors after resuming from suspend
        TrashItem::expireItems();
    }

    if (wasBuilt && reloadTabs) {
        // restore the note tabs
        Utils::Gui::reloadNoteTabs(_ui->noteEditTabWidget);
    }
}

/**
 * Creates the items in the note tree widget from the note and note sub
 * folder tables
 */
void NoteIndexManager::loadNoteDirectoryList() {
    qDebug() << __func__;

    // Clean up favorite notes list (remove entries for deleted notes)
    Note::cleanupFavoriteNotes();

    const QSignalBlocker blocker(_ui->noteTextEdit);
    Q_UNUSED(blocker)

    const QSignalBlocker blocker2(_ui->noteTreeWidget);
    Q_UNUSED(blocker2)

    const bool isCurrentNoteTreeEnabled = NoteFolder::isCurrentNoteTreeEnabled();
    _ui->noteTreeWidget->clear();
    //    ui->noteTreeWidget->setRootIsDecorated(isCurrentNoteTreeEnabled);
    int itemCount;

    if (isCurrentNoteTreeEnabled) {
        auto *noteFolderItem = new QTreeWidgetItem();
        noteFolderItem->setText(0, tr("Note folder"));
        noteFolderItem->setData(0, Qt::UserRole, 0);
        noteFolderItem->setData(0, Qt::UserRole + 1, MainWindow::FolderType);
        noteFolderItem->setIcon(0, Utils::Gui::folderIcon());
        noteFolderItem->setForeground(1, QColor(Qt::gray));
        _ui->noteTreeWidget->addTopLevelItem(noteFolderItem);

        _ui->noteSubFolderTreeWidget->buildTreeForParentItem(noteFolderItem);
        noteFolderItem->setExpanded(true);

        itemCount = Note::countAll();
    } else {
        // load all notes and add them to the note list widget
        const QVector<Note> noteList = Note::fetchAll();
        for (const Note &note : noteList) {
            _mainWindow->addNoteToNoteTreeWidget(note);
        }

        itemCount = noteList.count();
    }

    MetricsService::instance()->sendEventIfEnabled(
        QStringLiteral("note/list/loaded"), QStringLiteral("note"),
        QStringLiteral("note list loaded"), QString::number(itemCount) + QStringLiteral(" notes"),
        itemCount);

    // sort alphabetically again if necessary
    SettingsService settings;
    if (settings.value(QStringLiteral("notesPanelSort"), SORT_BY_LAST_CHANGE).toInt() ==
        SORT_ALPHABETICAL) {
        _ui->noteTreeWidget->sortItems(
            0, Utils::Gui::toQtOrder(settings.value(QStringLiteral("notesPanelOrder")).toInt()));
    }

    // setup tagging
    _mainWindow->setupTags();

    if (!isCurrentNoteTreeEnabled) {
        // setup note sub folders
        _mainWindow->setupNoteSubFolders();
    }

    // generate the tray context menu
    _mainWindow->generateSystemTrayContextMenu();

    // clear the text edits if there is no visible note
    if (_mainWindow->firstVisibleNoteTreeWidgetItem() == nullptr) {
        _mainWindow->unsetCurrentNote();
    } else {
        const auto item = _mainWindow->findNoteInNoteTreeWidget(_mainWindow->currentNote);

        // in the end we need to set the current item again if we can find it
        if (item != nullptr) {
            _ui->noteTreeWidget->setCurrentItem(item);
        }
    }
}

/**
 * Updates the note directory watcher
 */
void NoteIndexManager::updateNoteDirectoryWatcher() {
    // clear all paths from the directory watcher
    clearNoteDirectoryWatcher();

    const bool hasSubfolders = NoteFolder::isCurrentHasSubfolders();
    //    if (showSubfolders) {
    //        return;
    //    }

    const QString notePath =
        Utils::Misc::removeIfEndsWith(_mainWindow->notesPath, QDir::separator());
    if (QDir(notePath).exists()) {
        // watch the notes directory for changes
        noteDirectoryWatcherAddPath(notePath);
    }

    // Add the .git folder to the watcher if it exists
    const QString gitPath = notePath + QDir::separator() + QStringLiteral(".git");
    if (QDir(gitPath).exists()) {
        addDirectoryToDirectoryWatcher(gitPath);
    }

    if (hasSubfolders) {
        const QVector<NoteSubFolder> noteSubFolderList = NoteSubFolder::fetchAll();
        for (const NoteSubFolder &noteSubFolder : noteSubFolderList) {
            const QString path = notePath + QDir::separator() + noteSubFolder.relativePath();

            QDir folderDir(path);

            if (folderDir.exists()) {
                // watch the note sub folder path for changes
                noteDirectoryWatcherAddPath(path);
            }
        }
    }

    int count = 0;
    const QVector<Note> noteList = Note::fetchAll();
    for (const Note &note : noteList) {
#ifdef Q_OS_LINUX
        // only add the last first 200 notes to the file watcher to
        // prevent that nothing is watched at all because of too many
        // open files
        if (count > 200) {
            break;
        }
#endif
        const QString path = note.fullNoteFilePath();
        const QFile file(path);

        if (file.exists()) {
            // watch the note for changes
            noteDirectoryWatcherAddPath(path);

            ++count;
        }
    }

    //    qDebug() << __func__ << " - 'noteDirectoryWatcher.files()': " <<
    //    noteDirectoryWatcher.files();
    //
    //    qDebug() << __func__ << " - 'noteDirectoryWatcher.directories()': " <<
    //    noteDirectoryWatcher.directories();
}

/**
 * Clears all paths from the directory watcher
 */
void NoteIndexManager::clearNoteDirectoryWatcher() {
    const QStringList fileList =
        _mainWindow->noteDirectoryWatcher.directories() + _mainWindow->noteDirectoryWatcher.files();
    if (fileList.count() > 0) {
        _mainWindow->noteDirectoryWatcher.removePaths(fileList);
    }
}

/**
 * Asks to remove conflicted copies of the notes.sqlite database
 */
void NoteIndexManager::removeConflictedNotesDatabaseCopies() {
    const QStringList filter{"notes (*conflicted copy *).sqlite"};
    QDirIterator it(NoteFolder::currentLocalPath(), filter,
                    QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    auto files = QStringList();
    const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
    Q_UNUSED(blocker)

    FileWatchDisabler disable(_mainWindow);

    while (it.hasNext()) {
        const QString &file = it.next();
        qDebug() << "Found conflicting note folder database: " << file;

        // check if conflicted database copy is the same as the current note
        // folder database
        if (Utils::Misc::isSameFile(file, DatabaseService::getNoteFolderDatabasePath())) {
            _mainWindow->showStatusBarMessage(
                QFile::remove(file)
                    ? tr("Removed duplicate conflicted database: %1").arg(file)
                    : tr("Could not remove duplicate conflicted database: %1").arg(file),
                QStringLiteral("🗄️"), 4000);
        } else if (DatabaseService::mergeNoteFolderDatabase(file)) {
            _mainWindow->showStatusBarMessage(
                QFile::remove(file)
                    ? tr("Removed merged conflicted database: %1").arg(file)
                    : tr("Could not remove merged conflicted database: %1").arg(file),
                QStringLiteral("🗄️"), 4000);
        } else {
            files << file;
        }
    }

    int count = files.count();

    if (count == 0) {
        return;
    }

    if (Utils::Gui::question(
            _mainWindow, tr("Delete conflicted database copies"),
            Utils::Misc::replaceOwnCloudText(
                tr("Proceed with automatic deletion of <strong>%n</strong>"
                   " conflicted database copies that may block your ownCloud"
                   " sync process?",
                   "", count)) +
                QStringLiteral("<br /><br />") + files.join(QStringLiteral("<br />")),
            QStringLiteral("delete-conflicted-database-files")) != QMessageBox::Yes) {
        return;
    }

    count = 0;

    // remove the database files
    for (const QString &file : Utils::asConst(files)) {
        if (QFile::remove(file)) {
            ++count;
        }
    }

    _mainWindow->showStatusBarMessage(tr("Removed %n conflicted database copies", "", count),
                                      QStringLiteral("🗄️"));
}

void NoteIndexManager::notesWereModified(const QString &str) {
    // workaround when signal block doesn't work correctly
    if (_mainWindow->_isNotesWereModifiedDisabled) {
        return;
    }

    // if we should ignore all changes return here
    if (SettingsService().value(QStringLiteral("ignoreAllExternalNoteFolderChanges")).toBool()) {
        return;
    }

    // We are ignoring changes in the .git folder
    if (str.contains(QStringLiteral("/.git/"))) {
        return;
    }

    QFileInfo fi(str);
    Note note = Note::fetchByFileUrl(QUrl::fromLocalFile(str));

    qDebug() << __func__ << " - 'str': " << str;
    qDebug() << __func__ << " - 'note': " << note;
    qDebug() << __func__ << " - 'currentNote': " << _mainWindow->currentNote;

    // load note from disk if current note was changed
    if ((note.getFileName() == _mainWindow->currentNote.getFileName()) &&
        (note.getNoteSubFolderId() == _mainWindow->currentNote.getNoteSubFolderId())) {
        if (note.fileExists()) {
            // We can't rely only on the modified timestamp because some sync clients
            // replace files via move operations and preserve timestamps.
            if (fi.lastModified() == _mainWindow->currentNote.getFileLastModified()) {
                qDebug() << __func__ << " - Modification date didn't change, continuing";
            }

            const QString oldNoteText = note.getNoteText();

            // fetch text of note from disk
            note.updateNoteTextFromDisk();
            const QString noteTextOnDisk = Utils::Misc::transformLineFeeds(note.getNoteText());
            const bool isCurrentNoteNotEditedForAWhile =
                _mainWindow->currentNoteLastEdited.addSecs(60) < QDateTime::currentDateTime();
            // If the current note wasn't edited for a while, we want that it is possible
            // to get updated even with small changes, so we are setting a threshold of 0.
            // If it was recently edited, we use a threshold of 8 for the similarity check
            // to avoid false positives, but we'll still show the dialog if changes are detected.
            const int threshold = isCurrentNoteNotEditedForAWhile ? 0 : 8;

            const QString noteTextOnDiskHash = QString(
                QCryptographicHash::hash(noteTextOnDisk.toLocal8Bit(), QCryptographicHash::Sha1)
                    .toHex());

            // skip dialog if text of note file on disk and current note are
            // equal
            if (noteTextOnDiskHash == _mainWindow->_currentNoteTextHash) {
                qDebug() << __func__
                         << " - Note text and _currentNoteTextHash are the same, ignoring";
                return;
            }

            // Check if the old note text is the same or similar as the one on disk
            // only if the current note wasn't edited recently
            if (isCurrentNoteNotEditedForAWhile &&
                Utils::Misc::isSimilar(oldNoteText, noteTextOnDisk, threshold)) {
                qDebug() << __func__ << " - Old and new text are same or similar, ignoring";
                return;
            }

            // fetch current text
            const QString noteTextEditText = _ui->noteTextEdit->toPlainText();

            // skip dialog if text of note file on disk text from note text
            // edit are equal or similar, but only if the note wasn't edited recently
            // If it was recently edited, we want to show the dialog even for small changes
            if (isCurrentNoteNotEditedForAWhile &&
                Utils::Misc::isSimilar(noteTextEditText, noteTextOnDisk, threshold)) {
                qDebug() << __func__ << " - Note text and text on disk are too similar, ignoring";
                return;
            }

            _mainWindow->showStatusBarMessage(tr("Current note was modified externally"),
                                              QStringLiteral("🔄"), 5000);

            // if we don't want to get notifications at all
            // external modifications check if we really need one
            if (!_mainWindow->notifyAllExternalModifications) {
                // reloading the current note text straight away
                // if we didn't change it for a minute
                if (!_mainWindow->currentNote.getHasDirtyData() &&
                    isCurrentNoteNotEditedForAWhile) {
                    _mainWindow->updateNoteTextFromDisk(std::move(note));
                    return;
                }
            }

            const int result = _mainWindow->openNoteDiffDialog(note);
            switch (result) {
                // overwrite file with local changes
                case NoteDiffDialog::Overwrite: {
                    // disconnect the watcher before saving on disk
                    FileWatchDisabler disable(_mainWindow);

                    _mainWindow->showStatusBarMessage(
                        tr("Overwriting external changes of: %1")
                            .arg(_mainWindow->currentNote.getFileName()),
                        QStringLiteral("💾"), 3000);

                    // the note text has to be stored newly because the
                    // external change is already in the note table entry
                    _mainWindow->currentNote.storeNewText(_ui->noteTextEdit->toPlainText());
                    _mainWindow->currentNote.storeNoteTextFileToDisk();
                } break;

                // reload note file from disk
                case NoteDiffDialog::Reload:
                    _mainWindow->showStatusBarMessage(
                        tr("Loading external changes from: %1")
                            .arg(_mainWindow->currentNote.getFileName()),
                        QStringLiteral("🔄"), 3000);
                    _mainWindow->updateNoteTextFromDisk(note);
                    break;

                    //                case NoteDiffDialog::Cancel:
                    //                case NoteDiffDialog::Ignore:
                default:
                    // do nothing
                    break;
            }
        } else if (_mainWindow->_noteExternallyRemovedCheckEnabled &&
                   (_mainWindow->currentNote.getNoteSubFolderId() == 0)) {
            // only allow the check if current note was removed externally in
            // the root note folder, because it gets triggered every time
            // a note gets renamed in subfolders

            qDebug() << "Current note was removed externally!";

            if (Utils::Gui::questionNoSkipOverride(
                    _mainWindow, tr("Note was removed externally!"),
                    tr("Current note was removed outside of this application!\n"
                       "Restore current note?"),
                    QStringLiteral("restore-note")) == QMessageBox::Yes) {
                const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
                Q_UNUSED(blocker)

                QString text = _ui->noteTextEdit->toPlainText();
                note.storeNewText(std::move(text));

                // store note to disk again
                const bool noteWasStored = note.storeNoteTextFileToDisk();
                _mainWindow->showStatusBarMessage(
                    noteWasStored ? tr("Stored current note to disk")
                                  : tr("Current note could not be stored to disk"),
                    noteWasStored ? QStringLiteral("💾") : QStringLiteral("❌"), 3000);

                // rebuild and reload the notes directory list
                buildNotesIndexAndLoadNoteDirectoryList();

                // fetch note new (because all the IDs have changed
                // after the buildNotesIndex()
                note.refetch();

                // restore old selected row (but don't update the note text)
                _mainWindow->setCurrentNote(note, false);
            } else {
                // rebuild and reload the notes directory list
                buildNotesIndexAndLoadNoteDirectoryList();

                _mainWindow->resetCurrentNote(true);
            }
        }
    } else {
        qDebug() << "other note was changed: " << str;

        _mainWindow->showStatusBarMessage(tr("Note was modified externally: %1").arg(str),
                                          QStringLiteral("🔄"), 5000);

        // rebuild and reload the notes directory list
        buildNotesIndexAndLoadNoteDirectoryList();
        _mainWindow->setCurrentNote(std::move(_mainWindow->currentNote), false);
    }
}

void NoteIndexManager::notesDirectoryWasModified(const QString &str) {
    // workaround when signal block doesn't work correctly
    if (_mainWindow->_isNotesDirectoryWasModifiedDisabled) {
        return;
    }

    // if we should ignore all changes return here
    if (SettingsService().value(QStringLiteral("ignoreAllExternalNoteFolderChanges")).toBool()) {
        return;
    }

    // We are ignoring changes in the .git folder
    if (str.contains(QStringLiteral("/.git/"))) {
        return;
    }

    qDebug() << "notesDirectoryWasModified: " << str;
    _mainWindow->showStatusBarMessage(tr("Notes directory was modified externally"),
                                      QStringLiteral("🔄"), 5000);

    // rebuild and reload the notes directory list
    buildNotesIndexAndLoadNoteDirectoryList();

    // check if the current note was modified
    // this fixes not detected external note changes of the current note if the
    // event for the change in the current note comes after the event that the
    // note folder was modified
    QString noteFileName = _mainWindow->currentNote.getFileName();
    if (!noteFileName.isEmpty()) {
        // Use the full path, like a filesystem watcher would, instead of just the file-name
        notesWereModified(_mainWindow->currentNote.fullNoteFilePath());
    }

    // Schedule a deferred re-index to handle sync clients (e.g. Nextcloud) that
    // use a temp-file + atomic rename strategy to update note files. The rename
    // may complete after the first directoryChanged event fires — and on Linux
    // inotify does not always emit a second event for an in-place rename — so
    // the initial buildNotesIndexAndLoadNoteDirectoryList() above may run before
    // the updated content is in place. The deferred pass captures the final state.
    // See: https://github.com/pbek/QOwnNotes/issues/3468
    QTimer::singleShot(1000, _mainWindow, [this]() {
        if (!_mainWindow->_isNotesDirectoryWasModifiedDisabled) {
            qDebug() << __func__ << " - deferred re-index after external directory change";
            buildNotesIndexAndLoadNoteDirectoryList();
        }
    });

    // also update the text of the text edit if current note has changed
    bool updateNoteText = !_mainWindow->currentNote.exists();
    qDebug() << "updateNoteText: " << updateNoteText;

    // restore old selected row (but don't update the note text)
    _mainWindow->setCurrentNote(std::move(_mainWindow->currentNote), updateNoteText);
}

void NoteIndexManager::storeUpdatedNotesToDisk() {
    // disconnect the watcher before saving on disk
    FileWatchDisabler disable(_mainWindow);

    const QString oldNoteName = _mainWindow->currentNote.getName();

    // For some reason this->noteDirectoryWatcher gets an event from this.
    // I didn't find another solution than to wait yet.
    // All flushing and syncing didn't help.
    bool currentNoteChanged = false;
    bool noteWasRenamed = false;
    bool currentNoteTextChanged = false;

    // currentNote will be set by this method if the filename has changed
    const int count = Note::storeDirtyNotesToDisk(_mainWindow->currentNote, &currentNoteChanged,
                                                  &noteWasRenamed, &currentNoteTextChanged);

    if (count > 0) {
        _mainWindow->_noteViewNeedsUpdate = true;

        MetricsService::instance()->sendEventIfEnabled(
            QStringLiteral("note/notes/stored"), QStringLiteral("note"),
            QStringLiteral("notes stored"), QString::number(count) + QStringLiteral(" notes"),
            count);

        qDebug() << __func__ << " - 'count': " << count;

        _mainWindow->showStatusBarMessage(tr("Stored %n note(s) to disk", "", count),
                                          QStringLiteral("💾"), 3000);

        if (currentNoteChanged) {
            // strip trailing spaces of the current note (if enabled)
            if (SettingsService().value(QStringLiteral("Editor/removeTrailingSpaces")).toBool()) {
                const bool wasStripped = _mainWindow->currentNote.stripTrailingSpaces(
                    _mainWindow->activeNoteTextEdit()->textCursor().position());

                if (wasStripped) {
                    qDebug() << __func__ << " - 'wasStripped'";

                    // updating the current note text is disabled because it
                    // moves the cursor to the top
                    //                    const QSignalBlocker blocker2(activeNoteTextEdit());
                    //                    Q_UNUSED(blocker2)
                    //                    setNoteTextFromNote(&currentNote);
                }
            }

            if (currentNoteTextChanged) {
                // reload the current note if we had to change it during a note rename
                _mainWindow->reloadCurrentNoteByNoteId(true);
            }

            // just to make sure everything is up-to-date
            _mainWindow->currentNote.refetch();

            // create a hash of the text of the current note to be able if it
            // was modified outside of QOwnNotes
            _mainWindow->updateCurrentNoteTextHash();

            _ui->noteTextEdit->setMarkdownLspDocumentPath(
                _mainWindow->currentNote.fullNoteFilePath(), _ui->noteTextEdit->toPlainText());

            if (oldNoteName != _mainWindow->currentNote.getName()) {
                // just to make sure the window title is set correctly
                _mainWindow->updateWindowTitle();

                // update current tab name
                _mainWindow->updateCurrentTabData(_mainWindow->currentNote);
            }
        }

        if (noteWasRenamed) {
            // reload the directory list if note name has changed
            loadNoteDirectoryList();
        }

        _mainWindow->updateNoteGraphicsView();
    }
}

/**
 * Shows alerts for calendar items with an alarm date in the current minute
 * Also checks for expired note crypto keys
 */
void NoteIndexManager::frequentPeriodicChecker() {
    CalendarItem::alertTodoReminders();
    Note::expireCryptoKeys();

    if (QDateTime::currentDateTime().addSecs(-1200) >= _mainWindow->_lastHeartbeat) {
        _mainWindow->_lastHeartbeat = QDateTime::currentDateTime();
        MetricsService::instance()->sendHeartbeat();
    }

    SettingsService settings;
    QDateTime lastUpdateCheck = settings.value(QStringLiteral("LastUpdateCheck")).toDateTime();
    if (!lastUpdateCheck.isValid()) {
        // set the LastUpdateCheck if it wasn't set
        settings.setValue(QStringLiteral("LastUpdateCheck"), QDateTime::currentDateTime());
    } else if (lastUpdateCheck.addSecs(3600) <= QDateTime::currentDateTime()) {
        // check for updates every 1h
        _mainWindow->updateService->checkForUpdates(UpdateService::Periodic);
    }
}

/**
 * Activates or deactivates a workaround for the ill behaving directory watcher
 *
 * @param isNotesDirectoryWasModifiedDisabled
 * @param alsoHandleNotesWereModified
 */
void NoteIndexManager::directoryWatcherWorkaround(bool isNotesDirectoryWasModifiedDisabled,
                                                  bool alsoHandleNotesWereModified) {
    if (!isNotesDirectoryWasModifiedDisabled) {
        Utils::Misc::waitMsecs(200);
    }

    _mainWindow->_isNotesDirectoryWasModifiedDisabled = isNotesDirectoryWasModifiedDisabled;

    if (alsoHandleNotesWereModified) {
        _mainWindow->_isNotesWereModifiedDisabled = isNotesDirectoryWasModifiedDisabled;
    }
}

bool NoteIndexManager::noteDirectoryWatcherAddPath(const QString &path) {
    const bool result = _mainWindow->noteDirectoryWatcher.addPath(path);

    if (!result) {
        qDebug() << "Failed to add path to directory watcher: " << path;
    }

    return result;
}

void NoteIndexManager::addDirectoryToDirectoryWatcher(const QString &path) {
    QDir dir(path);
    QFileInfoList entries =
        dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);

    for (const QFileInfo &entryInfo : entries) {
        QString entryPath = entryInfo.filePath();
        noteDirectoryWatcherAddPath(entryPath);

        if (entryInfo.isDir()) {
            addDirectoryToDirectoryWatcher(entryPath);    // Recursively add subdirectories
        }
    }
}

void NoteIndexManager::connectFileWatcher(bool delayed) {
    if (!delayed) {
        connect(&_mainWindow->noteDirectoryWatcher, &QFileSystemWatcher::directoryChanged, this,
                &NoteIndexManager::notesDirectoryWasModified, Qt::UniqueConnection);
        connect(&_mainWindow->noteDirectoryWatcher, &QFileSystemWatcher::fileChanged, this,
                &NoteIndexManager::notesWereModified, Qt::UniqueConnection);
    } else {
        // In some cases, there are delayed signals coming in which we don't want to handle
        // so reconnect with delay
        QTimer::singleShot(300, this, [this] {
            connect(&_mainWindow->noteDirectoryWatcher, &QFileSystemWatcher::directoryChanged, this,
                    &NoteIndexManager::notesDirectoryWasModified, Qt::UniqueConnection);
            connect(&_mainWindow->noteDirectoryWatcher, &QFileSystemWatcher::fileChanged, this,
                    &NoteIndexManager::notesWereModified, Qt::UniqueConnection);
        });
    }
}
