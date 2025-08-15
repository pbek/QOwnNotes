#include "versiondialog.h"

#include <utils/misc.h>

#include <QDebug>
#include <QJSValue>
#include <QJSValueIterator>
#include <QPushButton>
#include <QSplitter>

#include "mainwindow.h"
#include "services/settingsservice.h"
#include "ui_versiondialog.h"

VersionDialog::VersionDialog(const QJSValue &versions, QWidget *parent)
    : MasterDialog(parent), ui(new Ui::VersionDialog) {
    ui->setupUi(this);
    afterSetupUI();
    setWindowTitle(Utils::Misc::replaceOwnCloudText(windowTitle()));
    ui->tabWidget->setCurrentIndex(0);

    // Init the note text edit search-frame
    ui->noteTextEdit->initSearchFrame(ui->noteTextEditSearchFrame);

    setupMainSplitter();

    QPushButton *button;
    ui->buttonBox->clear();

    button = new QPushButton(tr("&Restore selected version"));
    button->setProperty("ActionRole", Restore);
    button->setDefault(false);
    button->setIcon(QIcon::fromTheme(QStringLiteral("edit-download"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/edit-download.svg")));
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
    QString diffHtml;
    ui->versionListWidget->clear();
    diffList = new QStringList();
    dataList = new QStringList();

    // Init the iterator for the versions
    QJSValueIterator versionsIterator(versions);

    // This seems to report a hasNext even if there aren't any items
    if (!versionsIterator.hasNext()) {
        return;
    }

    // Iterate over the versions
    while (versionsIterator.hasNext()) {
        versionsIterator.next();
        QJSValue property =
            versionsIterator.value().property(QStringLiteral("humanReadableTimestamp"));

        if (property.isUndefined()) {
            continue;
        }

        itemName = property.toString();

        if (itemName.isEmpty()) {
            continue;
        }

        diffHtml = "<style>" + Utils::Misc::genericCSS() + "</style>" +
                   versionsIterator.value().property(QStringLiteral("diffHtml")).toString();
        diffHtml.replace(QLatin1String("\\n"), QLatin1String("&para;<br />"));
        diffHtml.replace(QLatin1String("\n"), QLatin1String("<br />"));

        ui->versionListWidget->addItem(itemName);
        diffList->append(diffHtml);
        dataList->append(versionsIterator.value().property(QStringLiteral("data")).toString());
    }

    ui->versionListWidget->setCurrentRow(0);
    ui->diffBrowser->setHtml(diffList->at(0));
}

void VersionDialog::setupMainSplitter() {
    versionSplitter = new QSplitter;

    versionSplitter->addWidget(ui->versionListWidget);
    versionSplitter->addWidget(ui->tabWidget);

    // restore splitter sizes
    SettingsService settings;
    QByteArray state = settings.value(QStringLiteral("versionSplitterSizes")).toByteArray();
    versionSplitter->restoreState(state);

    ui->gridLayout->addWidget(versionSplitter);
}

void VersionDialog::storeSettings() {
    // store the splitter sizes
    SettingsService settings;
    settings.setValue(QStringLiteral("versionSplitterSizes"), versionSplitter->saveState());
}

VersionDialog::~VersionDialog() { delete ui; }

void VersionDialog::on_versionListWidget_currentRowChanged(int currentRow) {
    ui->diffBrowser->setHtml(diffList->value(currentRow));
    ui->noteTextEdit->setPlainText(dataList->value(currentRow));
}

void VersionDialog::dialogButtonClicked(QAbstractButton *button) {
    int actionRole = button->property("ActionRole").toInt();

    if (actionRole == Restore) {
        MainWindow::instance()->setCurrentNoteText(
            dataList->value(ui->versionListWidget->currentRow()));
    }

    this->close();
}
