#pragma once

#include <QDialog>
#include <QMenuBar>
#include <QTreeWidgetItem>
#include "masterdialog.h"

namespace Ui {
class ActionDialog;
}

class ActionDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit ActionDialog(QMenuBar *menuBar, QWidget *parent = 0);
    ~ActionDialog();

private slots:
    void on_actionTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::ActionDialog *ui;
    QMenuBar *_menuBar;

    void buildActionTreeForMenu(QMenu *menu,
                                QTreeWidgetItem *parentItem = Q_NULLPTR);
};
