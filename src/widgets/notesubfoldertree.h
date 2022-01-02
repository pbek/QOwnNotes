#pragma once

#include <QTreeWidget>

class NoteSubFolderTree : public QTreeWidget {
    Q_OBJECT
public:
    NoteSubFolderTree(QWidget *parent = nullptr);

    void renameSubFolder(QTreeWidgetItem *item);

private Q_SLOTS:
    void onItemExpanded(QTreeWidgetItem *item);
    void onContextMenuRequested(QPoint pos);
    void removeSelectedNoteSubFolders();
    void onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void onItemChanged(QTreeWidgetItem *item, int column);
    void onItemSelectionChanged();

Q_SIGNALS:
    void currentSubFolderChanged();
    void multipleSubfoldersSelected();

private:
    void initConnections();
};
