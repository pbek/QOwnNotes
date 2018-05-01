#pragma once

#include <QDialog>
#include <QTreeWidgetItem>
#include <QEvent>
#include "masterdialog.h"

namespace Ui {
class OrphanedImagesDialog;
}

class OrphanedImagesDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit OrphanedImagesDialog(QWidget *parent = 0);
    ~OrphanedImagesDialog();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_fileTreeWidget_currentItemChanged(
            QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_deleteButton_clicked();

    void on_insertButton_clicked();

private:
    Ui::OrphanedImagesDialog *ui;

    QString getFilePath(QTreeWidgetItem *item);
};
