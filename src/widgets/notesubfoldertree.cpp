#include "notesubfoldertree.h"

#include <QHeaderView>
#include <QMenu>
#include <QSettings>
#include <memory>

#include "entities/notefolder.h"
#include "entities/notesubfolder.h"
#include "entities/tag.h"
#include "mainwindow.h"
#include "utils/gui.h"

NoteSubFolderTree::NoteSubFolderTree(QWidget *parent) : QTreeWidget(parent) {
    QTimer::singleShot(1, this, [this]() {
        header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    });
    setContextMenuPolicy(Qt::CustomContextMenu);

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
        QSettings settings;
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
    QSettings settings;
    const int linkCount = Note::countByNoteSubFolderId(
        id, settings.value(QStringLiteral("noteSubfoldersPanelShowNotesRecursively")).toBool());
    QString toolTip = tr("show notes in folder '%1' (%2)").arg(name, QString::number(linkCount));

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
    QSettings settings;
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
                    tr("Removed note subfolder: %1").arg(noteSubFolder.fullPath()));
            }
        }

        mainWindow->reloadNoteFolderAction()->trigger();
    }
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
