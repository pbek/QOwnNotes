#include "actiondialog.h"
#include <QDebug>
#include "ui_actiondialog.h"

ActionDialog::ActionDialog(QMenuBar* menuBar, QWidget *parent) :
        MasterDialog(parent),
    ui(new Ui::ActionDialog) {
    _menuBar = menuBar;
    ui->setupUi(this);
    ui->actionLineEdit->setFocus();
    ui->actionTreeWidget->clear();

    QList<QMenu *> menuList = menuBar->findChildren<QMenu *>(
            QString(), Qt::FindDirectChildrenOnly);

    Q_FOREACH(QMenu *menu, menuList) {
            buildActionTreeForMenu(menu);
        }

//    ui->actionTreeWidget->setRootIsDecorated(false);
    ui->actionTreeWidget->expandAll();
    ui->actionTreeWidget->resizeColumnToContents(0);
}

/**
 * Builds the action tree
 *
 * @param menu
 * @param parentItem
 */
void ActionDialog::buildActionTreeForMenu(QMenu *menu,
                                          QTreeWidgetItem *parentItem) {
    QTreeWidgetItem *menuItem = new QTreeWidgetItem();
    QString menuText = menu->title().remove("&");

    if (menuText.isEmpty()) {
        return;
    }

    menuItem->setText(0, menuText);

    if (parentItem == Q_NULLPTR) {
        ui->actionTreeWidget->addTopLevelItem(menuItem);
    } else {
        parentItem->addChild(menuItem);
    }

    // search for sub-menus in the menu
    QList<QMenu *> menuList = menu->findChildren<QMenu *>(
            QString(), Qt::FindDirectChildrenOnly);

    // build the tree for that sub-menu
    Q_FOREACH(QMenu *subMenu, menuList) {
            buildActionTreeForMenu(subMenu, menuItem);
        }

    if (parentItem != Q_NULLPTR) {
        // add all actions to the tree widget item
        Q_FOREACH(QAction *action, menu->actions()) {
                QTreeWidgetItem *item = new QTreeWidgetItem();
                QString text = action->text().remove("&");
                QString actionName = action->objectName();

                if (text.isEmpty() || actionName.isEmpty()) {
                    continue;
                }

                QString tooltip = action->toolTip();

                QKeySequence shortcut = action->shortcut();
                if (!shortcut.isEmpty()) {
                    tooltip += " (" + shortcut.toString() + ")";
                }

                item->setData(0, Qt::UserRole, actionName);
                item->setText(0, text);
                item->setToolTip(0, tooltip);
                item->setIcon(0, action->icon());

                menuItem->addChild(item);
            }
    }
}

ActionDialog::~ActionDialog()
{
    delete ui;
}

/**
 * Trigger a double clicked action
 *
 * @param item
 * @param column
 */
void ActionDialog::on_actionTreeWidget_itemDoubleClicked(
        QTreeWidgetItem *item, int column) {
    Q_UNUSED(column);

    if (item == Q_NULLPTR) {
        return;
    }

    QString objectName = item->data(0, Qt::UserRole).toString();

    if (objectName.isEmpty()) {
        return;
    }

    // search for actions in the parent of the menu bar
    // for some reasons not all actions are children of the menu bar
    QList<QAction *> actionList = _menuBar->parent()->findChildren<QAction *>
            (objectName);

    // trigger the action
    if (actionList.count() > 0) {
        actionList[0]->activate(QAction::Trigger);
    }
}
