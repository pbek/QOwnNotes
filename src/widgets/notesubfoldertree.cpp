#include "notesubfoldertree.h"

#include <QFile>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMenu>
#include <QSignalBlocker>
#include <memory>

#include "entities/note.h"
#include "entities/notefolder.h"
#include "entities/notesubfolder.h"
#include "entities/tag.h"
#include "mainwindow.h"
#include "services/settingsservice.h"
#include "utils/gui.h"

NoteSubFolderTree::NoteSubFolderTree(QWidget *parent) : QTreeWidget(parent) {
    QTimer::singleShot(1, this, [this]() {
        header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    });
    setContextMenuPolicy(Qt::CustomContextMenu);
    installEventFilter(this);
    initConnections();
}

void NoteSubFolderTree::initConnections() {
    connect(this, &QTreeWidget::itemExpanded, this, &NoteSubFolderTree::onItemExpanded);
    connect(this, &QTreeWidget::itemCollapsed, this, &NoteSubFolderTree::onItemExpanded);
    connect(this, &QTreeWidget::customContextMenuRequested, this,
            &NoteSubFolderTree::onContextMenuRequested);
    connect(this, &QTreeWidget::itemChanged, this, &NoteSubFolderTree::onItemChanged);
    connect(this, &QTreeWidget::currentItemChanged, this, &NoteSubFolderTree::onCurrentItemChanged);
    connect(this, &QTreeWidget::itemSelectionChanged, this,
            &NoteSubFolderTree::onItemSelectionChanged);
}

static QTreeWidgetItem *noteItem(const Note &note) {
    const QString name = note.getName();
    if (name.isEmpty()) {
        qWarning() << "Unexpected note with no name: " << note.getFileName();
        return nullptr;
    }

    auto *noteItem = new QTreeWidgetItem();
    Utils::Gui::setTreeWidgetItemToolTipForNote(noteItem, note);
    noteItem->setText(0, name);
    noteItem->setData(0, Qt::UserRole, note.getId());
    noteItem->setData(0, Qt::UserRole + 1, MainWindow::NoteType);
    noteItem->setIcon(0, Utils::Gui::noteIcon());

    const bool isEditable = Note::allowDifferentFileName();
    if (isEditable) {
        noteItem->setFlags(noteItem->flags() | Qt::ItemIsEditable);
    }

    const Tag tag = Tag::fetchOneOfNoteWithColor(note);
    if (!tag.isFetched()) {
        Utils::Gui::handleTreeWidgetItemTagColor(noteItem, tag);
    }
    return noteItem;
}

namespace {
struct MovedNoteInfo {
    Note oldNote;
    QString oldFullNotePath;
    QVector<int> backlinkNoteIds;
    QHash<Note, QSet<LinkHit>> linkedNoteHits;
};
}    // namespace

static QVector<MovedNoteInfo> collectMovedNotes(const NoteSubFolder &sourceSubFolder,
                                                const NoteSubFolder &destinationParent) {
    QVector<MovedNoteInfo> movedNotes;
    Q_UNUSED(sourceSubFolder)
    Q_UNUSED(destinationParent)

    const QVector<int> sourceSubFolderIds =
        NoteSubFolder::fetchIdsRecursivelyByParentId(sourceSubFolder.getId());

    for (const int sourceSubFolderId : sourceSubFolderIds) {
        const QVector<Note> notes = Note::fetchAllByNoteSubFolderId(sourceSubFolderId);

        for (auto note : notes) {
            MovedNoteInfo info;
            info.oldNote = note;
            info.oldFullNotePath = note.fullNoteFilePath();
            info.backlinkNoteIds = note.findBacklinkedNoteIds();
            info.linkedNoteHits = note.findLinkedNotes();
            movedNotes << info;
        }
    }

    return movedNotes;
}

static QString relativePathFromNoteToAbsolutePath(const Note &note, const QString &absolutePath) {
    const QDir dir(note.fullNoteFilePath());
    static const QRegularExpression re(QStringLiteral(R"(^\.\.\/)"));
    return dir.relativeFilePath(absolutePath).remove(re);
}

static bool updateIncomingRelativeNoteLinks(const Note &movedNote,
                                            const MovedNoteInfo &movedNoteInfo) {
    bool changedAny = false;

    for (const int backlinkNoteId : movedNoteInfo.backlinkNoteIds) {
        Note backlinkNote = Note::fetch(backlinkNoteId);
        if (!backlinkNote.isFetched()) {
            continue;
        }

        QString text = backlinkNote.getNoteText();
        const QString oldRelativePath =
            relativePathFromNoteToAbsolutePath(backlinkNote, movedNoteInfo.oldFullNotePath);
        const QString newRelativePath =
            Note::urlEncodeNoteUrl(backlinkNote.getFilePathRelativeToNote(movedNote));

        text.replace(QStringLiteral("<") + oldRelativePath + QStringLiteral(">"),
                     QStringLiteral("<") + newRelativePath + QStringLiteral(">"));
        text.replace(QStringLiteral("](") + oldRelativePath + QStringLiteral(")"),
                     QStringLiteral("](") + newRelativePath + QStringLiteral(")"));
        text.replace(QStringLiteral("](") + oldRelativePath + QStringLiteral("#"),
                     QStringLiteral("](") + newRelativePath + QStringLiteral("#"));

        const QString oldRelativePathEncoded = Note::urlEncodeNoteUrl(oldRelativePath);
        text.replace(QStringLiteral("<") + oldRelativePathEncoded + QStringLiteral(">"),
                     QStringLiteral("<") + newRelativePath + QStringLiteral(">"));
        text.replace(QStringLiteral("](") + oldRelativePathEncoded + QStringLiteral(")"),
                     QStringLiteral("](") + newRelativePath + QStringLiteral(")"));
        text.replace(QStringLiteral("](") + oldRelativePathEncoded + QStringLiteral("#"),
                     QStringLiteral("](") + newRelativePath + QStringLiteral("#"));

        if (text != backlinkNote.getNoteText()) {
            backlinkNote.storeNewText(std::move(text));
            changedAny = true;
        }
    }

    return changedAny;
}

static bool updateOutgoingRelativeNoteLinks(Note &movedNote, const MovedNoteInfo &movedNoteInfo) {
    QString noteText = movedNote.getNoteText();
    bool changed = false;

    for (auto it = movedNoteInfo.linkedNoteHits.begin(); it != movedNoteInfo.linkedNoteHits.end();
         ++it) {
        const Note &linkedNote = it.key();
        const QSet<LinkHit> &linkHits = it.value();

        for (const LinkHit &linkHit : linkHits) {
            const QString oldMarkdown = linkHit.markdown;
            const QString linkText = linkHit.text;
            const QString relativeFilePath =
                Note::urlEncodeNoteUrl(movedNote.getFilePathRelativeToNote(linkedNote));
            const QString newMarkdown =
                linkText.isEmpty() ? QStringLiteral("<") + relativeFilePath + QStringLiteral(">")
                                   : QStringLiteral("[") + linkText + QStringLiteral("](") +
                                         relativeFilePath + QStringLiteral(")");

            if (noteText.contains(oldMarkdown)) {
                noteText.replace(oldMarkdown, newMarkdown);
                changed = true;
            }
        }
    }

    if (changed) {
        movedNote.storeNewText(std::move(noteText));
    }

    return changed;
}

static bool updateRelativeAssetLinksAfterMove(Note &movedNote, const MovedNoteInfo &movedNoteInfo,
                                              const QString &assetFolderName) {
    QString noteText = movedNote.getNoteText();
    QFile noteFile(movedNote.fullNoteFilePath());
    if (noteFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        noteText = QString::fromUtf8(noteFile.readAll());
        noteFile.close();
    }

    bool changed = false;

    const QDir oldNoteDir(QFileInfo(movedNoteInfo.oldFullNotePath).dir());
    const QDir newNoteDir(QFileInfo(movedNote.fullNoteFilePath()).dir());

    const auto rewriteTarget = [&](const QString &target) -> QString {
        if (target.startsWith(QStringLiteral("file://"))) {
            return target;
        }

        const int hashPos = target.indexOf(QLatin1Char('#'));
        const int queryPos = target.indexOf(QLatin1Char('?'));
        int suffixPos = -1;
        if ((hashPos >= 0) && (queryPos >= 0)) {
            suffixPos = qMin(hashPos, queryPos);
        } else {
            suffixPos = qMax(hashPos, queryPos);
        }

        QString pathPart = suffixPos >= 0 ? target.left(suffixPos) : target;
        const QString suffix = suffixPos >= 0 ? target.mid(suffixPos) : QString();

        const QString decodedPathPart = Note::urlDecodeNoteUrl(pathPart);
        if (!decodedPathPart.contains(assetFolderName + QLatin1Char('/'))) {
            return target;
        }

        const QString absoluteAssetPath = oldNoteDir.absoluteFilePath(decodedPathPart);
        const QString newRelativePath = newNoteDir.relativeFilePath(absoluteAssetPath);
        const QString encodedRelativePath = Utils::Misc::encodeFilePath(newRelativePath);
        return encodedRelativePath + suffix;
    };

    // Markdown links and image links
    static const QRegularExpression markdownTargetRe(QStringLiteral(R"(\]\(([^)\s]+)\))"));
    auto markdownIterator = markdownTargetRe.globalMatch(noteText);
    while (markdownIterator.hasNext()) {
        const auto match = markdownIterator.next();
        const QString oldTarget = match.captured(1);
        const QString newTarget = rewriteTarget(oldTarget);
        if (newTarget != oldTarget) {
            noteText.replace(QStringLiteral("](") + oldTarget + QStringLiteral(")"),
                             QStringLiteral("](") + newTarget + QStringLiteral(")"));
            changed = true;
        }
    }

    // Autolinks like <../../media/foo.png>
    static const QRegularExpression autoLinkRe(QStringLiteral(R"(<([^>\s]+)>)"));
    auto autoLinkIterator = autoLinkRe.globalMatch(noteText);
    while (autoLinkIterator.hasNext()) {
        const auto match = autoLinkIterator.next();
        const QString oldTarget = match.captured(1);
        const QString newTarget = rewriteTarget(oldTarget);
        if (newTarget != oldTarget) {
            noteText.replace(QStringLiteral("<") + oldTarget + QStringLiteral(">"),
                             QStringLiteral("<") + newTarget + QStringLiteral(">"));
            changed = true;
        }
    }

    if (changed) {
        movedNote.storeNewText(std::move(noteText));
    }

    return changed;
}

static bool hasSelectedAncestor(const NoteSubFolder &noteSubFolder,
                                const QVector<int> &selectedFolderIds) {
    int parentId = noteSubFolder.getParentId();

    while (parentId > 0) {
        if (selectedFolderIds.contains(parentId)) {
            return true;
        }

        const NoteSubFolder parent = NoteSubFolder::fetch(parentId);
        if (!parent.isFetched()) {
            break;
        }

        parentId = parent.getParentId();
    }

    return false;
}

void NoteSubFolderTree::buildTreeForParentItem(QTreeWidgetItem *parent) {
    const int parentId = parent == nullptr ? 0 : parent->data(0, Qt::UserRole).toInt();
    const int activeNoteSubFolderId = NoteSubFolder::activeNoteSubFolderId();
    const bool isCurrentNoteTreeEnabled = NoteFolder::isCurrentNoteTreeEnabled();

    const auto noteSubFolderList = NoteSubFolder::fetchAllByParentId(parentId);

    // build the next level of note sub folders
    for (const auto &noteSubFolder : noteSubFolderList) {
        QTreeWidgetItem *item = addNoteSubFolder(parent, noteSubFolder);

        if (isCurrentNoteTreeEnabled) {
            // load all notes of the subfolder and add them to the note list
            // widget
            const QVector<Note> noteList = Note::fetchAllByNoteSubFolderId(noteSubFolder.getId());
            QList<QTreeWidgetItem *> noteItems;
            noteItems.reserve(noteList.size());
            for (const auto &note : noteList) {
                noteItems << noteItem(note);
            }
            item->addChildren(noteItems);
        } else {
            // set the active item
            if (activeNoteSubFolderId == noteSubFolder.getId()) {
                const QSignalBlocker blocker(this);
                setCurrentItem(item);
            }
        }

        buildTreeForParentItem(item);

        // set the expanded state
        const bool isExpanded = noteSubFolder.treeWidgetExpandState();
        item->setExpanded(isExpanded);

        // sort alphabetically, if necessary
        SettingsService settings;
        const int sort = settings.value(QStringLiteral("noteSubfoldersPanelSort")).toInt();
        if (sort == SORT_ALPHABETICAL) {
            item->sortChildren(
                0, Utils::Gui::toQtOrder(
                       settings.value(QStringLiteral("noteSubfoldersPanelOrder")).toInt()));
        }
    }

    // add the notes of the note folder root
    if (parentId == 0 && isCurrentNoteTreeEnabled) {
        const QVector<Note> noteList = Note::fetchAllByNoteSubFolderId(0);
        QList<QTreeWidgetItem *> noteItems;
        noteItems.reserve(noteList.size());
        for (const auto &note : noteList) {
            noteItems << noteItem(note);
        }
        if (!parent) {
            qWarning() << "Unexpected null parent when adding notes to notesubfoldertree";
            return;
        } else {
            parent->addChildren(noteItems);
        }
    }
}

QTreeWidgetItem *NoteSubFolderTree::addNoteSubFolder(QTreeWidgetItem *parentItem,
                                                     const NoteSubFolder &noteSubFolder) {
    const int id = noteSubFolder.getId();
    const QString name = noteSubFolder.getName();
    SettingsService settings;
    const int linkCount = Note::countByNoteSubFolderId(
        id, settings.value(QStringLiteral("noteSubfoldersPanelShowNotesRecursively")).toBool());
    QString toolTip = tr("Show notes in folder '%1' (%2)").arg(name, QString::number(linkCount));

#ifdef QT_DEBUG
    toolTip += QStringLiteral("<br />id: %1").arg(noteSubFolder.getId());
#endif

    auto *item = new QTreeWidgetItem();
    item->setText(0, name);
    item->setData(0, Qt::UserRole, id);
    item->setData(0, Qt::UserRole + 1, MainWindow::FolderType);
    item->setToolTip(0, toolTip);
    item->setIcon(0, Utils::Gui::folderIcon());
    item->setForeground(1, QColor(Qt::gray));
    item->setText(1, QString::number(linkCount));
    item->setToolTip(1, toolTip);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    if (parentItem == nullptr) {
        addTopLevelItem(item);
    } else {
        parentItem->addChild(item);
    }

    return item;
}

void NoteSubFolderTree::reload() {
    qDebug() << __func__;
    clear();
    const int activeNoteSubFolderId = NoteSubFolder::activeNoteSubFolderId();
    const bool showAllNotesItem = !NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively();
    auto *allItem = new QTreeWidgetItem();

    // add the "all notes" item
    if (showAllNotesItem) {
        int linkCount = Note::countAll();
        QString toolTip =
            tr("Show notes from all note subfolders (%1)").arg(QString::number(linkCount));

        allItem->setText(0, tr("All notes"));
        allItem->setData(0, Qt::UserRole, -1);
        allItem->setToolTip(0, toolTip);
        allItem->setIcon(
            0,
            QIcon::fromTheme(QStringLiteral("edit-copy"),
                             QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/edit-copy.svg"))));
        allItem->setForeground(1, QColor(Qt::gray));
        allItem->setText(1, QString::number(linkCount));
        allItem->setToolTip(1, toolTip);
        allItem->setFlags(allItem->flags() & ~Qt::ItemIsSelectable);
    }

    // add the "note folder" item
    SettingsService settings;
    const int linkCount = Note::countByNoteSubFolderId(
        0, settings.value(QStringLiteral("noteSubfoldersPanelShowNotesRecursively")).toBool());
    const QString toolTip =
        tr("Show notes in note root folder (%1)").arg(QString::number(linkCount));

    auto *item = new QTreeWidgetItem();
    if (settings.value(QStringLiteral("noteSubfoldersPanelShowRootFolderName")).toBool()) {
        item->setText(
            0, NoteFolder::currentRootFolderName(
                   settings.value(QStringLiteral("noteSubfoldersPanelShowFullPath")).toBool()));
    } else {
        item->setText(0, tr("Note folder"));
    }
    item->setData(0, Qt::UserRole, 0);
    item->setToolTip(0, toolTip);
    item->setIcon(0, Utils::Gui::folderIcon());
    item->setForeground(1, QColor(Qt::gray));
    item->setText(1, QString::number(linkCount));
    item->setToolTip(1, toolTip);

    if (settings.value(QStringLiteral("noteSubfoldersPanelDisplayAsFullTree")).toBool()) {
        if (showAllNotesItem) {
            // add 'All Notes'
            addTopLevelItem(allItem);
        }

        // add note root folder
        addTopLevelItem(item);

        // add all note sub folders recursively as items
        buildTreeForParentItem(item);

        if (item->childCount() > 0) {
            item->setExpanded(true);

            if (settings.value(QStringLiteral("noteSubfoldersPanelSort")).toInt() ==
                SORT_ALPHABETICAL) {
                item->sortChildren(
                    0, Utils::Gui::toQtOrder(
                           settings.value(QStringLiteral("noteSubfoldersPanelOrder")).toInt()));
            }
        }
    } else {    // the less hierarchical view
        // add root note folder first
        addTopLevelItem(item);
        // add subfolders recursively
        buildTreeForParentItem();

        // sort the widget
        if (settings.value(QStringLiteral("noteSubfoldersPanelSort")).toInt() ==
            SORT_ALPHABETICAL) {
            sortItems(0, Utils::Gui::toQtOrder(
                             settings.value(QStringLiteral("noteSubfoldersPanelOrder")).toInt()));
        }

        if (showAllNotesItem) {
            // finally, add 'All Notes' to the top
            insertTopLevelItem(0, allItem);
        }
    }

    // set the active item
    if (activeNoteSubFolderId == 0) {
        const QSignalBlocker blocker(this);
        auto *mw = MainWindow::instance();
        if (mw->showNotesFromAllNoteSubFolders() && allItem)
            setCurrentItem(allItem);
        else
            setCurrentItem(item);
    }
}

void NoteSubFolderTree::onItemExpanded(QTreeWidgetItem *item) {
    int noteSubFolderId = item->data(0, Qt::UserRole).toInt();
    NoteSubFolder noteSubFolder = NoteSubFolder::fetch(noteSubFolderId);
    if (noteSubFolder.isFetched()) {
        noteSubFolder.saveTreeWidgetExpandState(item->isExpanded());
    }
}

QMenu *NoteSubFolderTree::contextMenu(QTreeWidget *parent) {
    QMenu *menu = new QMenu(parent);

    auto *mainWindow = MainWindow::instance();
    Q_ASSERT(mainWindow);

    menu->addAction(mainWindow->newNoteAction());

    QAction *newAction = menu->addAction(tr("New subfolder"));
    connect(newAction, &QAction::triggered, parent,
            []() { MainWindow::instance()->createNewNoteSubFolder(); });

    QAction *renameAction(nullptr);
    QAction *removeAction(nullptr);
    QMenu *moveMenu(nullptr);

    int selectedFolderCount = 0;
    QVector<int> forbiddenDestinationIds;
    const auto selectedItems = parent->selectedItems();
    for (auto *item : selectedItems) {
        if (item->data(0, Qt::UserRole + 1).toInt() != MainWindow::FolderType) {
            continue;
        }

        const int noteSubFolderId = item->data(0, Qt::UserRole).toInt();
        if (noteSubFolderId <= 0) {
            continue;
        }

        selectedFolderCount++;
        const QVector<int> subTreeIds =
            NoteSubFolder::fetchIdsRecursivelyByParentId(noteSubFolderId);
        for (const int id : subTreeIds) {
            if (!forbiddenDestinationIds.contains(id)) {
                forbiddenDestinationIds << id;
            }
        }
    }

    if (NoteFolder::currentNoteFolder().getActiveNoteSubFolder().isFetched()) {
        renameAction = menu->addAction(tr("Rename subfolder"));
        connect(renameAction, &QAction::triggered, [parent]() {
            QTreeWidgetItem *item = parent->currentItem();
            // rename folder
            parent->editItem(item);
        });

        removeAction = menu->addAction(tr("Remove selected folders"));
        connect(removeAction, &QAction::triggered, parent, [parent]() {
            // remove folders
            removeSelectedNoteSubFolders(parent);
        });

        if (selectedFolderCount > 0) {
            moveMenu = menu->addMenu(tr("Move selected folders to..."));

            QAction *moveToRootAction = moveMenu->addAction(tr("Move to note folder"));
            connect(moveToRootAction, &QAction::triggered, parent,
                    [parent]() { moveSelectedNoteSubFoldersToParent(parent, 0); });

            moveMenu->addSeparator();
            buildMoveDestinationMenuTree(parent, moveMenu, 0, forbiddenDestinationIds);
        }
    }

    QAction *showInFileManagerAction = menu->addAction(tr("Show folder in file manager"));
    connect(showInFileManagerAction, &QAction::triggered, parent, []() {
        NoteSubFolder noteSubFolder = NoteFolder::currentNoteFolder().getActiveNoteSubFolder();

        // show the current folder in the file manager
        Utils::Misc::openPath(noteSubFolder.fullPath());
    });

    menu->addAction(mainWindow->reloadNoteFolderAction());

    return menu;
}

QMenu *NoteSubFolderTree::buildMoveDestinationMenuTree(
    QTreeWidget *treeWidget, QMenu *parentMenu, int parentNoteSubFolderId,
    const QVector<int> &forbiddenDestinationIds) {
    const QVector<NoteSubFolder> noteSubFolderList =
        NoteSubFolder::fetchAllByParentId(parentNoteSubFolderId, QStringLiteral("name ASC"));

    for (const auto &noteSubFolder : noteSubFolderList) {
        const int noteSubFolderId = noteSubFolder.getId();
        if (forbiddenDestinationIds.contains(noteSubFolderId)) {
            continue;
        }

        QMenu *noteSubFolderMenu = parentMenu->addMenu(noteSubFolder.getName());
        buildMoveDestinationMenuTree(treeWidget, noteSubFolderMenu, noteSubFolderId,
                                     forbiddenDestinationIds);

        noteSubFolderMenu->addSeparator();
        QAction *moveAction = noteSubFolderMenu->addAction(tr("Move to this subfolder"));
        connect(moveAction, &QAction::triggered, treeWidget, [treeWidget, noteSubFolderId]() {
            moveSelectedNoteSubFoldersToParent(treeWidget, noteSubFolderId);
        });
    }

    return parentMenu;
}

void NoteSubFolderTree::onContextMenuRequested(QPoint pos) {
    std::unique_ptr<QMenu> menu(contextMenu(this));
    menu->exec(mapToGlobal(pos));
}

void NoteSubFolderTree::removeSelectedNoteSubFolders(QTreeWidget *parent) {
    const auto selectedItems = parent->selectedItems();
    const int selectedItemsCount = selectedItems.size();

    if (selectedItemsCount == 0) {
        return;
    }

    // gather the folders that are about to be deleted
    QStringList noteSubFolderPathList;
    QVector<NoteSubFolder> noteSubFolderList;
    for (QTreeWidgetItem *item : selectedItems) {
        if (item->data(0, Qt::UserRole + 1).toInt() !=
            MainWindow::NoteTreeWidgetItemType::FolderType) {
            continue;
        }

        int id = item->data(0, Qt::UserRole).toInt();
        const NoteSubFolder noteSubFolder = NoteSubFolder::fetch(id);
        if (noteSubFolder.isFetched()) {
            noteSubFolderList << noteSubFolder;
            noteSubFolderPathList << noteSubFolder.fullPath();
        }
    }

    if (noteSubFolderList.count() == 0) {
        return;
    }

    auto *mainWindow = MainWindow::instance();

    if (Utils::Gui::question(parent, tr("Remove selected folders"),
                             tr("Remove <strong>%n</strong> selected folder(s)?"
                                "<ul><li>%1</li></ul>"
                                "All files and folders in these folders will be removed as"
                                " well!",
                                "", selectedItemsCount)
                                 .arg(noteSubFolderPathList.join(QStringLiteral("</li><li>"))),
                             QStringLiteral("remove-folders")) == QMessageBox::Yes) {
        // delete the note subfolders
        for (const auto &noteSubFolder : Utils::asConst(noteSubFolderList)) {
            // remove the directory recursively from the file system
            if (noteSubFolder.removeFromFileSystem()) {
                mainWindow->showStatusBarMessage(
                    tr("Removed note subfolder: %1").arg(noteSubFolder.fullPath()),
                    QStringLiteral("📁"));
            }
        }

        mainWindow->reloadNoteFolderAction()->trigger();
    }
}

void NoteSubFolderTree::moveSelectedNoteSubFoldersToParent(QTreeWidget *treeWidget,
                                                           int destinationParentId) {
    const auto selectedItems = treeWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        return;
    }

    const NoteSubFolder destinationParent =
        destinationParentId == 0 ? NoteSubFolder() : NoteSubFolder::fetch(destinationParentId);
    if ((destinationParentId != 0) && !destinationParent.isFetched()) {
        return;
    }

    QVector<NoteSubFolder> noteSubFolderList;
    for (auto *item : selectedItems) {
        if (item->data(0, Qt::UserRole + 1).toInt() != MainWindow::FolderType) {
            continue;
        }

        const int id = item->data(0, Qt::UserRole).toInt();
        if (id <= 0) {
            continue;
        }

        const NoteSubFolder noteSubFolder = NoteSubFolder::fetch(id);
        if (!noteSubFolder.isFetched()) {
            continue;
        }

        if (noteSubFolder.getParentId() == destinationParentId) {
            continue;
        }

        if (noteSubFolder.getId() == destinationParentId) {
            continue;
        }

        const QVector<int> descendantIds =
            NoteSubFolder::fetchIdsRecursivelyByParentId(noteSubFolder.getId());
        if (descendantIds.contains(destinationParentId)) {
            continue;
        }

        const NoteSubFolder conflictingSubFolder =
            NoteSubFolder::fetchByNameAndParentId(noteSubFolder.getName(), destinationParentId);
        if (conflictingSubFolder.isFetched()) {
            qWarning() << __func__ << "Could not move subfolder, destination already contains"
                       << noteSubFolder.getName();
            continue;
        }

        noteSubFolderList << noteSubFolder;
    }

    if (noteSubFolderList.isEmpty()) {
        return;
    }

    QVector<int> selectedFolderIds;
    for (const auto &noteSubFolder : noteSubFolderList) {
        selectedFolderIds << noteSubFolder.getId();
    }
    QVector<NoteSubFolder> filteredSubFolderList;
    for (const auto &noteSubFolder : noteSubFolderList) {
        if (!hasSelectedAncestor(noteSubFolder, selectedFolderIds)) {
            filteredSubFolderList << noteSubFolder;
        }
    }

    noteSubFolderList = filteredSubFolderList;

    if (noteSubFolderList.isEmpty()) {
        return;
    }

    const QString destinationName =
        destinationParentId == 0 ? tr("note folder") : destinationParent.getName();

    if (Utils::Gui::question(treeWidget, tr("Move selected folders"),
                             tr("Move <strong>%n</strong> selected folder(s) to "
                                "<strong>%1</strong>?",
                                "", noteSubFolderList.size())
                                 .arg(destinationName),
                             QStringLiteral("move-folders")) != QMessageBox::Yes) {
        return;
    }

    const bool migrateRelativeLinks =
        Utils::Gui::question(treeWidget, tr("Migrate relative links"),
                             tr("Do you want to migrate relative note links, media file links "
                                "and attachment links for moved notes?<br /><br />"
                                "If you choose <strong>No</strong>, those relative links may "
                                "break after moving the folders."),
                             QStringLiteral("move-folders-migrate-relative-links")) ==
        QMessageBox::Yes;

    auto *mainWindow = MainWindow::instance();
    Q_ASSERT(mainWindow);

    mainWindow->storeUpdatedNotesToDisk();

    const QString destinationParentPath =
        destinationParentId == 0 ? NoteFolder::currentLocalPath() : destinationParent.fullPath();
    int movedCount = 0;
    bool forceReload = false;

    for (const auto &noteSubFolder : noteSubFolderList) {
        const QVector<MovedNoteInfo> movedNotes =
            migrateRelativeLinks ? collectMovedNotes(noteSubFolder, destinationParent)
                                 : QVector<MovedNoteInfo>{};
        const QString oldRelativePath = noteSubFolder.relativePath();
        const QString newRelativePath =
            destinationParentId == 0
                ? noteSubFolder.getName()
                : destinationParent.relativePath() + QLatin1Char('/') + noteSubFolder.getName();

        const QString oldPath = noteSubFolder.fullPath();
        const QString newPath = destinationParentPath + QLatin1Char('/') + noteSubFolder.getName();
        const bool moved = QDir().rename(oldPath, newPath);
        if (!moved) {
            qWarning() << __func__ << "Could not move subfolder:" << oldPath << "to" << newPath;
            continue;
        }

        // update the parent relation in the DB so note paths resolve to the new location
        auto movedSubFolder = NoteSubFolder::fetch(noteSubFolder.getId());
        if (movedSubFolder.isFetched()) {
            movedSubFolder.setParentId(destinationParentId);
            if (!movedSubFolder.store()) {
                qWarning() << __func__ << "Could not update parent relation of moved subfolder:"
                           << movedSubFolder.getId();
            }
        }

        Tag::renameNoteSubFolderPathsOfLinks(oldRelativePath, newRelativePath);

        movedCount++;
        if (migrateRelativeLinks) {
            for (const auto &movedNoteInfo : movedNotes) {
                Note movedNote = Note::fetch(movedNoteInfo.oldNote.getId());
                if (!movedNote.isFetched()) {
                    qWarning() << __func__ << "Moved note couldn't be fetched by id:"
                               << movedNoteInfo.oldNote.getId();
                    continue;
                }

                const bool relativeNoteLinksUpdated =
                    updateIncomingRelativeNoteLinks(movedNote, movedNoteInfo) ||
                    updateOutgoingRelativeNoteLinks(movedNote, movedNoteInfo) ||
                    movedNote.handleNoteMoving(movedNoteInfo.oldNote);
                if (relativeNoteLinksUpdated) {
                    forceReload = true;
                }

                const bool relativeAssetLinksUpdated =
                    updateRelativeAssetLinksAfterMove(movedNote, movedNoteInfo,
                                                      QStringLiteral("media")) ||
                    updateRelativeAssetLinksAfterMove(movedNote, movedNoteInfo,
                                                      QStringLiteral("attachments"));
                if (relativeAssetLinksUpdated) {
                    forceReload = true;
                }

                const bool mediaFileLinksUpdated = movedNote.updateRelativeMediaFileLinks();
                const bool attachmentFileLinksUpdated =
                    movedNote.updateRelativeAttachmentFileLinks();
                if (relativeAssetLinksUpdated || mediaFileLinksUpdated ||
                    attachmentFileLinksUpdated) {
                    movedNote.storeNoteTextFileToDisk();
                }
            }
        }
    }

    if (movedCount == 0) {
        return;
    }

    mainWindow->showStatusBarMessage(
        tr("Moved <strong>%n</strong> note subfolder(s) to <strong>%1</strong>", "", movedCount)
            .arg(destinationName),
        QStringLiteral("📁"));

    // make sure moved folders and moved note links are fully refreshed
    mainWindow->buildNotesIndexAndLoadNoteDirectoryList(true, forceReload);
}

void NoteSubFolderTree::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *) {
    if (current == nullptr) {
        return;
    }

    auto mw = MainWindow::instance();

    // set the note sub folder id as active
    const int noteSubFolderId = current->data(0, Qt::UserRole).toInt();
    const bool showNotesFromAllNoteSubFolders = noteSubFolderId == -1;

    NoteSubFolder::setAsActive(showNotesFromAllNoteSubFolders ? 0 : noteSubFolderId);

    const auto items = this->selectedItems();
    // if multiple items are selected, we just set the last selected
    // as active and return;
    if (items.count() > 1) {
        return;
    }

    mw->setShowNotesFromAllNoteSubFolders(showNotesFromAllNoteSubFolders);

    qDebug() << "Current sub folder changed";
    Q_EMIT currentSubFolderChanged();
}

void NoteSubFolderTree::renameSubFolder(QTreeWidgetItem *item) {
    Q_ASSERT(item);
    onItemChanged(item, 0);
}

void NoteSubFolderTree::onItemChanged(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column)

    if (item == nullptr) {
        return;
    }

    NoteSubFolder noteSubFolder = NoteSubFolder::fetch(item->data(0, Qt::UserRole).toInt());
    if (noteSubFolder.isFetched()) {
        const QString name = item->text(0);

#ifdef Q_OS_WIN32
        auto *mw = MainWindow::instance();
        // clear all paths from the directory watcher, if a subfolder of the
        // folder to rename is watched Windows will block the renaming
        mw->clearNoteDirectoryWatcher();
#endif

        // rename the note subfolder in the file system
        noteSubFolder.rename(name);

        // for some reason the app crashes if we are using this with a note tree
        if (!NoteFolder::isCurrentNoteTreeEnabled()) {
            // reload tags, note subfolder and notes
            MainWindow::instance()->reloadNoteFolderAction()->trigger();
        }
#ifdef Q_OS_WIN32
        else {
            // update the noteDirectoryWatcher if the note folder wasn't reloaded
            mw->updateNoteDirectoryWatcher();
        }
#endif
    }
}

void NoteSubFolderTree::onItemSelectionChanged() {
    if (selectedItems().size() > 1) {
        Q_EMIT multipleSubfoldersSelected();
    }
}

bool NoteSubFolderTree::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = dynamic_cast<QKeyEvent *>(event);

        if ((keyEvent->key() == Qt::Key_Delete) || (keyEvent->key() == Qt::Key_Backspace)) {
            removeSelectedNoteSubFolders(this);
            return true;
        }
    }

    return QTreeWidget::eventFilter(obj, event);
}
