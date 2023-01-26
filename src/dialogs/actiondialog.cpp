#include "actiondialog.h"
// #include <QDebug>
#include <utils/gui.h>

#include <QKeyEvent>
#include <QMenuBar>
#include <QTreeWidgetItem>

#include "ui_actiondialog.h"

ActionDialog::ActionDialog(QMenuBar *menuBar, QWidget *parent)
    : MasterDialog(parent), ui(new Ui::ActionDialog) {
    _menuBar = menuBar;
    ui->setupUi(this);
    refreshUi();
}

/**
 * Refreshes the UI
 */
void ActionDialog::refreshUi() {
    ui->actionLineEdit->setFocus();
    ui->actionLineEdit->installEventFilter(this);
    ui->actionTreeWidget->clear();
    ui->actionTreeWidget->installEventFilter(this);

    QList<QMenu *> menuList =
        _menuBar->findChildren<QMenu *>(QString(), Qt::FindDirectChildrenOnly);

    Q_FOREACH (QMenu *menu, menuList) {
        buildActionTreeForMenu(menu);
    }

    ui->actionTreeWidget->setRootIsDecorated(false);
    ui->actionTreeWidget->expandAll();
    ui->actionTreeWidget->resizeColumnToContents(0);
    ui->actionTreeWidget->resizeColumnToContents(1);
    ui->actionLineEdit->clear();
}

/**
 * Builds the action tree
 *
 * @param menu
 * @param parentItem
 */
void ActionDialog::buildActionTreeForMenu(QMenu *menu, QTreeWidgetItem *parentItem) {
    QString menuText = menu->title().remove(QStringLiteral("&"));

    if (menuText.isEmpty()) {
        return;
    }

    auto *menuItem = new QTreeWidgetItem();
    menuItem->setText(0, menuText);

    if (parentItem == nullptr) {
        ui->actionTreeWidget->addTopLevelItem(menuItem);
    } else {
        parentItem->addChild(menuItem);
    }

    // search for sub-menus in the menu
    QList<QMenu *> menuList = menu->findChildren<QMenu *>(QString(), Qt::FindDirectChildrenOnly);

    // build the tree for that sub-menu
    Q_FOREACH (QMenu *subMenu, menuList) {
        buildActionTreeForMenu(subMenu, menuItem);
    }

    // add all actions to the tree widget item
    Q_FOREACH (QAction *action, menu->actions()) {
        QString text = action->text().remove(QStringLiteral("&"));
        QString actionName = action->objectName();

        if (text.isEmpty() || actionName.isEmpty() || !action->isVisible()) {
            continue;
        }

        auto *item = new QTreeWidgetItem();
        item->setData(0, Qt::UserRole, actionName);
        item->setText(0, text);
        item->setToolTip(0, action->toolTip());
        item->setIcon(0, action->icon());

        QKeySequence shortcut = action->shortcut();

        // add the shortcut in the 2nd column
        if (!shortcut.isEmpty()) {
            item->setText(1, shortcut.toString(QKeySequence::NativeText));
            item->setForeground(1, QColor(Qt::gray));
        }

        menuItem->addChild(item);
    }
}

ActionDialog::~ActionDialog() { delete ui; }

/**
 * Triggers an double clicked action
 *
 * @param item
 * @param column
 */
void ActionDialog::on_actionTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column);

    if (item == nullptr) {
        return;
    }

    QString objectName = item->data(0, Qt::UserRole).toString();

    if (objectName.isEmpty()) {
        return;
    }

    // search for actions in the parent of the menu bar
    // for some reasons not all actions are children of the menu bar
    QList<QAction *> actionList = _menuBar->parent()->findChildren<QAction *>(objectName);

    // close the dialog
    close();

    // trigger the action
    if (actionList.count() > 0) {
        actionList[0]->activate(QAction::Trigger);
    }
}

/**
 * Searches for the typed text
 *
 * @param arg1
 */
void ActionDialog::on_actionLineEdit_textChanged(const QString &arg1) {
    // search for the text
    Utils::Gui::searchForTextInTreeWidget(
        ui->actionTreeWidget, arg1,
        Utils::Gui::TreeWidgetSearchFlags(Utils::Gui::TreeWidgetSearchFlag::TooltipSearch |
                                          Utils::Gui::TreeWidgetSearchFlag::AllColumnsSearch |
                                          Utils::Gui::TreeWidgetSearchFlag::EveryWordSearch));
}

/**
 * Event filters
 *
 * @param obj
 * @param event
 * @return
 */
bool ActionDialog::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (obj == ui->actionLineEdit) {
            // give the action tree widget the focus if the down button was
            // pressed in the line edit
            if (keyEvent->key() == Qt::Key_Down) {
                ui->actionTreeWidget->setFocus();
                return true;
            }

            return false;
        } else if (obj == ui->actionTreeWidget) {
            // trigger the action if return was pressed in the tree widget
            if (keyEvent->key() == Qt::Key_Return) {
                on_actionTreeWidget_itemDoubleClicked(ui->actionTreeWidget->currentItem(), 0);
                return true;
            }

            return false;
        }
    }

    return MasterDialog::eventFilter(obj, event);
}
