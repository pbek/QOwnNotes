#include "notebookmarkdialog.h"

#include <QDialogButtonBox>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidgetItem>
#include <algorithm>

#include "entities/note.h"
#include "ui_notebookmarkdialog.h"
#include "utils/gui.h"

NoteBookmarkDialog::NoteBookmarkDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::NoteBookmarkDialog) {
    ui->setupUi(this);
    afterSetupUI();

    // Set up table column widths
    ui->bookmarkTableWidget->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::ResizeToContents);
    ui->bookmarkTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->bookmarkTableWidget->horizontalHeader()->setSectionResizeMode(
        2, QHeaderView::ResizeToContents);

    // Add Jump and Delete action buttons
    QPushButton *jumpButton =
        ui->buttonBox->addButton(tr("Jump to bookmark"), QDialogButtonBox::ActionRole);
    jumpButton->setIcon(
        QIcon::fromTheme(QStringLiteral("go-next"),
                         QIcon(QStringLiteral(":/icons/breeze-qownnotes/16x16/go-next.svg"))));
    jumpButton->setToolTip(tr("Jump to the selected bookmark in the main window"));

    QPushButton *deleteButton =
        ui->buttonBox->addButton(tr("Delete bookmark"), QDialogButtonBox::ActionRole);
    deleteButton->setIcon(
        QIcon::fromTheme(QStringLiteral("edit-delete"),
                         QIcon(QStringLiteral(":/icons/breeze-qownnotes/16x16/edit-delete.svg"))));
    deleteButton->setToolTip(tr("Delete the selected bookmark"));

    connect(jumpButton, &QPushButton::clicked, this, &NoteBookmarkDialog::onJumpButtonClicked);
    connect(deleteButton, &QPushButton::clicked, this, &NoteBookmarkDialog::onDeleteButtonClicked);
    connect(ui->bookmarkTableWidget, &QTableWidget::cellDoubleClicked, this,
            &NoteBookmarkDialog::onBookmarkTableDoubleClicked);

    // Install event filter on the table to handle the Del key
    ui->bookmarkTableWidget->installEventFilter(this);
}

NoteBookmarkDialog::~NoteBookmarkDialog() { delete ui; }

/**
 * Populates the bookmark table from the given bookmarks hash
 */
void NoteBookmarkDialog::setBookmarks(const QHash<int, NoteHistoryItem> &bookmarks) {
    // Collect and sort by slot number for a predictable display order
    QList<int> slotNumbers = bookmarks.keys();
    std::sort(slotNumbers.begin(), slotNumbers.end());

    ui->bookmarkTableWidget->setRowCount(0);

    for (int slot : slotNumbers) {
        const NoteHistoryItem &item = bookmarks[slot];
        const Note note = item.getNote();

        if (!note.isFetched()) {
            // Skip bookmarks whose note no longer exists
            continue;
        }

        const int row = ui->bookmarkTableWidget->rowCount();
        ui->bookmarkTableWidget->insertRow(row);

        auto *slotItem = new QTableWidgetItem(QString::number(slot));
        slotItem->setData(Qt::UserRole, slot);
        slotItem->setTextAlignment(Qt::AlignCenter);

        auto *noteItem = new QTableWidgetItem(item.getNoteName());
        auto *posItem = new QTableWidgetItem(QString::number(item.getCursorPosition()));
        posItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ui->bookmarkTableWidget->setItem(row, 0, slotItem);
        ui->bookmarkTableWidget->setItem(row, 1, noteItem);
        ui->bookmarkTableWidget->setItem(row, 2, posItem);
    }
}

/**
 * Returns the slot number of the currently selected row, or -1 if none
 */
int NoteBookmarkDialog::selectedSlot() const {
    const QList<QTableWidgetItem *> selected = ui->bookmarkTableWidget->selectedItems();

    if (selected.isEmpty()) {
        return -1;
    }

    // The slot is stored in column 0 via UserRole
    const int row = ui->bookmarkTableWidget->row(selected.first());
    QTableWidgetItem *slotItem = ui->bookmarkTableWidget->item(row, 0);

    if (!slotItem) {
        return -1;
    }

    return slotItem->data(Qt::UserRole).toInt();
}

void NoteBookmarkDialog::onJumpButtonClicked() {
    const int slot = selectedSlot();

    if (slot >= 0) {
        emit jumpToBookmarkRequested(slot);
    }
}

void NoteBookmarkDialog::onDeleteButtonClicked() {
    const int slot = selectedSlot();

    if (slot < 0) {
        return;
    }

    // Ask the user to confirm before deleting the bookmark, with a "Don't ask again!" option
    const auto answer = Utils::Gui::question(
        this, tr("Delete bookmark"),
        tr("Are you sure you want to delete the bookmark at slot %1?").arg(slot),
        QStringLiteral("delete-note-bookmark"));

    if (answer == QMessageBox::Yes) {
        emit deleteBookmarkRequested(slot);
    }
}

void NoteBookmarkDialog::onBookmarkTableDoubleClicked(int /*row*/, int /*column*/) {
    onJumpButtonClicked();
}

/**
 * Intercepts key events on the bookmark table to allow deletion with the Del key
 */
bool NoteBookmarkDialog::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->bookmarkTableWidget && event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_Delete) {
            onDeleteButtonClicked();
            return true;
        }
    }

    return MasterDialog::eventFilter(watched, event);
}
