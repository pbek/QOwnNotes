#pragma once

#include <QTreeWidget>

class NoteSubFolder;
class Note;

class NoteSubFolderTree : public QTreeWidget {
    Q_OBJECT
   public:
    NoteSubFolderTree(QWidget *parent = nullptr);

    void renameSubFolder(QTreeWidgetItem *item);

    /**
     * Populates the note sub folder tree recursively with its items
     */
    void buildTreeForParentItem(QTreeWidgetItem *parent = nullptr);

    void reload();

    static QMenu *contextMenu(QTreeWidget *parent);

   private Q_SLOTS:
    void onItemExpanded(QTreeWidgetItem *item);
    void onContextMenuRequested(QPoint pos);
    void onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void onItemChanged(QTreeWidgetItem *item, int column);
    void onItemSelectionChanged();

    static void removeSelectedNoteSubFolders(QTreeWidget *treeWidget);

   Q_SIGNALS:
    void currentSubFolderChanged();
    void multipleSubfoldersSelected();

   private:
    void initConnections();
    QTreeWidgetItem *addNoteSubFolder(QTreeWidgetItem *parentItem,
                                      const NoteSubFolder &noteSubFolder);
};
