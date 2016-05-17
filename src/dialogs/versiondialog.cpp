#include "versiondialog.h"
#include "ui_versiondialog.h"
#include <QSettings>
#include <QDebug>
#include <QPushButton>
#include <QJSValue>
#include <QJSValueIterator>

VersionDialog::VersionDialog(QJSValue versions, MainWindow *mainWindow,
                             QWidget *parent) :
        MasterDialog(parent),
        ui(new Ui::VersionDialog) {
    this->mainWindow = mainWindow;
    ui->setupUi(this);

    setupMainSplitter();

    QPushButton *button;
    ui->buttonBox->clear();

    button = new QPushButton(tr("&Restore selected version"));
    button->setProperty("ActionRole", Restore);
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
    QString diffHtml;
    ui->versionListWidget->clear();
    diffList = new QStringList();
    dataList = new QStringList();

    // init the iterator for the versions
    QJSValueIterator versionsIterator(versions);

    // this seems to report a has next even if there aren't any items
    if (!versionsIterator.hasNext()) {
        return;
    }

    // iterate over the versions
    while (versionsIterator.hasNext()) {
        versionsIterator.next();
        QJSValue property = versionsIterator.value()
                .property("humanReadableTimestamp");

        if (property.isUndefined()) {
            continue;
        }

        itemName = property.toString();

        if (itemName == "") {
            continue;
        }

        diffHtml = versionsIterator.value().property("diffHtml").toString();
        diffHtml.replace("<ins>",
                         "<span style='background-color: rgb(214, 255, 199)'>");
        diffHtml.replace("</ins>", "</span>");
        diffHtml.replace("<del>",
                         "<span style='background-color: rgb(255, 215, 215)'>");
        diffHtml.replace("</del>", "</span>");
        diffHtml.replace("\n", "<br />");

        ui->versionListWidget->addItem(itemName);
        diffList->append(diffHtml);
        dataList->append(versionsIterator.value().property("data").toString());
    }

    ui->versionListWidget->setCurrentRow(0);
    ui->diffBrowser->setHtml(diffList->at(0));
}

void VersionDialog::setupMainSplitter() {
    this->versionSplitter = new QSplitter;

    this->versionSplitter->addWidget(ui->versionListWidget);
    this->versionSplitter->addWidget(ui->diffBrowser);

    // restore splitter sizes
    QSettings settings;
    QByteArray state = settings.value("versionSplitterSizes").toByteArray();
    this->versionSplitter->restoreState(state);

    this->ui->gridLayout->layout()->addWidget(this->versionSplitter);
    this->ui->gridLayout->layout()->addWidget(ui->buttonBox);
}

void VersionDialog::storeSettings() {
    // store the splitter sizes
    QSettings settings;
    settings.setValue("versionSplitterSizes",
                      this->versionSplitter->saveState());
}

VersionDialog::~VersionDialog() {
    delete ui;
}

void VersionDialog::on_versionListWidget_currentRowChanged(int currentRow) {
    ui->diffBrowser->setHtml(diffList->value(currentRow));
}

void VersionDialog::dialogButtonClicked(QAbstractButton *button) {
    int actionRole = button->property("ActionRole").toInt();

    if (actionRole == Restore) {
        mainWindow->setCurrentNoteText(
                dataList->value(ui->versionListWidget->currentRow()));
    }

    this->close();
}
