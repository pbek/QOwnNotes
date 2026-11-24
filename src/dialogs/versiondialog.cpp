#include "versiondialog.h"

#include <diff_match_patch.h>
#include <utils/misc.h>

#include <QDebug>
#include <QJSValue>
#include <QJSValueIterator>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QTextBrowser>
#include <QVBoxLayout>

#include "mainwindow.h"
#include "services/settingsservice.h"
#include "ui_versiondialog.h"
#include "utils/gui.h"

VersionDialog::VersionDialog(const QJSValue &versions, QWidget *parent) : VersionDialog(parent) {
    setVersions(versions);
}

VersionDialog::VersionDialog(QWidget *parent) : MasterDialog(parent), ui(new Ui::VersionDialog) {
    ui->setupUi(this);
    afterSetupUI();
    setIgnoreReturnKey(true);
    ui->tabWidget->setCurrentIndex(
        SettingsService().value(QStringLiteral("VersionDialog/tabWidgetIndex"), 0).toInt());

    // Init the note text edit search-frame
    ui->noteTextEdit->initSearchFrame(ui->noteTextEditSearchFrame);

    setupMainSplitter();

    Utils::Gui::setSectionHeaderFont(ui->previewHeaderLabel);
    Utils::Gui::setMutedTextColor(ui->versionCountLabel);
    ui->diffBrowser->setPlaceholderText(tr("Select a version to see its changes"));
    ui->noteTextEdit->setPlaceholderText(tr("Select a version to see its note text"));

    QPushButton *button;
    ui->buttonBox->clear();

    button = new QPushButton(tr("&Restore selected version"));
    _restoreButton = button;
    _restoreButton->setEnabled(false);
    button->setProperty("ActionRole", Restore);
    button->setToolTip(tr("Replace the text of the current note with the selected version"));
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

    _diffList = new QStringList();
    _dataList = new QStringList();
    updateVersionPreview();
}

/**
 * Fills the version list, can also be called after the dialog was opened
 */
void VersionDialog::setVersions(const QJSValue &versions) {
    const Note note =
        _localComparisonNote ? *_localComparisonNote : MainWindow::instance()->getCurrentNote();
    setVersions(versions, note, nullptr);
}

void VersionDialog::enableLocalComparison(const Note &note, const QString &editorText,
                                          bool decrypted) {
    _localComparisonNote.reset(new Note(note));
    _localComparisonText = Utils::Misc::transformLineFeeds(editorText);
    _localComparisonDecrypted = decrypted;

    auto *tab = new QWidget(ui->tabWidget);
    auto *layout = new QVBoxLayout(tab);
    auto *label = new QLabel(
        tr("Local editor text (including unsaved edits) → Selected server version\n"
           "Struck-through text is local only; highlighted additions are in the server version."),
        tab);
    label->setWordWrap(true);
    layout->addWidget(label);

    _localComparisonBrowser = new QTextBrowser(tab);
    _localComparisonBrowser->setPlaceholderText(tr("Select a server version to compare"));
    layout->addWidget(_localComparisonBrowser);
    ui->tabWidget->addTab(tab, tr("Compare with local note"));
    connect(ui->tabWidget, &QTabWidget::currentChanged, this,
            &VersionDialog::updateLocalComparison);
}

void VersionDialog::setGitVersions(const QJSValue &versions, const Note &note,
                                   const QString &currentNoteText) {
    setVersions(versions, note, &currentNoteText);
}

void VersionDialog::setVersions(const QJSValue &versions, const Note &note,
                                const QString *currentNoteText) {
    QString itemName;
    QString diffHtml;
    _currentNoteItem = nullptr;
    ui->versionListWidget->clear();
    _diffList->clear();
    _dataList->clear();
    const bool canDecryptNoteText = note.canDecryptNoteText();

    // If the text was decrypted, we use the styling of the encrypted note text edit
    if (canDecryptNoteText) {
        static const QString encryptedStyle =
            "QTextEdit,QPlainTextEdit {border: 3px dashed #aa0000;}";
        ui->noteTextEdit->setStyleSheet(encryptedStyle);
        ui->diffBrowser->setStyleSheet(encryptedStyle);
        if (_localComparisonBrowser && _localComparisonDecrypted) {
            _localComparisonBrowser->setStyleSheet(encryptedStyle);
        }
    }

    if (currentNoteText != nullptr) {
        _currentNoteItem = new QListWidgetItem(tr("Current note"), ui->versionListWidget);
        _diffList->append(QString());
        _dataList->append(*currentNoteText);
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

        auto *item = new QListWidgetItem(itemName, ui->versionListWidget);
        _diffList->append(diffHtml);
        QString noteText = versionsIterator.value().property(QStringLiteral("data")).toString();
        if (_localComparisonBrowser) {
            // Preserve the server text so the comparison can match the editor's encryption state.
            item->setData(Qt::UserRole, noteText);
        }

        // Decrypt the note text if possible
        if (canDecryptNoteText) {
            noteText = note.getDecryptedNoteText(Note::parseEncryptedNoteText(noteText));
        }

        _dataList->append(noteText);
    }

    // Select the newest historical version by default, not the comparison baseline.
    const int firstVersionRow = _currentNoteItem == nullptr ? 0 : 1;
    if (ui->versionListWidget->count() > firstVersionRow) {
        ui->versionListWidget->setCurrentRow(firstVersionRow);
    }

    updateVersionPreview();
    ui->searchLineEdit->setFocus();
}

void VersionDialog::setupMainSplitter() {
    versionSplitter = new QSplitter;
    versionSplitter->setChildrenCollapsible(false);

    versionSplitter->addWidget(ui->listFrame);
    versionSplitter->addWidget(ui->previewFrame);

    // Restore the splitter sizes, give the preview more room by default
    SettingsService settings;
    QByteArray state = settings.value(QStringLiteral("versionSplitterSizes")).toByteArray();
    if (!versionSplitter->restoreState(state)) {
        versionSplitter->setStretchFactor(0, 1);
        versionSplitter->setStretchFactor(1, 2);
    }

    ui->gridLayout->addWidget(versionSplitter);
}

void VersionDialog::storeSettings() {
    // Store the splitter sizes and last-used version view tab
    SettingsService settings;
    settings.setValue(QStringLiteral("versionSplitterSizes"), versionSplitter->saveState());
    // Don't save the cloud-only comparison tab as the default for the shared dialog.
    if (_localComparisonBrowser == nullptr ||
        ui->tabWidget->currentWidget() != _localComparisonBrowser->parentWidget()) {
        settings.setValue(QStringLiteral("VersionDialog/tabWidgetIndex"),
                          ui->tabWidget->currentIndex());
    }
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
    const auto *item = ui->versionListWidget->currentItem();
    const bool hasVersion = item != nullptr && !item->isHidden();
    const bool isCurrentNote = hasVersion && item == _currentNoteItem;
    _restoreButton->setEnabled(hasVersion && !isCurrentNote);
    updateCountLabel();

    if (isCurrentNote) {
        ui->previewHeaderLabel->setText(item->text());
    } else if (hasVersion) {
        ui->previewHeaderLabel->setText(tr("Version from %1").arg(item->text()));
    } else if (ui->versionListWidget->count() == 0) {
        ui->previewHeaderLabel->setText(tr("No versions loaded"));
    } else if (!ui->searchLineEdit->text().isEmpty()) {
        ui->previewHeaderLabel->setText(tr("No version matches the search"));
    } else {
        ui->previewHeaderLabel->setText(tr("No version selected"));
    }

    if (isCurrentNote) {
        ui->diffBrowser->setPlainText(tr("Historical versions are compared with this snapshot."));
    } else {
        ui->diffBrowser->setHtml(
            hasVersion ? Utils::Gui::readableDiffHtml(_diffList->value(currentRow)) : QString());
    }
    ui->noteTextEdit->setPlainText(hasVersion ? _dataList->value(currentRow) : QString());
    updateLocalComparison();

    // Highlight and jump to the matches of the selected version
    if (hasVersion && !ui->searchLineEdit->text().isEmpty()) {
        auto *searchWidget = ui->noteTextEdit->searchWidget();
        searchWidget->doSearchCount();
        searchWidget->activate(false);
        ui->noteTextEdit->ensureCursorVisible();
    }
}

void VersionDialog::updateLocalComparison() {
    if (_localComparisonBrowser == nullptr ||
        ui->tabWidget->currentWidget() != _localComparisonBrowser->parentWidget()) {
        return;
    }

    const auto *item = ui->versionListWidget->currentItem();
    if (item == nullptr || item->isHidden()) {
        _localComparisonBrowser->clear();
        return;
    }

    QString serverText = item->data(Qt::UserRole).toString();
    if (_localComparisonDecrypted) {
        Note historicalNote = *_localComparisonNote;
        historicalNote.setNoteText(serverText);
        const QString encryptedText = Note::parseEncryptedNoteText(serverText);
        if (!encryptedText.isEmpty()) {
            serverText = historicalNote.getDecryptedNoteText(encryptedText);
            if (Note::parseEncryptedNoteText(serverText) == encryptedText) {
                _localComparisonBrowser->setPlainText(
                    tr("The selected server version could not be decrypted for comparison."));
                return;
            }
        }
    }

    serverText = Utils::Misc::transformLineFeeds(serverText);
    if (_localComparisonText == serverText) {
        _localComparisonBrowser->setPlainText(tr("No differences from the local editor text."));
        return;
    }

    diff_match_patch diff;
    auto changes = diff.diff_main(_localComparisonText, serverText);
    diff.diff_cleanupSemantic(changes);
    _localComparisonBrowser->setHtml(
        QStringLiteral("<div style=\"white-space: pre-wrap;\">%1</div>")
            .arg(Utils::Gui::readableDiffHtml(diff.diff_prettyHtml(changes))));
}

/**
 * Shows how many versions are in the list, and how many of them match the search
 */
void VersionDialog::updateCountLabel() {
    const int firstVersionRow = _currentNoteItem == nullptr ? 0 : 1;
    const int total = ui->versionListWidget->count() - firstVersionRow;

    if (ui->searchLineEdit->text().isEmpty()) {
        ui->versionCountLabel->setText(_currentNoteItem == nullptr
                                           ? tr("%n version(s)", "", total)
                                           : tr("%n historical version(s)", "", total));
        return;
    }

    int visible = 0;
    for (int row = firstVersionRow; row < ui->versionListWidget->count(); ++row) {
        if (!ui->versionListWidget->item(row)->isHidden()) {
            ++visible;
        }
    }

    const QString countText = _currentNoteItem == nullptr
                                  ? tr("%1 of %n version(s)", "", total)
                                  : tr("%1 of %n historical version(s)", "", total);
    ui->versionCountLabel->setText(countText.arg(visible));
}

void VersionDialog::dialogButtonClicked(QAbstractButton *button) {
    int actionRole = button->property("ActionRole").toInt();

    if (actionRole == Restore) {
        auto *currentItem = ui->versionListWidget->currentItem();
        if (currentItem == nullptr || currentItem == _currentNoteItem || currentItem->isHidden()) {
            return;
        }
        MainWindow::instance()->setCurrentNoteText(
            _dataList->value(ui->versionListWidget->currentRow()));
    }

    this->close();
}
