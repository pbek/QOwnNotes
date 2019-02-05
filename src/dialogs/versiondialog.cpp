#include "versiondialog.h"
#include "ui_versiondialog.h"
#include <QSettings>
#include <QDebug>
#include <QPushButton>
#include <QJSValue>
#include <QJSValueIterator>
#include <utils/misc.h>

VersionDialog::VersionDialog(QJSValue versions, MainWindow *mainWindow,
                             QWidget *parent) :
        MasterDialog(parent),
        ui(new Ui::VersionDialog) {
    this->mainWindow = mainWindow;
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    // init the note text edit search frame
    ui->noteTextEdit->initSearchFrame(ui->noteTextEditSearchFrame);

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

        diffHtml = "<style>" + Utils::Misc::genericCSS() + "</style>" +
            versionsIterator.value().property("diffHtml").toString();
        diffHtml.replace("\\n", "&para;<br />");
        diffHtml.replace("\n", "<br />");

        ui->versionListWidget->addItem(itemName);
        diffList->append(diffHtml);
        dataList->append(versionsIterator.value().property("data").toString());
    }

    ui->versionListWidget->setCurrentRow(0);
    ui->diffBrowser->setHtml(diffList->at(0));
}

void VersionDialog::setupMainSplitter() {
    versionSplitter = new QSplitter;

    versionSplitter->addWidget(ui->versionListWidget);
    versionSplitter->addWidget(ui->tabWidget);

    // restore splitter sizes
    QSettings settings;
    QByteArray state = settings.value("versionSplitterSizes").toByteArray();
    versionSplitter->restoreState(state);

    ui->gridLayout->addWidget(versionSplitter);
}

void VersionDialog::storeSettings() {
    // store the splitter sizes
    QSettings settings;
    settings.setValue("versionSplitterSizes",
                      versionSplitter->saveState());
}

VersionDialog::~VersionDialog() {
    delete ui;
}

void VersionDialog::on_versionListWidget_currentRowChanged(int currentRow) {
    ui->diffBrowser->setHtml(diffList->value(currentRow));
    ui->noteTextEdit->setPlainText(dataList->value(currentRow));
}

void VersionDialog::dialogButtonClicked(QAbstractButton *button) {
    int actionRole = button->property("ActionRole").toInt();

    if (actionRole == Restore) {
        mainWindow->setCurrentNoteText(
                dataList->value(ui->versionListWidget->currentRow()));
    }

    this->close();
}
