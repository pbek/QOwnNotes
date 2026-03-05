#pragma once

#include <QTreeWidget>
#include <QVector>

class NoteSubFolder;
class Note;
class QMenu;

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
    static void removeSelectedNoteSubFolders(QTreeWidget *treeWidget);
    static void moveSelectedNoteSubFoldersToParent(QTreeWidget *treeWidget,
                                                   int destinationParentId);

   public Q_SLOTS:
    void onItemExpanded(QTreeWidgetItem *item);

   private Q_SLOTS:
    void onContextMenuRequested(QPoint pos);
    void onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void onItemChanged(QTreeWidgetItem *item, int column);
    void onItemSelectionChanged();

   Q_SIGNALS:
    void currentSubFolderChanged();
    void multipleSubfoldersSelected();

   private:
    void initConnections();
    QTreeWidgetItem *addNoteSubFolder(QTreeWidgetItem *parentItem,
                                      const NoteSubFolder &noteSubFolder);
    static QMenu *buildMoveDestinationMenuTree(QTreeWidget *treeWidget, QMenu *parentMenu,
                                               int parentNoteSubFolderId,
                                               const QVector<int> &forbiddenDestinationIds);

   protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};
