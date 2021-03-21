#pragma once

#include "masterdialog.h"

class QTreeWidgetItem;

namespace Ui {
class StoredImagesDialog;
}

class StoredImagesDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit StoredImagesDialog(QWidget *parent = 0);
    ~StoredImagesDialog();

   protected:
    bool eventFilter(QObject *obj, QEvent *event);

   private slots:
    void on_fileTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                              QTreeWidgetItem *previous);

    void on_deleteButton_clicked();

    void on_insertButton_clicked();

   private:
    Ui::StoredImagesDialog *ui;

    QString getFilePath(QTreeWidgetItem *item);
};
