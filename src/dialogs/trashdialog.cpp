#include "trashdialog.h"
#include "ui_trashdialog.h"
#include <QSettings>
#include <QDebug>
#include <QScriptValueIterator>
#include <QPushButton>

TrashDialog::TrashDialog(QScriptValue notes, MainWindow *mainWindow,
                         QWidget *parent) :
        MasterDialog(parent),
        ui(new Ui::TrashDialog) {
    this->mainWindow = mainWindow;
    ui->setupUi(this);

    setupMainSplitter();

    QPushButton *button;
    ui->buttonBox->clear();

    button = new QPushButton(tr("&Restore selected note on server"));
    button->setToolTip(tr("<h3>Slower, but with note versions</h3>"
          "<p>The note will be restored on your ownCloud "
                                  "server with all versions.</p>"
          "<p>You will have to wait until it is synced to "
                                  "QOwnNotes by ownCloud sync.</p>"));
    button->setProperty("ActionRole", RestoreOnServer);
    button->setDefault(false);
    button->setIcon(
            QIcon::fromTheme(
                    "view-restore",
                    QIcon(":/icons/breeze-qownnotes/16x16/view-restore.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Download selected note"));
    button->setToolTip(tr("<h3>Faster, but without versions</h3>"
          "<p>The note will be created with the text from the preview.</p>"
          "<p>The note versions on your ownCloud server will not be restored "
                                  "and the note will remain in the trash.</p>"
          "<p>You can always restore the note and its versions later.</p>"));
    button->setProperty("ActionRole", Download);
    button->setDefault(false);
    button->setIcon(
            QIcon::fromTheme(
                    "edit-download",
                    QIcon(":/icons/breeze-qownnotes/16x16/edit-download.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Cancel"));
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
    dataList = new QStringList();
    timestampList = new QList<int>;

    // init the iterator for the versions
    QScriptValueIterator notesIterator(notes);

    // iterate over the trashed notes
    while (notesIterator.hasNext()) {
        notesIterator.next();

        itemName = notesIterator.value().property("noteName").toString();
        dateString = notesIterator.value().property("dateString").toString();
        data = notesIterator.value().property("data").toString();
        timestamp = notesIterator.value().property("timestamp").toInt32();
        QString fileName =
                notesIterator.value().property("fileName").toString();

        if (itemName == "") {
            continue;
        }

        QListWidgetItem *item = new QListWidgetItem();
        item->setText(itemName);
        item->setWhatsThis(fileName);
        item->setToolTip(dateString);
        ui->trashListWidget->addItem(item);
        dataList->append(data);
        timestampList->append(timestamp);
    }

    ui->trashListWidget->setCurrentRow(0);
    ui->noteBrowser->setText(dataList->at(0));
}

void TrashDialog::setupMainSplitter() {
    this->trashSplitter = new QSplitter;

    this->trashSplitter->addWidget(ui->trashListWidget);
    this->trashSplitter->addWidget(ui->noteBrowser);

    // restore splitter sizes
    QSettings settings;
    QByteArray state = settings.value("trashSplitterSizes").toByteArray();
    this->trashSplitter->restoreState(state);

    this->ui->gridLayout->layout()->addWidget(this->trashSplitter);
    this->ui->gridLayout->layout()->addWidget(ui->buttonBox);
}

void TrashDialog::storeSettings() {
    // store the splitter sizes
    QSettings settings;
    settings.setValue("trashSplitterSizes", this->trashSplitter->saveState());
}

TrashDialog::~TrashDialog() {
    delete ui;
}

void TrashDialog::on_trashListWidget_currentRowChanged(int currentRow) {
    ui->noteBrowser->setText(dataList->value(currentRow));
}

void TrashDialog::dialogButtonClicked(QAbstractButton *button) {
    int actionRole = button->property("ActionRole").toInt();
    QString name = ui->trashListWidget->currentItem()->text();
    QString fileName = ui->trashListWidget->currentItem()->whatsThis();

    switch (actionRole) {
        case Download: {
            QString text = dataList->value(ui->trashListWidget->currentRow());
            mainWindow->createNewNote(name, text);
            break;
        }

        case RestoreOnServer: {
            int timestamp = this->timestampList->value(
                    ui->trashListWidget->currentRow());
            qDebug() << name << timestamp;

            mainWindow->restoreTrashedNoteOnServer(fileName, timestamp);
            break;
        }

        default:
            break;
    }

    this->close();
}
