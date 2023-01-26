#pragma once

#include "entities/note.h"
#include "masterdialog.h"

class QTreeWidgetItem;

namespace Ui {
class StoredImagesDialog;
}

class StoredImagesDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit StoredImagesDialog(QWidget *parent = nullptr);
    ~StoredImagesDialog() override;

   protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

   private slots:
    void on_fileTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_deleteButton_clicked();

    void on_insertButton_clicked();

    void on_checkBox_toggled(bool checked);

    void on_searchLineEdit_textChanged(const QString &arg1);

    void on_fileTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_noteTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_refreshButton_clicked();

    void on_fileTreeWidget_itemChanged(QTreeWidgetItem *item, int column);

    void on_fileTreeWidget_customContextMenuRequested(const QPoint &pos);

    void on_noteTreeWidget_customContextMenuRequested(const QPoint &pos);

    void on_openFileButton_clicked();

    void on_openFolderButton_clicked();

   private:
    Ui::StoredImagesDialog *ui;
    bool _orphanedImagesOnly = false;
    QHash<QString, QVector<Note>> _fileNoteList;

    static QString getFilePath(QTreeWidgetItem *item);
    void refreshMediaFiles();
    void loadCurrentFileDetails();
    void refreshAndJumpToFileName(const QString &fileName);
    void openCurrentNote();
};
