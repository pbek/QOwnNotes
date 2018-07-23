#pragma once

#include <QDialog>
#include <QTreeWidgetItem>
#include <QEvent>
#include "masterdialog.h"

namespace Ui {
class OrphanedAttachmentsDialog;
}

class OrphanedAttachmentsDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit OrphanedAttachmentsDialog(QWidget *parent = 0);
    ~OrphanedAttachmentsDialog();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_fileTreeWidget_currentItemChanged(
            QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_deleteButton_clicked();

    void on_insertButton_clicked();

    void on_openFileButton_clicked();

    void on_openFolderButton_clicked();

private:
    Ui::OrphanedAttachmentsDialog *ui;

    QString getFilePath(QTreeWidgetItem *item);
};
