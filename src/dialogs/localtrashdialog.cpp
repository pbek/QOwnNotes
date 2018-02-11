#include "localtrashdialog.h"
#include "ui_localtrashdialog.h"
#include <QSettings>
#include <QDebug>
#include <QPushButton>
#include <QDialogButtonBox>
#include <entities/trashitem.h>

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

    button = new QPushButton(tr("Cancel"));
    button->setProperty("ActionRole", Cancel);
    button->setIcon(
            QIcon::fromTheme(
                    "dialog-cancel",
                    QIcon(":/icons/breeze-qownnotes/16x16/dialog-cancel.svg")));
    button->setDefault(true);
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    connect(this->ui->buttonBox, SIGNAL(clicked(QAbstractButton *)),
            SLOT(dialogButtonClicked(QAbstractButton *)));
    connect(this, SIGNAL(finished(int)), this, SLOT(storeSettings()));

    QString itemName;
    QString dateString;
    QString data;
    int timestamp;
    ui->trashListWidget->clear();

    loadTrashedNotes();

    dataList = new QStringList();
    timestampList = new QList<int>;

//    // init the iterator for the versions
//    QJSValueIterator notesIterator(notes);
//
//    // iterate over the trashed notes
//    while (notesIterator.hasNext()) {
//        notesIterator.next();
//
//        QJSValue property = notesIterator.value().property("noteName");
//
//        if (property.isUndefined()) {
//            continue;
//        }
//
//        itemName = property.toString();
//
//        if (itemName == "") {
//            continue;
//        }
//
//        dateString = notesIterator.value().property("dateString").toString();
//        data = notesIterator.value().property("data").toString();
//        timestamp = notesIterator.value().property("timestamp").toInt();
//        QString fileName =
//                notesIterator.value().property("fileName").toString();
//
//        QListWidgetItem *item = new QListWidgetItem();
//        item->setText(itemName);
//        item->setData(Qt::UserRole, fileName);
//        item->setToolTip(dateString);
//        ui->trashListWidget->addItem(item);
//        dataList->append(data);
//        timestampList->append(timestamp);
//    }

    ui->trashListWidget->setCurrentRow(0);
    if (dataList->count() > 0) {
        ui->noteBrowser->setPlainText(dataList->at(0));
    }
}

void LocalTrashDialog::loadTrashedNotes() {
    QList<TrashItem> trashItems = TrashItem::fetchAll();
    QListIterator<TrashItem> iterator(trashItems);

    while (iterator.hasNext()) {
        TrashItem trashItem = iterator.next();

        QListWidgetItem *item = new QListWidgetItem();
        item->setText(trashItem.getFileName());
        item->setData(Qt::UserRole, trashItem.getId());
//        item->setToolTip(dateString);
        ui->trashListWidget->addItem(item);
    }
}
void LocalTrashDialog::setupMainSplitter() {
    trashSplitter = new QSplitter;

    trashSplitter->addWidget(ui->trashListWidget);
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

void LocalTrashDialog::on_trashListWidget_currentRowChanged(int currentRow) {
    ui->noteBrowser->setPlainText(dataList->value(currentRow));
}

void LocalTrashDialog::dialogButtonClicked(QAbstractButton *button) {
    if (ui->trashListWidget->currentItem() != Q_NULLPTR) {
        int actionRole = button->property("ActionRole").toInt();

        QString name = ui->trashListWidget->currentItem()->text();
        QString fileName = ui->trashListWidget->currentItem()
                ->data(Qt::UserRole).toString();

        switch (actionRole) {
            case Remove: {
                QString text = dataList->value(
                        ui->trashListWidget->currentRow());
//                mainWindow->createNewNote(name, text);
                break;
            }

            case Restore: {
                int timestamp = this->timestampList->value(
                        ui->trashListWidget->currentRow());
                qDebug() << name << timestamp;

//                mainWindow->restoreTrashedNoteOnServer(fileName, timestamp);
                break;
            }

            default:
                break;
        }
    }

    this->close();
}
