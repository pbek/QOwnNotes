#include "notesubfoldertree.h"
#include "entities/notesubfolder.h"
#include "entities/notefolder.h"
#include "mainwindow.h"
#include "utils/gui.h"

#include <QSettings>
#include <QHeaderView>
#include <QMenu>

NoteSubFolderTree::NoteSubFolderTree(QWidget *parent)
    : QTreeWidget(parent)
{
    QTimer::singleShot(1, this, [this](){
        header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    });
    setContextMenuPolicy(Qt::CustomContextMenu);

    initConnections();
}

void NoteSubFolderTree::initConnections()
{
    connect(this, &QTreeWidget::itemExpanded, this, &NoteSubFolderTree::onItemExpanded);
    connect(this, &QTreeWidget::itemCollapsed, this, &NoteSubFolderTree::onItemExpanded);
    connect(this, &QTreeWidget::customContextMenuRequested, this, &NoteSubFolderTree::onContextMenuRequested);
    connect(this, &QTreeWidget::itemChanged, this, &NoteSubFolderTree::onItemChanged);
    connect(this, &QTreeWidget::currentItemChanged, this, &NoteSubFolderTree::onCurrentItemChanged);
    connect(this, &QTreeWidget::itemSelectionChanged, this, &NoteSubFolderTree::onItemSelectionChanged);
}

void NoteSubFolderTree::onItemExpanded(QTreeWidgetItem *item) {
    int noteSubFolderId = item->data(0, Qt::UserRole).toInt();
    NoteSubFolder noteSubFolder = NoteSubFolder::fetch(noteSubFolderId);
    if (noteSubFolder.isFetched()) {
        noteSubFolder.saveTreeWidgetExpandState(item->isExpanded());
    }
}

void NoteSubFolderTree::onContextMenuRequested(QPoint pos)
{
    auto *mainWindow = MainWindow::instance();
    Q_ASSERT(mainWindow);

    QMenu menu;
    menu.addAction(mainWindow->newNoteAction());
    QAction *newAction = menu.addAction(tr("New subfolder"));
    QAction *renameAction(nullptr);
    QAction *removeAction(nullptr);

    if (NoteFolder::currentNoteFolder().getActiveNoteSubFolder().isFetched()) {
        renameAction = menu.addAction(tr("Rename subfolder"));
        removeAction = menu.addAction(tr("Remove selected folders"));
    }

    QAction *showInFileManagerAction =
        menu.addAction(tr("Show folder in file manager"));
    menu.addAction(mainWindow->reloadNoteFolderAction());

    QAction *selectedItem = menu.exec(mapToGlobal(pos));
    if (selectedItem) {
        if (selectedItem == newAction) {
            // create a new folder
            mainWindow->createNewNoteSubFolder();
        } else if (selectedItem == removeAction) {
            // remove folders
            removeSelectedNoteSubFolders();
        } else if (selectedItem == renameAction) {
            QTreeWidgetItem *item = currentItem();

            // rename folder
            editItem(item);
        } else if (selectedItem == showInFileManagerAction) {
            NoteSubFolder noteSubFolder =
                NoteFolder::currentNoteFolder().getActiveNoteSubFolder();

            // show the current folder in the file manager
            Utils::Misc::openPath(noteSubFolder.fullPath());
        }
    }
}

void NoteSubFolderTree::removeSelectedNoteSubFolders() {
    const auto selectedItems = this->selectedItems();
    const int selectedItemsCount = selectedItems.size();

    if (selectedItemsCount == 0) {
        return;
    }

    // gather the folders that are about to be deleted
    QStringList noteSubFolderPathList;
    QVector<NoteSubFolder> noteSubFolderList;
    for (QTreeWidgetItem *item : selectedItems) {
        if (item->data(0, Qt::UserRole + 1).toInt() != MainWindow::NoteTreeWidgetItemType::FolderType) {
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

    if (Utils::Gui::question(
            this, tr("Remove selected folders"),
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
                mainWindow->showStatusBarMessage(tr("Removed note subfolder: %1")
                                         .arg(noteSubFolder.fullPath()));
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

void NoteSubFolderTree::renameSubFolder(QTreeWidgetItem *item)
{
    Q_ASSERT(item);
    onItemChanged(item, 0);
}

void NoteSubFolderTree::onItemChanged(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column)

    if (item == nullptr) {
        return;
    }

    NoteSubFolder noteSubFolder =
        NoteSubFolder::fetch(item->data(0, Qt::UserRole).toInt());
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

void NoteSubFolderTree::onItemSelectionChanged()
{
    if (selectedItems().size() > 1) {
        Q_EMIT multipleSubfoldersSelected();
    }
}
