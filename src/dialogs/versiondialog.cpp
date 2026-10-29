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
    setIgnoreReturnKey(true);
    ui->tabWidget->setCurrentIndex(
        SettingsService().value(QStringLiteral("VersionDialog/tabWidgetIndex"), 0).toInt());

    // Init the note text edit search-frame
    ui->noteTextEdit->initSearchFrame(ui->noteTextEditSearchFrame);

    setupMainSplitter();

    QPushButton *button;
    ui->buttonBox->clear();

    button = new QPushButton(tr("&Restore selected version"));
    _restoreButton = button;
    _restoreButton->setEnabled(false);
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
    _diffList = new QStringList();
    _dataList = new QStringList();
    const auto currentNote = MainWindow::instance()->getCurrentNote();
    const bool canDecryptNoteText = currentNote.canDecryptNoteText();

    // If the text was decrypted, we use the styling of the encrypted note text edit
    if (canDecryptNoteText) {
        static const QString encryptedStyle =
            "QTextEdit,QPlainTextEdit {border: 3px dashed #aa0000;}";
        ui->noteTextEdit->setStyleSheet(encryptedStyle);
        ui->diffBrowser->setStyleSheet(encryptedStyle);
    }

    // Init the iterator for the versions
    QJSValueIterator versionsIterator(versions);

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
        _diffList->append(diffHtml);
        QString noteText = versionsIterator.value().property(QStringLiteral("data")).toString();

        // Decrypt the note text if possible
        if (canDecryptNoteText) {
            noteText = currentNote.getDecryptedNoteText(Note::parseEncryptedNoteText(noteText));
        }

        _dataList->append(noteText);
    }

    if (ui->versionListWidget->count() > 0) {
        ui->versionListWidget->setCurrentRow(0);
    }

    updateVersionPreview();
    ui->searchLineEdit->setFocus();
}

void VersionDialog::setupMainSplitter() {
    versionSplitter = new QSplitter;

    versionSplitter->addWidget(ui->listFrame);
    versionSplitter->addWidget(ui->tabWidget);

    // restore splitter sizes
    SettingsService settings;
    QByteArray state = settings.value(QStringLiteral("versionSplitterSizes")).toByteArray();
    versionSplitter->restoreState(state);

    ui->gridLayout->addWidget(versionSplitter);
}

void VersionDialog::storeSettings() {
    // Store the splitter sizes and last-used version view tab
    SettingsService settings;
    settings.setValue(QStringLiteral("versionSplitterSizes"), versionSplitter->saveState());
    settings.setValue(QStringLiteral("VersionDialog/tabWidgetIndex"),
                      ui->tabWidget->currentIndex());
}

VersionDialog::~VersionDialog() {
    delete _diffList;
    delete _dataList;
    delete ui;
}

void VersionDialog::on_searchLineEdit_textChanged(const QString &text) {
    int firstMatchingRow = -1;

    // Hide items without removing them so preview and restoration indices stay unchanged
    for (int row = 0; row < ui->versionListWidget->count(); ++row) {
        const bool matches = _dataList->at(row).contains(text, Qt::CaseInsensitive);
        ui->versionListWidget->item(row)->setHidden(!matches);
        if (matches && firstMatchingRow == -1) {
            firstMatchingRow = row;
        }
    }

    auto *searchWidget = ui->noteTextEdit->searchWidget();

    if (text.isEmpty()) {
        searchWidget->deactivate();
        ui->searchLineEdit->setFocus();
    } else {
        // Show the matches with the regular search bar of the note text tab
        ui->tabWidget->setCurrentWidget(ui->noteTextTab);
        searchWidget->setSearchText(text);
    }

    auto *currentItem = ui->versionListWidget->currentItem();
    if (currentItem == nullptr || currentItem->isHidden()) {
        ui->versionListWidget->setCurrentRow(firstMatchingRow);
    }

    updateVersionPreview();
}

void VersionDialog::on_versionListWidget_currentRowChanged(int) { updateVersionPreview(); }

void VersionDialog::updateVersionPreview() {
    const int currentRow = ui->versionListWidget->currentRow();
    const bool hasVersion = currentRow >= 0 && !ui->versionListWidget->item(currentRow)->isHidden();
    _restoreButton->setEnabled(hasVersion);
    ui->diffBrowser->setHtml(hasVersion ? _diffList->value(currentRow) : QString());
    ui->noteTextEdit->setPlainText(hasVersion ? _dataList->value(currentRow) : QString());

    // Highlight and jump to the matches of the selected version
    if (hasVersion && !ui->searchLineEdit->text().isEmpty()) {
        auto *searchWidget = ui->noteTextEdit->searchWidget();
        searchWidget->doSearchCount();
        searchWidget->activate(false);
        ui->noteTextEdit->ensureCursorVisible();
    }
}

void VersionDialog::dialogButtonClicked(QAbstractButton *button) {
    int actionRole = button->property("ActionRole").toInt();

    if (actionRole == Restore) {
        auto *currentItem = ui->versionListWidget->currentItem();
        if (currentItem == nullptr || currentItem->isHidden()) {
            return;
        }
        MainWindow::instance()->setCurrentNoteText(
            _dataList->value(ui->versionListWidget->currentRow()));
    }

    this->close();
}
