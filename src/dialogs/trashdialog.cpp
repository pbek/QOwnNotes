#include "trashdialog.h"

#include <services/cloudservice.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QAction>
#include <QCheckBox>
#include <QDebug>
#include <QJSValue>
#include <QJSValueIterator>
#include <QKeySequence>
#include <QProgressDialog>
#include <QPushButton>
#include <QSplitter>

#include "libraries/qmarkdowntextedit/qplaintexteditsearchwidget.h"
#include "mainwindow.h"
#include "services/settingsservice.h"
#include "ui_trashdialog.h"

namespace {
enum ItemRole { NoteTextRole = Qt::UserRole + 1, TimestampRole, DateStringRole };
}

TrashDialog::TrashDialog(const QJSValue &notes, QWidget *parent) : TrashDialog(parent) {
    setNotes(notes);
}

TrashDialog::TrashDialog(QWidget *parent) : MasterDialog(parent), ui(new Ui::TrashDialog) {
    ui->setupUi(this);
    afterSetupUI();

    // init the note browser search frame
    ui->noteBrowser->initSearchFrame(ui->noteBrowserSearchFrame);

    setupMainSplitter();

    Utils::Gui::setSectionHeaderFont(ui->previewHeaderLabel);
    Utils::Gui::setMutedTextColor(ui->previewDetailLabel);
    ui->previewDetailLabel->setTextFormat(Qt::RichText);
    Utils::Gui::setMutedTextColor(ui->trashCountLabel);
    ui->noteBrowser->setPlaceholderText(tr("Select a trashed note to see its text"));

    // Give the preview header some room, so it doesn't stick to the list
    ui->previewHeaderLabel->setContentsMargins(8, 4, 8, 0);
    ui->previewDetailLabel->setContentsMargins(8, 0, 8, 4);
    ui->trashCountLabel->setContentsMargins(0, 0, 4, 0);

    QPushButton *button;
    ui->buttonBox->clear();

    button = new QPushButton(tr("&Restore on server"));
    button->setToolTip(
        tr("<h3>Slower, but with note versions</h3>"
           "<p>The selected notes will be restored on your Nextcloud / ownCloud "
           "server with all versions.</p>"
           "<p>You will have to wait until they are synced to "
           "QOwnNotes by Nextcloud / ownCloud sync.</p>"));
    button->setProperty("ActionRole", RestoreOnServer);
    button->setDefault(false);
    _restoreButton = button;
    button->setIcon(QIcon::fromTheme(QStringLiteral("view-restore"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/view-restore.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Download"));
    button->setToolTip(
        tr("<h3>Faster, but without versions</h3>"
           "<p>The selected notes will be created with the text from the trash.</p>"
           "<p>The note versions on your Nextcloud / ownCloud server will not be "
           "restored and the notes will remain in the trash.</p>"
           "<p>You can always restore the notes and their versions later.</p>"));
    button->setProperty("ActionRole", Download);
    button->setDefault(false);
    _downloadButton = button;
    button->setIcon(QIcon::fromTheme(QStringLiteral("edit-download"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/edit-download.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Delete"));
    button->setToolTip(tr("Permanently delete selected notes on server"));
    button->setProperty("ActionRole", DeleteOnServer);
    button->setDefault(false);
    _deleteButton = button;
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
    connect(ui->trashListWidget, &QListWidget::itemSelectionChanged, this,
            &TrashDialog::updatePreviewInfo);
    connect(ui->selectAllCheckBox, &QCheckBox::clicked, this,
            &TrashDialog::onSelectAllCheckBoxClicked);

    setupContextMenu();
    updatePreviewInfo();
}

/**
 * Adds the bulk actions to the context menu of the trash list
 */
void TrashDialog::setupContextMenu() {
    auto *list = ui->trashListWidget;
    list->setContextMenuPolicy(Qt::ActionsContextMenu);

    _restoreAction = new QAction(_restoreButton->icon(), tr("Restore on server"), list);
    connect(_restoreAction, &QAction::triggered, this, &TrashDialog::restoreSelectedNotesOnServer);

    _downloadAction = new QAction(_downloadButton->icon(), tr("Download"), list);
    connect(_downloadAction, &QAction::triggered, this, &TrashDialog::downloadSelectedNotes);

    _deleteAction = new QAction(_deleteButton->icon(), tr("Delete on server"), list);
    _deleteAction->setShortcut(QKeySequence::Delete);
    _deleteAction->setShortcutContext(Qt::WidgetShortcut);
    connect(_deleteAction, &QAction::triggered, this, &TrashDialog::deleteSelectedNotesOnServer);

    auto *separator = new QAction(list);
    separator->setSeparator(true);

    // Select all only selects the notes that match the current search
    _selectAllAction = new QAction(
        QIcon::fromTheme(
            QStringLiteral("edit-select-all"),
            QIcon(QStringLiteral(":/icons/breeze-qownnotes/16x16/edit-select-all.svg"))),
        tr("Select all"), list);
    _selectAllAction->setShortcut(QKeySequence::SelectAll);
    _selectAllAction->setShortcutContext(Qt::WidgetShortcut);
    connect(_selectAllAction, &QAction::triggered, this, &TrashDialog::selectAllVisibleItems);

    _clearSelectionAction = new QAction(
        QIcon::fromTheme(QStringLiteral("edit-clear"),
                         QIcon(QStringLiteral(":/icons/breeze-qownnotes/16x16/edit-clear.svg"))),
        tr("Clear selection"), list);
    connect(_clearSelectionAction, &QAction::triggered, this, &TrashDialog::clearSelection);

    list->addActions({_restoreAction, _downloadAction, _deleteAction, separator, _selectAllAction,
                      _clearSelectionAction});
}

/**
 * Fills the trash list, can also be called after the dialog was opened
 */
void TrashDialog::setNotes(const QJSValue &notes) {
    QString itemName;
    QString dateString;
    QString data;
    int timestamp;
    ui->trashListWidget->clear();

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
        item->setData(NoteTextRole, data);
        item->setData(TimestampRole, timestamp);
        item->setData(DateStringRole, dateString);
        item->setToolTip(dateString);
        ui->trashListWidget->addItem(item);
    }

    ui->trashListWidget->setCurrentRow(0);
    updatePreviewInfo();
}

void TrashDialog::setupMainSplitter() {
    trashSplitter = new QSplitter;
    trashSplitter->setChildrenCollapsible(false);

    trashSplitter->addWidget(ui->listFrame);
    trashSplitter->addWidget(ui->noteBrowserFrame);

    // Restore the splitter sizes, give the preview more room by default
    SettingsService settings;
    QByteArray state = settings.value(QStringLiteral("trashSplitterSizes")).toByteArray();
    if (!trashSplitter->restoreState(state)) {
        trashSplitter->setStretchFactor(0, 1);
        trashSplitter->setStretchFactor(1, 2);
    }

    ui->gridLayout->layout()->addWidget(trashSplitter);
    ui->gridLayout->layout()->addWidget(ui->buttonBox);
}

void TrashDialog::storeSettings() {
    // store the splitter sizes
    SettingsService settings;
    settings.setValue(QStringLiteral("trashSplitterSizes"), trashSplitter->saveState());
}

TrashDialog::~TrashDialog() { delete ui; }

void TrashDialog::on_trashListWidget_currentRowChanged(int currentRow) {
    auto *item = ui->trashListWidget->item(currentRow);
    ui->noteBrowser->setPlainText(
        item != nullptr && !item->isHidden() ? item->data(NoteTextRole).toString() : QString());
    updatePreviewInfo();

    if (!ui->searchLineEdit->text().isEmpty()) {
        // Highlight and jump to the query without taking focus from the trash filter
        auto *searchWidget = ui->noteBrowser->searchWidget();
        searchWidget->setSearchMode(QPlainTextEditSearchWidget::PlainTextMode);
        searchWidget->setSearchText(ui->searchLineEdit->text());
        searchWidget->doSearchCount();
        searchWidget->activate(false);
        ui->noteBrowser->ensureCursorVisible();
    }
}

/**
 * Updates the preview header, the trashed note count, the selection state and the actions
 */
void TrashDialog::updatePreviewInfo() {
    const int total = ui->trashListWidget->count();
    const int visible = visibleItemCount();
    const int selected = static_cast<int>(selectedVisibleItems().count());

    QString countText = ui->searchLineEdit->text().isEmpty()
                            ? tr("%n trashed note(s)", "", total)
                            : tr("%1 of %n trashed note(s)", "", total).arg(visible);
    if (selected > 0) {
        countText = tr("%1 · %n selected", "", selected).arg(countText);
    }
    ui->trashCountLabel->setText(countText);

    // Reflect the selection state of the visible notes in the select all checkbox
    ui->selectAllCheckBox->setEnabled(visible > 0);
    ui->selectAllCheckBox->setCheckState(selected == 0         ? Qt::Unchecked
                                         : selected == visible ? Qt::Checked
                                                               : Qt::PartiallyChecked);

    const bool hasSelection = selected > 0;
    _restoreButton->setEnabled(hasSelection);
    _downloadButton->setEnabled(hasSelection);
    _deleteButton->setEnabled(hasSelection);
    _restoreAction->setEnabled(hasSelection);
    _downloadAction->setEnabled(hasSelection);
    _deleteAction->setEnabled(hasSelection);
    _selectAllAction->setEnabled(visible > 0 && selected < visible);
    _clearSelectionAction->setEnabled(hasSelection);

    // Show how many notes a button will affect when more than one is selected
    const bool multiple = selected > 1;
    _restoreButton->setText(multiple ? tr("&Restore %n notes on server", "", selected)
                                     : tr("&Restore on server"));
    _downloadButton->setText(multiple ? tr("&Download %n notes", "", selected) : tr("&Download"));
    _deleteButton->setText(multiple ? tr("&Delete %n notes", "", selected) : tr("&Delete"));
    _restoreAction->setText(multiple ? tr("Restore %n notes on server", "", selected)
                                     : tr("Restore on server"));
    _downloadAction->setText(multiple ? tr("Download %n notes", "", selected) : tr("Download"));
    _deleteAction->setText(multiple ? tr("Delete %n notes on server", "", selected)
                                    : tr("Delete on server"));

    const auto *item = ui->trashListWidget->currentItem();

    if (item != nullptr && !item->isHidden()) {
        ui->previewHeaderLabel->setText(item->text());
        const QString dateString = item->data(DateStringRole).toString();
        ui->previewDetailLabel->setText(
            dateString.isEmpty() ? QString() : tr("Trashed: %1").arg(dateString.toHtmlEscaped()));
    } else {
        ui->previewHeaderLabel->setText(total == 0     ? tr("No trashed notes")
                                        : visible == 0 ? tr("No trashed note matches the search")
                                                       : tr("No trashed note selected"));
        ui->previewDetailLabel->clear();
    }

    ui->previewDetailLabel->setHidden(ui->previewDetailLabel->text().isEmpty());
}

void TrashDialog::dialogButtonClicked(QAbstractButton *button) {
    switch (button->property("ActionRole").toInt()) {
        case Download:
            downloadSelectedNotes();
            break;
        case RestoreOnServer:
            restoreSelectedNotesOnServer();
            break;
        case DeleteOnServer:
            deleteSelectedNotesOnServer();
            break;
        default:
            close();
            break;
    }
}

/**
 * Restores the selected notes with all their versions on the server and closes the dialog
 */
void TrashDialog::restoreSelectedNotesOnServer() {
    const QList<QListWidgetItem *> items = selectedVisibleItems();
    const int count = static_cast<int>(items.count());

    if (count == 0) {
        return;
    }

    if (count > 1 &&
        Utils::Gui::question(
            this, tr("Restore notes on server"),
            tr("Restore <strong>%n</strong> selected note(s) on the server?", "", count),
            QStringLiteral("trashdialog-restore-notes")) != QMessageBox::Yes) {
        return;
    }

    for (auto *item : items) {
        const QString fileName = item->data(Qt::UserRole).toString();
        const int timestamp = item->data(TimestampRole).toInt();
        qDebug() << item->text() << timestamp;

#ifndef INTEGRATION_TESTS
        MainWindow::instance()->restoreTrashedNoteOnServer(fileName, timestamp);
#else
        Q_UNUSED(fileName)
#endif
    }

    close();
}

/**
 * Creates new notes with the text of the selected trashed notes and closes the dialog
 */
void TrashDialog::downloadSelectedNotes() {
    const QList<QListWidgetItem *> items = selectedVisibleItems();
    const int count = static_cast<int>(items.count());

    if (count == 0) {
        return;
    }

    if (count > 1 &&
        Utils::Gui::question(this, tr("Download notes"),
                             tr("Create <strong>%n</strong> new note(s) from the selected "
                                "trashed note(s)?",
                                "", count),
                             QStringLiteral("trashdialog-download-notes")) != QMessageBox::Yes) {
        return;
    }

#ifndef INTEGRATION_TESTS
    for (auto *item : items) {
        MainWindow::instance()->createNewNote(item->text(), item->data(NoteTextRole).toString());
    }
#endif

    close();
}

/**
 * Permanently deletes the selected trashed notes on the server
 */
void TrashDialog::deleteSelectedNotesOnServer() {
    const QList<QListWidgetItem *> items = selectedVisibleItems();
    const int count = static_cast<int>(items.count());

    if (count == 0) {
        return;
    }

    if (Utils::Gui::question(
            this, tr("Delete notes on server"),
            tr("Permanently delete <strong>%n</strong> selected trashed note(s) on the server?", "",
               count),
            QStringLiteral("trashdialog-delete"), QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    CloudService *cloud = CloudService::instance();
    ui->trashListWidget->setDisabled(true);
    ui->buttonBox->setDisabled(true);
    ui->selectAllCheckBox->setDisabled(true);

    // Every deletion is a separate request, so show the progress right away for many notes
    QProgressDialog progress(QString(), tr("Cancel"), 0, count, this);
    progress.setWindowTitle(tr("Deleting notes on server"));
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(count > 1 ? 0 : 500);

    QList<QListWidgetItem *> deletedItems;
    QStringList failedNotes;

    for (int i = 0; i < count; ++i) {
        if (progress.wasCanceled()) {
            break;
        }

        auto *item = items.at(i);
        progress.setLabelText(tr("Deleting %1…").arg(item->text()));
        progress.setValue(i);

        const int statusCode = cloud->deleteTrashedNoteOnServer(item->data(Qt::UserRole).toString(),
                                                                item->data(TimestampRole).toInt());

        if (statusCode >= 200 && statusCode < 300) {
            deletedItems << item;
        } else {
            failedNotes
                << tr("%1 (status code: %2)").arg(item->text(), QString::number(statusCode));
        }
    }

    progress.setValue(count);
    qDeleteAll(deletedItems);

    ui->trashListWidget->setDisabled(false);
    ui->buttonBox->setDisabled(false);
    on_searchLineEdit_textChanged(ui->searchLineEdit->text());

    if (!failedNotes.isEmpty()) {
        Utils::Gui::warning(this, tr("Error while deleting notes"),
                            tr("<strong>%n</strong> trashed note(s) couldn't be deleted:", "",
                               failedNotes.count()) +
                                failedNotesHtml(failedNotes),
                            QStringLiteral("trashdialog-delete-failed"));
    }
}

/**
 * Returns an HTML list of note names, long lists are shortened
 */
QString TrashDialog::failedNotesHtml(const QStringList &names) const {
    const int maxShown = 10;
    QString html = QStringLiteral("<ul>");

    for (int i = 0; i < names.count() && i < maxShown; ++i) {
        html += QStringLiteral("<li>") + names.at(i).toHtmlEscaped() + QStringLiteral("</li>");
    }

    if (names.count() > maxShown) {
        html += QStringLiteral("<li>") +
                tr("…and %n more", "", static_cast<int>(names.count()) - maxShown) +
                QStringLiteral("</li>");
    }

    return html + QStringLiteral("</ul>");
}

/**
 * Returns the number of items that are not hidden by the search
 */
int TrashDialog::visibleItemCount() const {
    int visible = 0;
    for (int row = 0; row < ui->trashListWidget->count(); ++row) {
        if (!ui->trashListWidget->item(row)->isHidden()) {
            ++visible;
        }
    }
    return visible;
}

/**
 * Returns the selected items that are not hidden by the search, in list order
 */
QList<QListWidgetItem *> TrashDialog::selectedVisibleItems() const {
    QList<QListWidgetItem *> items;
    for (int row = 0; row < ui->trashListWidget->count(); ++row) {
        auto *item = ui->trashListWidget->item(row);
        if (item->isSelected() && !item->isHidden()) {
            items << item;
        }
    }
    return items;
}

/**
 * Selects all notes that match the current search
 */
void TrashDialog::selectAllVisibleItems() {
    {
        const QSignalBlocker blocker(ui->trashListWidget);
        Q_UNUSED(blocker)

        for (int row = 0; row < ui->trashListWidget->count(); ++row) {
            auto *item = ui->trashListWidget->item(row);
            item->setSelected(!item->isHidden());
        }
    }

    updatePreviewInfo();
}

void TrashDialog::clearSelection() {
    ui->trashListWidget->clearSelection();
    updatePreviewInfo();
}

void TrashDialog::onSelectAllCheckBoxClicked() {
    const int selected = static_cast<int>(selectedVisibleItems().count());

    // Deselect everything if all notes are selected, otherwise select all of them
    if (selected > 0 && selected == visibleItemCount()) {
        clearSelection();
    } else {
        selectAllVisibleItems();
    }
}

void TrashDialog::on_searchLineEdit_textChanged(const QString &arg1) {
    if (arg1.isEmpty()) {
        ui->noteBrowser->searchWidget()->deactivate();
        ui->searchLineEdit->setFocus();
    }

    int firstMatchingRow = -1;

    // Keep note text and restoration metadata attached to each filtered item
    for (int row = 0; row < ui->trashListWidget->count(); ++row) {
        auto *item = ui->trashListWidget->item(row);
        const bool matches =
            item->text().contains(arg1, Qt::CaseInsensitive) ||
            item->data(NoteTextRole).toString().contains(arg1, Qt::CaseInsensitive);
        if (!matches) {
            item->setSelected(false);
        }
        item->setHidden(!matches);
        if (matches && firstMatchingRow == -1) {
            firstMatchingRow = row;
        }
    }

    auto *current = ui->trashListWidget->currentItem();
    if (current == nullptr || current->isHidden()) {
        ui->trashListWidget->setCurrentRow(firstMatchingRow);
    }

    // Changing the current row may select it again in extended selection mode,
    // hidden notes must never be part of a bulk operation
    for (auto *item : ui->trashListWidget->selectedItems()) {
        if (item->isHidden()) {
            item->setSelected(false);
        }
    }
    on_trashListWidget_currentRowChanged(ui->trashListWidget->currentRow());
}
