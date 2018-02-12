#include "localtrashdialog.h"
#include "ui_localtrashdialog.h"
#include <QSettings>
#include <QDebug>
#include <QPushButton>
#include <QDialogButtonBox>
#include <entities/trashitem.h>
#include <utils/gui.h>

LocalTrashDialog::LocalTrashDialog(MainWindow *mainWindow, QWidget *parent) :
        MasterDialog(parent),
        ui(new Ui::LocalTrashDialog) {
    this->mainWindow = mainWindow;
    ui->setupUi(this);

    // init the note browser search frame
    ui->noteBrowser->initSearchFrame(ui->noteBrowserSearchFrame);

    setupMainSplitter();

    QPushButton *button;
    ui->buttonBox->clear();

    button = new QPushButton(tr("Restore"));
    button->setToolTip(tr("Restore selected notes"));
    button->setProperty("ActionRole", Restore);
    button->setDefault(false);
    button->setIcon(
            QIcon::fromTheme(
                    "view-restore",
                    QIcon(":/icons/breeze-qownnotes/16x16/view-restore.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("Remove"));
    button->setToolTip(tr("Remove selected notes"));
    button->setProperty("ActionRole", Remove);
    button->setDefault(false);
    button->setIcon(
            QIcon::fromTheme(
                    "edit-delete",
                    QIcon(":/icons/breeze-qownnotes/16x16/edit-delete.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

//    button = new QPushButton(tr("Cancel"));
//    button->setProperty("ActionRole", Cancel);
//    button->setIcon(
//            QIcon::fromTheme(
//                    "dialog-cancel",
//                    QIcon(":/icons/breeze-qownnotes/16x16/dialog-cancel.svg")));
//    button->setDefault(true);
//    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    connect(this->ui->buttonBox, SIGNAL(clicked(QAbstractButton *)),
            SLOT(dialogButtonClicked(QAbstractButton *)));
    connect(this, SIGNAL(finished(int)), this, SLOT(storeSettings()));

    loadTrashedNotes();
}

/**
 * Loads the trashed notes
 */
void LocalTrashDialog::loadTrashedNotes() {
    QList<TrashItem> trashItems = TrashItem::fetchAll();
    QListIterator<TrashItem> iterator(trashItems);
    ui->trashTreeWidget->clear();
    ui->noteBrowser->clear();

    while (iterator.hasNext()) {
        TrashItem trashItem = iterator.next();

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, trashItem.getFileName());
        item->setText(1, trashItem.getCreated().toString());
        item->setData(0, Qt::UserRole, trashItem.getId());

        QString toolTipText = tr("File will be restored to: %1").arg(
                trashItem.restorationFilePath());
        item->setToolTip(0, toolTipText);
        item->setToolTip(1, toolTipText);

        ui->trashTreeWidget->addTopLevelItem(item);
    }

    ui->trashTreeWidget->sortItems(1, Qt::SortOrder::DescendingOrder);
    ui->trashTreeWidget->resizeColumnToContents(0);
}

void LocalTrashDialog::setupMainSplitter() {
    trashSplitter = new QSplitter;

    trashSplitter->addWidget(ui->trashTreeWidget);
    trashSplitter->addWidget(ui->noteBrowserFrame);

    // restore splitter sizes
    QSettings settings;
    QByteArray state = settings.value("localTrashSplitterSizes").toByteArray();
    trashSplitter->restoreState(state);

    ui->gridLayout->layout()->addWidget(trashSplitter);
    ui->gridLayout->layout()->addWidget(ui->buttonBox);
}

void LocalTrashDialog::storeSettings() {
    // store the splitter sizes
    QSettings settings;
    settings.setValue("localTrashSplitterSizes", trashSplitter->saveState());
}

LocalTrashDialog::~LocalTrashDialog() {
    delete ui;
}

void LocalTrashDialog::dialogButtonClicked(QAbstractButton *button) {
    int actionRole = button->property("ActionRole").toInt();

    switch (actionRole) {
        case Remove: {
            removeSelectedTrashItems();
            break;
        }

        case Restore: {
            restoreSelectedTrashItems();
            break;
        }

        default:
            this->close();
            break;
    }
}

/**
 * Load the note text of the currently selected item
 *
 * @param current
 * @param previous
 */
void LocalTrashDialog::on_trashTreeWidget_currentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous);
    int trashItemId = current->data(0, Qt::UserRole).toInt();
    TrashItem trashItem = TrashItem::fetch(trashItemId);

    QString text = trashItem.loadFileFromDisk();
    ui->noteBrowser->setPlainText(text);
}

/**
 * Restores selected notes
 */
void LocalTrashDialog::restoreSelectedTrashItems() {
    int selectedItemsCount = ui->trashTreeWidget->selectedItems().count();

    if (selectedItemsCount == 0) {
        return;
    }

    if (Utils::Gui::question(
            this,
            tr("Restore selected notes"),
            tr("Restore <strong>%n</strong> selected note(s)?", "",
               selectedItemsCount), "local-trash-restore-notes") ==
            QMessageBox::Yes) {
        const QSignalBlocker blocker(ui->trashTreeWidget);
        Q_UNUSED(blocker);

        int restoreCount = 0;
        Q_FOREACH(QTreeWidgetItem *item,
                  ui->trashTreeWidget->selectedItems()) {
                int id = item->data(0, Qt::UserRole).toInt();
                TrashItem trashItem = TrashItem::fetch(id);
                bool wasRestored = trashItem.restoreFile();
                if (wasRestored) {
                    qDebug() << "Restored note " << trashItem.getFileName();
                    restoreCount++;
                } else {
                    qDebug() << "Note " << trashItem.getFileName() << " "
                            "couldn't be restored!";
                }
            }

        Utils::Gui::information(
                this,
                tr("Notes restored"),
                tr("<strong>%n</strong> note(s) where restored", "",
                   restoreCount), "local-trash-notes-restored");

        if (restoreCount > 0) {
            loadTrashedNotes();
        }
    }
}

/**
 * Remove selected trashed notes
 */
void LocalTrashDialog::removeSelectedTrashItems() {
    int selectedItemsCount = ui->trashTreeWidget->selectedItems().count();

    if (selectedItemsCount == 0) {
        return;
    }

    if (Utils::Gui::question(
            this,
            tr("Remove selected notes"),
            tr("Remove <strong>%n</strong> selected note(s)?", "",
               selectedItemsCount), "local-trash-remove-notes") ==
            QMessageBox::Yes) {
        const QSignalBlocker blocker(ui->trashTreeWidget);
        Q_UNUSED(blocker);

        int removeCount = 0;
        Q_FOREACH(QTreeWidgetItem *item,
                  ui->trashTreeWidget->selectedItems()) {
                int id = item->data(0, Qt::UserRole).toInt();
                TrashItem trashItem = TrashItem::fetch(id);
                bool wasRestored = trashItem.remove(true);
                if (wasRestored) {
                    qDebug() << "Removed note " << trashItem.getFileName();
                    removeCount++;
                } else {
                    qDebug() << "Note " << trashItem.getFileName() << " "
                            "couldn't be removed!";
                }
            }

        Utils::Gui::information(
                this,
                tr("Notes removed"),
                tr("<strong>%n</strong> note(s) where removed", "",
                   removeCount), "local-trash-notes-removed");

        if (removeCount > 0) {
            loadTrashedNotes();
        }
    }
}
