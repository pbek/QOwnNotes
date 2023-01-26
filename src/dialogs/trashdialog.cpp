#include "trashdialog.h"

#include <services/owncloudservice.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QDebug>
#include <QJSValue>
#include <QJSValueIterator>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>

#include "mainwindow.h"
#include "ui_trashdialog.h"

TrashDialog::TrashDialog(const QJSValue &notes, QWidget *parent)
    : MasterDialog(parent), ui(new Ui::TrashDialog) {
    ui->setupUi(this);

    // init the note browser search frame
    ui->noteBrowser->initSearchFrame(ui->noteBrowserSearchFrame);

    setupMainSplitter();

    QPushButton *button;
    ui->buttonBox->clear();

    button = new QPushButton(tr("&Restore selected note on server"));
    button->setToolTip(
        Utils::Misc::replaceOwnCloudText(tr("<h3>Slower, but with note versions</h3>"
                                            "<p>The note will be restored on your ownCloud "
                                            "server with all versions.</p>"
                                            "<p>You will have to wait until it is synced to "
                                            "QOwnNotes by ownCloud sync.</p>")));
    button->setProperty("ActionRole", RestoreOnServer);
    button->setDefault(false);
    button->setIcon(QIcon::fromTheme(QStringLiteral("view-restore"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/view-restore.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Download selected note"));
    button->setToolTip(Utils::Misc::replaceOwnCloudText(
        tr("<h3>Faster, but without versions</h3>"
           "<p>The note will be created with the text from the preview.</p>"
           "<p>The note versions on your ownCloud server will not be restored "
           "and the note will remain in the trash.</p>"
           "<p>You can always restore the note and its versions later.</p>")));
    button->setProperty("ActionRole", Download);
    button->setDefault(false);
    button->setIcon(QIcon::fromTheme(QStringLiteral("edit-download"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/edit-download.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Delete"));
    button->setToolTip(tr("Delete selected note on server"));
    button->setProperty("ActionRole", DeleteOnServer);
    button->setDefault(false);
    button->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/edit-delete.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Cancel"));
    button->setProperty("ActionRole", Cancel);
    button->setIcon(QIcon::fromTheme(QStringLiteral("dialog-cancel"),
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
    dataList = new QStringList();
    timestampList = new QList<int>;

    // init the iterator for the versions
    QJSValueIterator notesIterator(notes);

    // iterate over the trashed notes
    while (notesIterator.hasNext()) {
        notesIterator.next();

        QJSValue property = notesIterator.value().property(QStringLiteral("noteName"));

        if (property.isUndefined()) {
            continue;
        }

        itemName = property.toString();

        if (itemName.isEmpty()) {
            continue;
        }

        dateString = notesIterator.value().property(QStringLiteral("dateString")).toString();
        data = notesIterator.value().property(QStringLiteral("data")).toString();
        timestamp = notesIterator.value().property(QStringLiteral("timestamp")).toInt();
        QString fileName = notesIterator.value().property(QStringLiteral("fileName")).toString();

        auto *item = new QListWidgetItem();
        item->setText(itemName);
        item->setData(Qt::UserRole, fileName);
        item->setToolTip(dateString);
        ui->trashListWidget->addItem(item);
        dataList->append(data);
        timestampList->append(timestamp);
    }

    ui->trashListWidget->setCurrentRow(0);
    if (dataList->count() > 0) {
        ui->noteBrowser->setPlainText(dataList->at(0));
    }
}

void TrashDialog::setupMainSplitter() {
    trashSplitter = new QSplitter;

    trashSplitter->addWidget(ui->listFrame);
    trashSplitter->addWidget(ui->noteBrowserFrame);

    // restore splitter sizes
    QSettings settings;
    QByteArray state = settings.value(QStringLiteral("trashSplitterSizes")).toByteArray();
    trashSplitter->restoreState(state);

    ui->gridLayout->layout()->addWidget(trashSplitter);
    ui->gridLayout->layout()->addWidget(ui->buttonBox);
}

void TrashDialog::storeSettings() {
    // store the splitter sizes
    QSettings settings;
    settings.setValue(QStringLiteral("trashSplitterSizes"), trashSplitter->saveState());
}

TrashDialog::~TrashDialog() { delete ui; }

void TrashDialog::on_trashListWidget_currentRowChanged(int currentRow) {
    ui->noteBrowser->setPlainText(dataList->value(currentRow));
}

void TrashDialog::dialogButtonClicked(QAbstractButton *button) {
    if (ui->trashListWidget->currentItem() != nullptr) {
        int actionRole = button->property("ActionRole").toInt();

        QString name = ui->trashListWidget->currentItem()->text();
        QString fileName = ui->trashListWidget->currentItem()->data(Qt::UserRole).toString();

        switch (actionRole) {
            case Download: {
                QString text = dataList->value(ui->trashListWidget->currentRow());
                MainWindow::instance()->createNewNote(name, text);
                break;
            }

            case RestoreOnServer: {
                int timestamp = this->timestampList->value(ui->trashListWidget->currentRow());
                qDebug() << name << timestamp;

                MainWindow::instance()->restoreTrashedNoteOnServer(fileName, timestamp);
                break;
            }

            case DeleteOnServer: {
                if (Utils::Gui::question(this, tr("Delete note on server"),
                                         tr("Delete selected trashed note on server?"),
                                         "trashdialog-delete", QMessageBox::Yes | QMessageBox::No,
                                         QMessageBox::No) == QMessageBox::No) {
                    return;
                }

                const int timestamp = this->timestampList->value(ui->trashListWidget->currentRow());
                auto currentItem = ui->trashListWidget->currentItem();
                OwnCloudService *ownCloud = OwnCloudService::instance();
                ui->trashListWidget->setDisabled(true);
                ui->buttonBox->setDisabled(true);

                // delete trashed note on server
                const int statusCode = ownCloud->deleteTrashedNoteOnServer(fileName, timestamp);

                if (statusCode >= 200 && statusCode < 300) {
                    delete currentItem;
                } else {
                    Utils::Gui::warning(this, tr("Error while deleting note"),
                                        tr("Deleting trashed note failed with status code: %1")
                                            .arg(QString::number(statusCode)),
                                        "trashdialog-delete-failed");
                }

                ui->trashListWidget->setDisabled(false);
                ui->buttonBox->setDisabled(false);

                return;
            }

            default:
                break;
        }
    }

    this->close();
}

void TrashDialog::on_searchLineEdit_textChanged(const QString &arg1) {
    Utils::Gui::searchForTextInListWidget(ui->trashListWidget, arg1);
}
