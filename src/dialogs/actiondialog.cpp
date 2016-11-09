#include "actiondialog.h"
#include "ui_actiondialog.h"

ActionDialog::ActionDialog(QMenuBar* menuBar, QWidget *parent) :
        MasterDialog(parent),
    ui(new Ui::ActionDialog)
{
    ui->setupUi(this);
    ui->actionLineEdit->setFocus();
    ui->actionTreeWidget->clear();

    QList<QMenu *> menuList =
            menuBar->findChildren<QMenu *>(QString(),
                                               Qt::FindDirectChildrenOnly);

    Q_FOREACH(QMenu *menu, menuList) {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            QString title = menu->title().remove("&");
            item->setText(0, title);

            ui->actionTreeWidget->addTopLevelItem(item);
        }
}

ActionDialog::~ActionDialog()
{
    delete ui;
}
