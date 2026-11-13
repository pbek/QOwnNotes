#include "localtrashdialog.h"

#include <entities/trashitem.h>
#include <utils/gui.h>

#include <QAction>
#include <QCheckBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QKeySequence>
#include <QLocale>
#include <QProgressDialog>
#include <QPushButton>
#include <QSplitter>

#include "libraries/qmarkdowntextedit/qplaintexteditsearchwidget.h"
#include "services/settingsservice.h"
#include "ui_localtrashdialog.h"

namespace {
// Column 0 data roles, Qt::UserRole holds the trash item id
enum ItemRole { NoteTextRole = Qt::UserRole + 1, DetailRole };
}    // namespace

class LocalTrashTreeWidgetItem : public QTreeWidgetItem {
   public:
    LocalTrashTreeWidgetItem(QTreeWidget *parent) : QTreeWidgetItem(parent) {}

   private:
    bool operator<(const QTreeWidgetItem &other) const {
        int column = treeWidget()->sortColumn();

        // allow sorting for the date column
        if (column == 1) {
            return data(column, Qt::UserRole).toLongLong() <
                   other.data(column, Qt::UserRole).toLongLong();
        }

        return text(column).toLower() < other.text(column).toLower();
    }
};

LocalTrashDialog::LocalTrashDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::LocalTrashDialog) {
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

    button = new QPushButton(tr("&Restore"), this);
    button->setToolTip(tr("Restore selected notes"));
    button->setProperty("ActionRole", Restore);
    button->setDefault(false);
    _restoreButton = button;
    button->setIcon(QIcon::fromTheme(QStringLiteral("view-restore"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/view-restore.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("Re&move"), this);
    button->setToolTip(tr("Permanently remove selected notes"));
    button->setProperty("ActionRole", Remove);
    button->setDefault(false);
    _removeButton = button;
    button->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/edit-delete.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    // The close button has no action role, so it just closes the dialog
    ui->buttonBox->addButton(QDialogButtonBox::Close);

    connect(this->ui->buttonBox, SIGNAL(clicked(QAbstractButton *)),
            SLOT(dialogButtonClicked(QAbstractButton *)));
    connect(this, SIGNAL(finished(int)), this, SLOT(storeSettings()));
    connect(ui->trashTreeWidget, &QTreeWidget::itemSelectionChanged, this,
            &LocalTrashDialog::updatePreviewInfo);
    connect(ui->selectAllCheckBox, &QCheckBox::clicked, this,
            &LocalTrashDialog::onSelectAllCheckBoxClicked);
    Utils::Gui::initTreeWidgetHeaderOrderPersistence(
        ui->trashTreeWidget, QStringLiteral("LocalTrashDialog/trashTreeWidgetHeaderOrder"));

    setupContextMenu();
    loadTrashedNotes();
}

/**
 * Adds the bulk actions to the context menu of the trash list
 */
void LocalTrashDialog::setupContextMenu() {
    auto *tree = ui->trashTreeWidget;
    tree->setContextMenuPolicy(Qt::ActionsContextMenu);

    _restoreAction = new QAction(_restoreButton->icon(), tr("Restore"), tree);
    connect(_restoreAction, &QAction::triggered, this,
            &LocalTrashDialog::restoreSelectedTrashItems);

    _removeAction = new QAction(_removeButton->icon(), tr("Remove"), tree);
    _removeAction->setShortcut(QKeySequence::Delete);
    _removeAction->setShortcutContext(Qt::WidgetShortcut);
    connect(_removeAction, &QAction::triggered, this, &LocalTrashDialog::removeSelectedTrashItems);

    auto *separator = new QAction(tree);
    separator->setSeparator(true);

    // Select all only selects the notes that match the current search
    _selectAllAction = new QAction(
        QIcon::fromTheme(
            QStringLiteral("edit-select-all"),
            QIcon(QStringLiteral(":/icons/breeze-qownnotes/16x16/edit-select-all.svg"))),
        tr("Select all"), tree);
    _selectAllAction->setShortcut(QKeySequence::SelectAll);
    _selectAllAction->setShortcutContext(Qt::WidgetShortcut);
    connect(_selectAllAction, &QAction::triggered, this, &LocalTrashDialog::selectAllVisibleItems);

    _clearSelectionAction = new QAction(
        QIcon::fromTheme(QStringLiteral("edit-clear"),
                         QIcon(QStringLiteral(":/icons/breeze-qownnotes/16x16/edit-clear.svg"))),
        tr("Clear selection"), tree);
    connect(_clearSelectionAction, &QAction::triggered, this, &LocalTrashDialog::clearSelection);

    tree->addActions(
        {_restoreAction, _removeAction, separator, _selectAllAction, _clearSelectionAction});
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

        auto *item = new LocalTrashTreeWidgetItem(ui->trashTreeWidget);
        item->setText(0, trashItem.getFileName());
        item->setText(1, QLocale().toString(trashItem.getCreated(), QLocale::ShortFormat));
        item->setData(0, Qt::UserRole, trashItem.getId());
        // add timestamp data to allow sorting
        item->setData(1, Qt::UserRole, trashItem.getCreated().toMSecsSinceEpoch());

        QString toolTipText =
            tr("File will be restored to: %1").arg(trashItem.restorationFilePath().toHtmlEscaped());

        // mark missing items if file is missing
        if (!trashItem.fileExists()) {
            item->setIcon(
                0, QIcon::fromTheme(QStringLiteral("dialog-warning"),
                                    QIcon(":/icons/breeze-qownnotes/16x16/edit-delete.svg")));
            toolTipText = tr("File <strong>%1</strong> isn't readable and can't"
                             " be restored!")
                              .arg(trashItem.fullFilePath().toHtmlEscaped());
        }

        item->setData(0, DetailRole, toolTipText);

        item->setToolTip(0, toolTipText);
        item->setToolTip(1, toolTipText);

        ui->trashTreeWidget->addTopLevelItem(item);
    }

    ui->trashTreeWidget->sortItems(1, Qt::SortOrder::DescendingOrder);

    if (Utils::Gui::hasTreeWidgetHeaderLayout(ui->trashTreeWidget)) {
        Utils::Gui::restoreTreeWidgetHeaderLayout(ui->trashTreeWidget);
    } else {
        ui->trashTreeWidget->resizeColumnToContents(0);
    }

    on_searchLineEdit_textChanged(ui->searchLineEdit->text());
}

void LocalTrashDialog::setupMainSplitter() {
    trashSplitter = new QSplitter(this);
    trashSplitter->setChildrenCollapsible(false);

    trashSplitter->addWidget(ui->listFrame);
    trashSplitter->addWidget(ui->noteBrowserFrame);

    // Restore the splitter sizes, give the preview more room by default
    SettingsService settings;
    QByteArray state = settings.value(QStringLiteral("localTrashSplitterSizes")).toByteArray();
    if (!trashSplitter->restoreState(state)) {
        trashSplitter->setStretchFactor(0, 1);
        trashSplitter->setStretchFactor(1, 2);
    }

    ui->gridLayout->layout()->addWidget(trashSplitter);
    ui->gridLayout->layout()->addWidget(ui->buttonBox);
}

void LocalTrashDialog::storeSettings() {
    // store the splitter sizes
    SettingsService settings;
    settings.setValue(QStringLiteral("localTrashSplitterSizes"), trashSplitter->saveState());
}

LocalTrashDialog::~LocalTrashDialog() { delete ui; }

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
void LocalTrashDialog::on_trashTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                             QTreeWidgetItem *previous) {
    Q_UNUSED(previous);
    QString text;
    if (current != nullptr && !current->isHidden()) {
        int trashItemId = current->data(0, Qt::UserRole).toInt();
        TrashItem trashItem = TrashItem::fetch(trashItemId);
        text = trashItem.loadFileFromDisk();
    }
    ui->noteBrowser->setPlainText(text);
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
 * Restores the selected notes that are visible in the list
 */
void LocalTrashDialog::restoreSelectedTrashItems() {
    const QList<QTreeWidgetItem *> items = selectedVisibleItems();
    const int count = static_cast<int>(items.count());

    if (count == 0) {
        return;
    }

    if (Utils::Gui::question(this, tr("Restore selected notes"),
                             tr("Restore <strong>%n</strong> selected note(s)?", "", count),
                             QStringLiteral("local-trash-restore-notes")) != QMessageBox::Yes) {
        return;
    }

    const QSignalBlocker blocker(ui->trashTreeWidget);
    Q_UNUSED(blocker)

    // The progress dialog only shows up if restoring takes a noticeable amount of time
    QProgressDialog progress(QString(), tr("Cancel"), 0, count, this);
    progress.setWindowTitle(tr("Restoring notes"));
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(500);

    int restoreCount = 0;
    QStringList failedNotes;

    for (int i = 0; i < count; ++i) {
        if (progress.wasCanceled()) {
            break;
        }

        const QString name = items.at(i)->text(0);
        progress.setLabelText(tr("Restoring %1…").arg(name));
        progress.setValue(i);

        TrashItem trashItem = TrashItem::fetch(items.at(i)->data(0, Qt::UserRole).toInt());
        if (trashItem.restoreFile()) {
            qDebug() << "Restored note " << trashItem.getFileName();
            restoreCount++;
        } else {
            qDebug() << "Note " << trashItem.getFileName() << " couldn't be restored!";
            failedNotes << name;
        }
    }

    progress.setValue(count);

    if (failedNotes.isEmpty()) {
        Utils::Gui::information(this, tr("Notes restored"),
                                tr("<strong>%n</strong> note(s) were restored", "", restoreCount),
                                QStringLiteral("local-trash-notes-restored"));
    } else {
        Utils::Gui::warning(
            this, tr("Not all notes were restored"),
            tr("<strong>%n</strong> note(s) were restored", "", restoreCount) +
                QStringLiteral("<br><br>") +
                tr("<strong>%n</strong> note(s) couldn't be restored:", "", failedNotes.count()) +
                failedNotesHtml(failedNotes),
            QStringLiteral("local-trash-notes-restore-failed"));
    }

    if (restoreCount > 0) {
        loadTrashedNotes();
    }
}

/**
 * Permanently removes the selected notes that are visible in the list
 */
void LocalTrashDialog::removeSelectedTrashItems() {
    const QList<QTreeWidgetItem *> items = selectedVisibleItems();
    const int count = static_cast<int>(items.count());

    if (count == 0) {
        return;
    }

    if (Utils::Gui::question(this, tr("Remove selected notes"),
                             tr("Remove <strong>%n</strong> selected note(s)?", "", count),
                             QStringLiteral("local-trash-remove-notes")) != QMessageBox::Yes) {
        return;
    }

    const QSignalBlocker blocker(ui->trashTreeWidget);
    Q_UNUSED(blocker)

    // The progress dialog only shows up if removing takes a noticeable amount of time
    QProgressDialog progress(QString(), tr("Cancel"), 0, count, this);
    progress.setWindowTitle(tr("Removing notes"));
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(500);

    int removeCount = 0;
    QStringList failedNotes;

    for (int i = 0; i < count; ++i) {
        if (progress.wasCanceled()) {
            break;
        }

        const QString name = items.at(i)->text(0);
        progress.setLabelText(tr("Removing %1…").arg(name));
        progress.setValue(i);

        TrashItem trashItem = TrashItem::fetch(items.at(i)->data(0, Qt::UserRole).toInt());
        if (trashItem.remove(true)) {
            qDebug() << "Removed note " << trashItem.getFileName();
            removeCount++;
        } else {
            qDebug() << "Note " << trashItem.getFileName() << " couldn't be removed!";
            failedNotes << name;
        }
    }

    progress.setValue(count);

    if (failedNotes.isEmpty()) {
        Utils::Gui::information(this, tr("Notes removed"),
                                tr("<strong>%n</strong> note(s) were removed", "", removeCount),
                                QStringLiteral("local-trash-notes-removed"));
    } else {
        Utils::Gui::warning(
            this, tr("Not all notes were removed"),
            tr("<strong>%n</strong> note(s) were removed", "", removeCount) +
                QStringLiteral("<br><br>") +
                tr("<strong>%n</strong> note(s) couldn't be removed:", "", failedNotes.count()) +
                failedNotesHtml(failedNotes),
            QStringLiteral("local-trash-notes-remove-failed"));
    }

    if (removeCount > 0) {
        loadTrashedNotes();
    }
}

/**
 * Returns an HTML list of note names, long lists are shortened
 */
QString LocalTrashDialog::failedNotesHtml(const QStringList &names) const {
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
int LocalTrashDialog::visibleItemCount() const {
    int visible = 0;
    for (int row = 0; row < ui->trashTreeWidget->topLevelItemCount(); ++row) {
        if (!ui->trashTreeWidget->topLevelItem(row)->isHidden()) {
            ++visible;
        }
    }
    return visible;
}

/**
 * Returns the selected items that are not hidden by the search, in list order
 */
QList<QTreeWidgetItem *> LocalTrashDialog::selectedVisibleItems() const {
    QList<QTreeWidgetItem *> items;
    for (int row = 0; row < ui->trashTreeWidget->topLevelItemCount(); ++row) {
        auto *item = ui->trashTreeWidget->topLevelItem(row);
        if (item->isSelected() && !item->isHidden()) {
            items << item;
        }
    }
    return items;
}

/**
 * Selects all notes that match the current search
 */
void LocalTrashDialog::selectAllVisibleItems() {
    {
        const QSignalBlocker blocker(ui->trashTreeWidget);
        Q_UNUSED(blocker)

        for (int row = 0; row < ui->trashTreeWidget->topLevelItemCount(); ++row) {
            auto *item = ui->trashTreeWidget->topLevelItem(row);
            item->setSelected(!item->isHidden());
        }
    }

    updatePreviewInfo();
}

void LocalTrashDialog::clearSelection() {
    ui->trashTreeWidget->clearSelection();
    updatePreviewInfo();
}

void LocalTrashDialog::onSelectAllCheckBoxClicked() {
    const int selected = static_cast<int>(selectedVisibleItems().count());

    // Deselect everything if all notes are selected, otherwise select all of them
    if (selected > 0 && selected == visibleItemCount()) {
        clearSelection();
    } else {
        selectAllVisibleItems();
    }
}

void LocalTrashDialog::on_searchLineEdit_textChanged(const QString &arg1) {
    if (arg1.isEmpty()) {
        ui->noteBrowser->searchWidget()->deactivate();
        ui->searchLineEdit->setFocus();
    }

    QTreeWidgetItem *firstMatchingItem = nullptr;

    for (int row = 0; row < ui->trashTreeWidget->topLevelItemCount(); ++row) {
        auto *item = ui->trashTreeWidget->topLevelItem(row);
        bool matches = item->text(0).contains(arg1, Qt::CaseInsensitive);

        if (!matches) {
            // Cache note text on demand to avoid reading every file on each keystroke
            QVariant text = item->data(0, NoteTextRole);
            if (!text.isValid()) {
                TrashItem trashItem = TrashItem::fetch(item->data(0, Qt::UserRole).toInt());
                text = trashItem.loadFileFromDisk();
                item->setData(0, NoteTextRole, text);
            }
            matches = text.toString().contains(arg1, Qt::CaseInsensitive);
        }

        if (!matches) {
            item->setSelected(false);
        }
        item->setHidden(!matches);
        if (matches && firstMatchingItem == nullptr) {
            firstMatchingItem = item;
        }
    }

    auto *current = ui->trashTreeWidget->currentItem();
    if (current == nullptr || current->isHidden()) {
        ui->trashTreeWidget->setCurrentItem(firstMatchingItem);
    }

    // Changing the current item may select it again in extended selection mode,
    // hidden notes must never be part of a bulk operation
    for (auto *item : ui->trashTreeWidget->selectedItems()) {
        if (item->isHidden()) {
            item->setSelected(false);
        }
    }
    on_trashTreeWidget_currentItemChanged(ui->trashTreeWidget->currentItem(), nullptr);
}

/**
 * Updates the preview header, the trashed note count and the action buttons
 */
void LocalTrashDialog::updatePreviewInfo() {
    const int total = ui->trashTreeWidget->topLevelItemCount();
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
    _removeButton->setEnabled(hasSelection);
    _restoreAction->setEnabled(hasSelection);
    _removeAction->setEnabled(hasSelection);
    _selectAllAction->setEnabled(visible > 0 && selected < visible);
    _clearSelectionAction->setEnabled(hasSelection);

    // Show how many notes a button will affect when more than one is selected
    _restoreButton->setText(selected > 1 ? tr("&Restore %n notes", "", selected) : tr("&Restore"));
    _removeButton->setText(selected > 1 ? tr("Re&move %n notes", "", selected) : tr("Re&move"));
    _restoreAction->setText(selected > 1 ? tr("Restore %n notes", "", selected) : tr("Restore"));
    _removeAction->setText(selected > 1 ? tr("Remove %n notes", "", selected) : tr("Remove"));

    const auto *item = ui->trashTreeWidget->currentItem();

    if (item != nullptr && !item->isHidden()) {
        ui->previewHeaderLabel->setText(item->text(0));
        ui->previewDetailLabel->setText(item->data(0, DetailRole).toString());
    } else {
        ui->previewHeaderLabel->setText(total == 0     ? tr("No trashed notes")
                                        : visible == 0 ? tr("No trashed note matches the search")
                                                       : tr("No trashed note selected"));
        ui->previewDetailLabel->clear();
    }

    ui->previewDetailLabel->setHidden(ui->previewDetailLabel->text().isEmpty());
}
