#pragma once

class QTreeWidgetItem;
class QMenuBar;
class QMenu;
#include "masterdialog.h"

namespace Ui {
class ActionDialog;
}

class ActionDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit ActionDialog(QMenuBar *menuBar, QWidget *parent = 0);
    ~ActionDialog();
    void refreshUi();

   private slots:
    void on_actionTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_actionLineEdit_textChanged(const QString &arg1);

   protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

   private:
    Ui::ActionDialog *ui;
    QMenuBar *_menuBar;

    void buildActionTreeForMenu(QMenu *menu, QTreeWidgetItem *parentItem = nullptr);
};
