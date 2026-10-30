#include "nextclouddeckdialog.h"

#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QSignalBlocker>
#include <QTimeZone>
#include <QTimer>
#include <QVBoxLayout>

#include "mainwindow.h"
#include "services/metricsservice.h"
#include "services/nextclouddeckservice.h"
#include "services/settingsservice.h"
#ifdef INTEGRATION_TESTS
#include "widgets/qownnotesmarkdowntextedit.h"
// Test the dialog with the base editor without linking the full application editor.
#define QOwnNotesMarkdownTextEdit QMarkdownTextEdit
#endif
#include "ui_nextclouddeckdialog.h"
#ifdef INTEGRATION_TESTS
#undef QOwnNotesMarkdownTextEdit
#endif
#include "utils/gui.h"

NextcloudDeckDialog::NextcloudDeckDialog(QWidget *parent, Mode mode)
    : MasterDialog(parent), ui(new Ui::NextcloudDeckDialog), _mode(mode) {
    ui->setupUi(this);
    if (_mode == Mode::InsertLinks) {
        // Keep picker geometry separate from the full management dialog.
        setObjectName(QStringLiteral("NextcloudDeckLinkDialog"));
    }
    afterSetupUI();

    // Handle Enter explicitly in the search field and card list, without also activating
    // a default button (which could create a card immediately in management mode).
    setIgnoreReturnKey(true);

    setupUi();
    if (_mode == Mode::InsertLinks) {
        setWindowTitle(tr("Insert Nextcloud Deck card links"));
        ui->editFrame->hide();
        ui->saveButton->setShortcut(QKeySequence());
        ui->createAndInsertLinkButton->setShortcut(QKeySequence());
        ui->newItemEdit->setPlaceholderText(tr("Search cards"));
        ui->cardItemTreeWidget->setContextMenuPolicy(Qt::NoContextMenu);

        auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
        _insertLinksButton = buttons->addButton(tr("Insert links"), QDialogButtonBox::ActionRole);
        _insertLinksButton->setObjectName(QStringLiteral("insertLinksButton"));
        _insertLinksButton->setDefault(true);
        ui->gridLayout->addWidget(buttons, 1, 0, 1, 2);
        connect(_insertLinksButton, &QPushButton::clicked, this,
                &NextcloudDeckDialog::insertSelectedCardLinks);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
        connect(ui->cardItemTreeWidget, &QTreeWidget::itemSelectionChanged, this,
                &NextcloudDeckDialog::updateInsertLinksButton);
        updateInsertLinksButton();
    }
    _currentCard = NextcloudDeckService::Card();
    setSaveButtonsEnabled(false);
    ui->archiveCardButton->setEnabled(false);
    ui->deleteCardButton->setEnabled(false);
    ui->moveCardButton->setEnabled(false);
    ui->dueDateTimeCheckBox->setChecked(true);
    ui->newItemEdit->setFocus();

    // Set the default due date to one hour from now
    ui->dueDateTimeEdit->setDateTime(QDateTime::currentDateTime().addSecs(3600));

    // Hide currently unused UI elements
    ui->showDueTodayItemsOnlyCheckBox->setHidden(true);

    updateEditState();
    updateCardCountLabel();
}

NextcloudDeckDialog::~NextcloudDeckDialog() { delete ui; }

void NextcloudDeckDialog::setupUi() {
    setupMainSplitter();

    // Load the data from the server after the dialog is shown, so the loading progress is visible
    if (NextcloudDeckService(this).isEnabled()) {
        _initialLoadPending = true;
    } else {
        refreshUi();
    }

    Utils::Gui::initTreeWidgetHeaderOrderPersistence(
        ui->cardItemTreeWidget,
        QStringLiteral("NextcloudDeckDialog/cardItemTreeWidgetHeaderOrder"));

    ui->newItemEdit->installEventFilter(this);
    ui->cardItemTreeWidget->installEventFilter(this);
    ui->cardItemTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->newItemEdit->addAction(
        QIcon::fromTheme(QStringLiteral("edit-find"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/edit-find.svg"))),
        QLineEdit::LeadingPosition);
    ui->newItemEdit->setFocus();

    Utils::Gui::setSectionHeaderFont(ui->editHeaderLabel);
    Utils::Gui::setMutedTextColor(ui->editHintLabel);
    Utils::Gui::setMutedTextColor(ui->cardCountLabel);

    // Adding shortcuts not working when defined in the ui file
    //    auto *shortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+S")), this);
    //    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(on_saveButton_clicked()));
    //    shortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+I")), this);
    //    QObject::connect(shortcut, SIGNAL(activated()), this,
    //    SLOT(onSaveAndInsertButtonClicked())); shortcut = new
    //    QShortcut(QKeySequence(QStringLiteral("Ctrl+R")), this); QObject::connect(shortcut,
    //    SIGNAL(activated()), this, SLOT(on_removeButton_clicked()));

    /*
     * Set up the note button menu
     */
    auto *noteMenu = new QMenu(this);

    QAction *insertAction = noteMenu->addAction(tr("Save and insert into note"));
    insertAction->setIcon(QIcon::fromTheme(
        QStringLiteral("document-save"), QIcon(":icons/breeze-qownnotes/16x16/document-save.svg")));
    insertAction->setToolTip(
        tr("Save the current card item and insert a link to it into the current note"));
    //    connect(insertAction, SIGNAL(triggered()), this, SLOT(onSaveAndInsertButtonClicked()));

    //    ui->noteButton->setMenu(noteMenu);

    // Clean up unused menu to prevent memory leak
    // TODO: Remove this entire section when noteButton functionality is implemented
    delete noteMenu;
}

void NextcloudDeckDialog::showEvent(QShowEvent *event) {
    MasterDialog::showEvent(event);

    if (_initialLoadPending && !_initialLoadScheduled) {
        _initialLoadScheduled = true;

        // Let the dialog be painted first
        QTimer::singleShot(0, this, &NextcloudDeckDialog::initialLoad);
    }
}

void NextcloudDeckDialog::initialLoad() {
    _initialLoadPending = false;
    beginLoading(tr("Loading Nextcloud Deck cards…"));
    loadBoardStackComboBox();

    if (_pendingCardId > 0) {
        setCardId(_pendingCardId, _pendingBoardId);
    } else {
        reloadCardList();
    }

    endLoading();
}

/**
 * Shows the loading overlay, nested calls share the overlay of the outermost call
 */
void NextcloudDeckDialog::beginLoading(const QString &title) {
    if (_loadingDepth++ > 0) {
        return;
    }

    _loadingCanceled = false;

    if (!isVisible()) {
        return;
    }

    showLoadingOverlay(title, tr("Data is currently loaded from your Nextcloud server"), [this]() {
        _loadingCanceled = true;

        if (_activeDeckService) {
            _activeDeckService->cancelRequests();
        }
    });
}

void NextcloudDeckDialog::endLoading() {
    if (--_loadingDepth > 0) {
        return;
    }

    if (isLoading()) {
        hideLoadingOverlay();

        if (ui->newItemEdit->isEnabled()) {
            ui->newItemEdit->setFocus();
        }
    }
}

/**
 * Allows canceling the requests of the service and shows their download progress
 */
void NextcloudDeckDialog::trackDeckService(NextcloudDeckService &nextcloudDeckService) {
    _activeDeckService = &nextcloudDeckService;
    connect(&nextcloudDeckService, &NextcloudDeckService::downloadProgress, this,
            &MasterDialog::setLoadingProgress);
}

void NextcloudDeckDialog::on_saveButton_clicked() { storeCurrentCard(false); }

void NextcloudDeckDialog::on_createAndInsertLinkButton_clicked() {
    // Linking is only offered for new cards, existing cards can be linked from the context menu
    if (_currentCard.id > 0) {
        return;
    }

    storeCurrentCard(true);
}

/**
 * Creates or updates the card in the editor
 *
 * If insertLinkAndClose is true, a link to the stored card is inserted into the current note
 * and the dialog is closed, otherwise the dialog stays open with the stored card selected
 */
void NextcloudDeckDialog::storeCurrentCard(bool insertLinkAndClose) {
    setSaveButtonsEnabled(false);
    NextcloudDeckService nextcloudDeckService(this);

    if (_currentCard.id > 0) {
        nextcloudDeckService.setBoardAndStackIds(_currentCard.boardId, _currentCard.stackId);
    } else {
        configureDeckService(nextcloudDeckService);
    }

    auto *dateTime = new QDateTime(ui->dueDateTimeEdit->dateTime());
    dateTime->setTimeZone(QTimeZone::systemTimeZone());
    // We want to set the seconds to 0
    dateTime->setTime(dateTime->time().addSecs(0 - dateTime->time().second()));
    const QString &title = ui->titleLineEdit->text();

    // Check if we're updating an existing card or creating a new one
    int cardIdToUpdate = (_currentCard.id > 0) ? _currentCard.id : -1;

    int cardId = nextcloudDeckService.storeCard(
        title, ui->descriptionTextEdit->toPlainText(),
        ui->dueDateTimeCheckBox->isChecked() ? dateTime : nullptr, cardIdToUpdate,
        _currentCard.stackId, _currentCard.order, _currentCard.owner);

    // Clean up allocated dateTime to prevent memory leak
    delete dateTime;

    if (cardId < 1) {
        // Keep the dialog open and allow retrying after a failed request
        setSaveButtonsEnabled(!title.isEmpty());
        return;
    }

    if (insertLinkAndClose) {
        const auto linkText =
            QStringLiteral("[%1](%2)").arg(title, nextcloudDeckService.getCardLinkForId(cardId));

#ifndef INTEGRATION_TESTS
        MainWindow *mainWindow = MainWindow::instance();
        if (mainWindow != nullptr) {
            mainWindow->writeToNoteTextEdit(linkText);
        }
#endif

        close();
        return;
    }

    // Reload the card list and re-select the stored card, so it can be edited further
    reloadCardList();
    selectCardInList(cardId);

    ui->newItemEdit->clear();
    ui->newItemEdit->setFocus();
}

void NextcloudDeckDialog::setSaveButtonsEnabled(bool enabled) {
    ui->saveButton->setEnabled(enabled);
    ui->createAndInsertLinkButton->setEnabled(enabled);
}

/**
 * Deletes all currently selected cards after asking for confirmation
 */
void NextcloudDeckDialog::deleteSelectedCards() {
    const auto selectedItems = ui->cardItemTreeWidget->selectedItems();

    if (selectedItems.isEmpty()) {
        return;
    }

    const int count = selectedItems.count();
    const QString confirmMessage =
        count == 1 ? tr("Do you want to delete the selected Nextcloud Deck card?")
                   : tr("Do you want to delete %n Nextcloud Deck card(s)?", "", count);

    if (Utils::Gui::question(this, tr("Delete card(s)"), confirmMessage,
                             QStringLiteral("nextcloud-deck-delete-selected")) !=
        QMessageBox::Yes) {
        return;
    }

    NextcloudDeckService nextcloudDeckService(this);
    bool anyDeleted = false;

    for (const QTreeWidgetItem *item : selectedItems) {
        const int cardId = item->data(0, Qt::UserRole).toInt();

        if (cardId < 1) {
            continue;
        }

        configureDeckServiceForCard(nextcloudDeckService, cardId);

        if (nextcloudDeckService.deleteCard(cardId)) {
            anyDeleted = true;
        }
    }

    if (anyDeleted) {
        reloadCardList();
    }
}

void NextcloudDeckDialog::on_moveCardButton_clicked() { moveCards({_currentCard.id}); }

void NextcloudDeckDialog::moveCards(const QList<int> &cardIds) {
    if (cardIds.isEmpty()) {
        return;
    }

    for (int cardId : cardIds) {
        if (!_cards.contains(cardId) || _cards.value(cardId).archived) {
            return;
        }
    }

    // Moving uses the saved server card, not pending changes in the editor.
    if (cardIds.contains(_currentCard.id) &&
        (ui->titleLineEdit->text() != _currentCard.title ||
         ui->descriptionTextEdit->toPlainText() != _currentCard.description ||
         ui->dueDateTimeCheckBox->isChecked() != _currentCard.duedate.isValid() ||
         (ui->dueDateTimeCheckBox->isChecked() &&
          ui->dueDateTimeEdit->dateTime() != _currentCard.duedate))) {
        if (QMessageBox::question(this, tr("Move card(s)"),
                                  tr("The current card has unsaved changes. Discard them and move "
                                     "the saved card?"),
                                  QMessageBox::Yes | QMessageBox::Cancel,
                                  QMessageBox::Cancel) != QMessageBox::Yes) {
            return;
        }
    }

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Move card(s)"));
    dialog.setMinimumWidth(380);
    auto *layout = new QVBoxLayout(&dialog);
    auto *label = new QLabel(tr("Destination board / stack:"), &dialog);
    auto *destination = new QComboBox(&dialog);
    label->setBuddy(destination);
    layout->addWidget(label);
    layout->addWidget(destination);

    for (int i = 0; i < ui->boardStackComboBox->count(); ++i) {
        const int boardId = ui->boardStackComboBox->itemData(i, Qt::UserRole).toInt();
        const int stackId = ui->boardStackComboBox->itemData(i, Qt::UserRole + 1).toInt();
        bool alreadyInStack = true;
        for (int cardId : cardIds) {
            const auto card = _cards.value(cardId);
            alreadyInStack = alreadyInStack && card.boardId == boardId && card.stackId == stackId;
        }
        if (alreadyInStack) {
            continue;
        }
        const int index = destination->count();
        destination->addItem(ui->boardStackComboBox->itemText(i), boardId);
        destination->setItemData(index, stackId, Qt::UserRole + 1);
    }

    if (destination->count() == 0) {
        QMessageBox::information(this, tr("Move card(s)"),
                                 tr("There is no other board or stack available. Reload the "
                                    "boards and stacks and try again."));
        return;
    }

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setText(tr("Move"));
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    const int targetBoardId = destination->currentData(Qt::UserRole).toInt();
    const int targetStackId = destination->currentData(Qt::UserRole + 1).toInt();
    NextcloudDeckService service(this);
    QList<int> movedCardIds;
    for (int cardId : cardIds) {
        configureDeckServiceForCard(service, cardId);
        if (!service.moveCard(cardId, targetBoardId, targetStackId, movedCardIds.count())) {
            // Stop on failure and keep the source selected so remaining cards stay visible.
            reloadCardList();
            return;
        }
        movedCardIds.append(cardId);
    }

    selectBoardStack(targetBoardId, targetStackId);
    persistSelectedBoardAndStack();
    reloadCardList();
    selectCardInList(movedCardIds.first());
}

/**
 * Event filter to handle key presses on the card list tree widget
 */
bool NextcloudDeckDialog::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (obj == ui->cardItemTreeWidget) {
            if (_mode == Mode::InsertLinks &&
                (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)) {
                insertSelectedCardLinks();
                return true;
            }
            if (keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace) {
                if (_mode == Mode::Manage) {
                    deleteSelectedCards();
                }
                return true;
            }
        }
    }

    return MasterDialog::eventFilter(obj, event);
}

void NextcloudDeckDialog::on_add1HourButton_clicked() {
    ui->dueDateTimeEdit->setDateTime(ui->dueDateTimeEdit->dateTime().addSecs(3600));
}

void NextcloudDeckDialog::on_add10MinButton_clicked() {
    ui->dueDateTimeEdit->setDateTime(ui->dueDateTimeEdit->dateTime().addSecs(600));
}

void NextcloudDeckDialog::on_add1DayButton_clicked() {
    ui->dueDateTimeEdit->setDateTime(ui->dueDateTimeEdit->dateTime().addDays(1));
}

void NextcloudDeckDialog::on_sub10MinButton_clicked() {
    ui->dueDateTimeEdit->setDateTime(ui->dueDateTimeEdit->dateTime().addSecs(-600));
}

void NextcloudDeckDialog::on_sub1HourButton_clicked() {
    ui->dueDateTimeEdit->setDateTime(ui->dueDateTimeEdit->dateTime().addSecs(-3600));
}

void NextcloudDeckDialog::on_subd1DayButton_clicked() {
    ui->dueDateTimeEdit->setDateTime(ui->dueDateTimeEdit->dateTime().addDays(-1));
}

void NextcloudDeckDialog::on_titleLineEdit_textChanged(const QString &arg1) {
    setSaveButtonsEnabled(!arg1.isEmpty());
}

void NextcloudDeckDialog::on_dueDateTimeCheckBox_toggled(bool checked) {
    ui->dueDateTimeEdit->setEnabled(checked);
    ui->dueDateAdjustFrame->setEnabled(checked);
}

void NextcloudDeckDialog::setTitle(const QString &title) { ui->titleLineEdit->setText(title); }

void NextcloudDeckDialog::setupMainSplitter() {
    this->mainSplitter = new QSplitter(this);
    this->mainSplitter->setChildrenCollapsible(false);

    this->mainSplitter->addWidget(ui->selectFrame);
    this->mainSplitter->addWidget(ui->editFrame);

    // Only management mode uses two panes and its saved splitter sizes.
    if (_mode == Mode::Manage) {
        SettingsService settings;
        QByteArray state =
            settings.value(QStringLiteral("NextcloudDeckDialog/mainSplitterState")).toByteArray();

        // Give the editor a bit more room than the card list by default
        if (!this->mainSplitter->restoreState(state)) {
            this->mainSplitter->setStretchFactor(0, 2);
            this->mainSplitter->setStretchFactor(1, 3);
        }

        connect(this->mainSplitter, &QSplitter::splitterMoved, this, [this]() {
            SettingsService().setValue(QStringLiteral("NextcloudDeckDialog/mainSplitterState"),
                                       this->mainSplitter->saveState());
        });
    }

    // Place the splitter explicitly, so additional rows like the picker buttons can't overlap it
    ui->gridLayout->addWidget(this->mainSplitter, 0, 0, 1, 2);
    ui->gridLayout->setRowStretch(0, 1);
}

void NextcloudDeckDialog::refreshUi() { reloadDeckData(); }

void NextcloudDeckDialog::reloadDeckData() {
    beginLoading(tr("Loading Nextcloud Deck cards…"));
    loadBoardStackComboBox();
    reloadCardList();
    endLoading();
}

void NextcloudDeckDialog::loadBoardStackComboBox() {
    const int previousBoardId = selectedBoardId();
    const int previousStackId = selectedStackId();
    const CloudConnection cloudConnection = CloudConnection::currentCloudConnection();
    const int configuredBoardId = cloudConnection.getNextcloudDeckBoardId();
    const int configuredStackId = cloudConnection.getNextcloudDeckStackId();
    NextcloudDeckService nextcloudDeckService(this);

    {
        const QSignalBlocker blocker(ui->boardStackComboBox);

        if (!nextcloudDeckService.isEnabled()) {
            ui->boardStackComboBox->clear();
            updateDeckControlsEnabledState();
            return;
        }

        beginLoading(tr("Loading Nextcloud Deck boards…"));
        trackDeckService(nextcloudDeckService);
        const auto boards = nextcloudDeckService.getBoards();
        endLoading();

        // Keep the previous boards if the loading was canceled
        if (_loadingCanceled) {
            updateDeckControlsEnabledState();
            return;
        }

        ui->boardStackComboBox->clear();
        int targetIndex = -1;
        int fallbackIndex = -1;

        for (const auto &board : boards) {
            QHash<int, QString>::const_iterator it;

            for (it = board.stacks.constBegin(); it != board.stacks.constEnd(); ++it) {
                const int index = ui->boardStackComboBox->count();
                const int stackId = it.key();
                const QString comboText = tr("%1 / %2").arg(board.title, it.value());

                ui->boardStackComboBox->addItem(comboText);
                ui->boardStackComboBox->setItemData(index, board.id, Qt::UserRole);
                ui->boardStackComboBox->setItemData(index, stackId, Qt::UserRole + 1);

                if (previousBoardId == board.id && previousStackId == stackId) {
                    targetIndex = index;
                } else if (targetIndex == -1 && configuredBoardId == board.id &&
                           configuredStackId == stackId) {
                    fallbackIndex = index;
                }
            }
        }

        if (targetIndex == -1) {
            targetIndex = fallbackIndex;
        }

        if (targetIndex == -1 && ui->boardStackComboBox->count() > 0) {
            targetIndex = 0;
        }

        if (targetIndex >= 0) {
            ui->boardStackComboBox->setCurrentIndex(targetIndex);
        }
    }

    updateDeckControlsEnabledState();
}

void NextcloudDeckDialog::persistSelectedBoardAndStack() const {
    const int boardId = selectedBoardId();
    const int stackId = selectedStackId();

    if (boardId < 1 || stackId < 1) {
        return;
    }

    CloudConnection cloudConnection = CloudConnection::currentCloudConnection();

    if (!cloudConnection.isFetched()) {
        return;
    }

    cloudConnection.setNextcloudDeckBoardId(boardId);
    cloudConnection.setNextcloudDeckStackId(stackId);
}

bool NextcloudDeckDialog::selectBoardStack(int boardId, int stackId) {
    int targetIndex = -1;

    for (int i = 0; i < ui->boardStackComboBox->count(); ++i) {
        const int comboBoardId = ui->boardStackComboBox->itemData(i, Qt::UserRole).toInt();
        const int comboStackId = ui->boardStackComboBox->itemData(i, Qt::UserRole + 1).toInt();

        if (comboBoardId != boardId) {
            continue;
        }

        if (stackId < 1 || comboStackId == stackId) {
            targetIndex = i;
            break;
        }
    }

    if (targetIndex < 0) {
        return false;
    }

    const QSignalBlocker blocker(ui->boardStackComboBox);
    ui->boardStackComboBox->setCurrentIndex(targetIndex);
    return true;
}

bool NextcloudDeckDialog::selectBoardForCard(int boardId, int cardId) {
    if (!selectBoardStack(boardId)) {
        return false;
    }

    persistSelectedBoardAndStack();
    reloadCardList();

    if (_cards.contains(cardId)) {
        return true;
    }

    for (int i = 0; i < ui->boardStackComboBox->count(); ++i) {
        if (ui->boardStackComboBox->itemData(i, Qt::UserRole).toInt() != boardId) {
            continue;
        }

        if (i == ui->boardStackComboBox->currentIndex()) {
            continue;
        }

        {
            const QSignalBlocker blocker(ui->boardStackComboBox);
            ui->boardStackComboBox->setCurrentIndex(i);
        }

        persistSelectedBoardAndStack();
        reloadCardList();

        if (_cards.contains(cardId)) {
            return true;
        }
    }

    return false;
}

void NextcloudDeckDialog::reloadCardList() {
    ui->newItemEdit->clear();
    _cards.clear();
    ui->cardItemTreeWidget->clear();
    updateDeckControlsEnabledState();

    if (selectedBoardId() < 1 || selectedStackId() < 1) {
        resetEditFrameControls();
        return;
    }

    beginLoading(tr("Loading Nextcloud Deck cards…"));

    // Don't load the cards if the loading of the boards was canceled
    if (_loadingCanceled) {
        endLoading();
        return;
    }

    NextcloudDeckService nextcloudDeckService(this);
    configureDeckService(nextcloudDeckService);
    trackDeckService(nextcloudDeckService);
    _cards = nextcloudDeckService.getCards(ui->showArchivedCardsCheckBox->isChecked());
    endLoading();

    qDebug() << __func__ << " - 'cards': " << _cards;

    // Populate the tree widget with cards
    QList<QTreeWidgetItem *> items;    // Collect items for batch insertion (more efficient)
    const QColor mutedColor =
        ui->cardItemTreeWidget->palette().color(QPalette::Disabled, QPalette::Text);
    const QDateTime now = QDateTime::currentDateTime();

    for (const auto &card : _cards) {
        // Create item with explicit parent - ownership transferred to tree widget
        auto *item = new QTreeWidgetItem();

        // Set the summary (title) in the first column
        item->setText(0, card.title);

        // Show archived cards muted and in italics, so they stand out from active cards
        if (card.archived) {
            QFont font = item->font(0);
            font.setItalic(true);
            item->setFont(0, font);
            item->setFont(1, font);
            item->setForeground(0, QBrush(mutedColor));
        }

        // Set the due date in the second column
        if (card.duedate.isValid()) {
            item->setText(1, card.duedate.toString("yyyy-MM-dd hh:mm"));

            // Highlight due dates that are already in the past
            if (card.duedate < now) {
                // Use a muted red that is readable in light and dark mode
                item->setForeground(1, QBrush(QColor(0xd9, 0x53, 0x4f)));
                QFont font = item->font(1);
                font.setBold(true);
                item->setFont(1, font);
                item->setToolTip(1, tr("The due date is in the past"));
            } else if (card.duedate.date() == now.date()) {
                // Emphasize cards that are due later today
                QFont font = item->font(1);
                font.setBold(true);
                item->setFont(1, font);
                item->setToolTip(1, tr("The card is due today"));
            }
        } else {
            item->setText(1, tr("No due date"));
            item->setForeground(1, QBrush(mutedColor));
        }

        // Store the card ID as user data for later reference
        item->setData(0, Qt::UserRole, card.id);

        // Set tooltip with description if available
        if (!card.description.isEmpty()) {
            item->setToolTip(0, card.description);
        }

        items.append(item);
    }

    // Add all items at once - this ensures proper ownership transfer
    ui->cardItemTreeWidget->addTopLevelItems(items);

    if (Utils::Gui::hasTreeWidgetHeaderLayout(ui->cardItemTreeWidget)) {
        Utils::Gui::restoreTreeWidgetHeaderLayout(ui->cardItemTreeWidget);
    } else {
        // Auto-resize columns to content until the user customizes the layout.
        ui->cardItemTreeWidget->resizeColumnToContents(0);
        ui->cardItemTreeWidget->resizeColumnToContents(1);
    }

    updateCardCountLabel();
}

/**
 * Shows how many cards are in the list, and how many of them match the search
 */
void NextcloudDeckDialog::updateCardCountLabel() {
    const int total = ui->cardItemTreeWidget->topLevelItemCount();

    if (total == 0) {
        ui->cardCountLabel->clear();
        return;
    }

    if (ui->newItemEdit->text().isEmpty()) {
        ui->cardCountLabel->setText(tr("%n card(s)", "", total));
        return;
    }

    int visible = 0;
    for (int i = 0; i < total; ++i) {
        if (!ui->cardItemTreeWidget->topLevelItem(i)->isHidden()) {
            ++visible;
        }
    }

    ui->cardCountLabel->setText(tr("%1 of %n card(s)", "", total).arg(visible));
}

/**
 * Updates the editor header, hint and save button for the current editing state
 */
void NextcloudDeckDialog::updateEditState() {
    if (_mode != Mode::Manage) {
        return;
    }

    const bool isExistingCard = _currentCard.id > 0;
    const bool isNewCard = !isExistingCard && ui->editFrame->isEnabledTo(this);

    const QKeySequence primaryShortcut(QStringLiteral("Ctrl+S"));
    const QKeySequence secondaryShortcut(QStringLiteral("Ctrl+Shift+S"));
    const auto withShortcut = [](const QString &text, const QKeySequence &shortcut) {
        return QStringLiteral("%1 (%2)").arg(text, shortcut.toString(QKeySequence::NativeText));
    };

    // Setting the text resets the shortcut, so the shortcuts are set after the texts
    if (isExistingCard) {
        ui->editHeaderLabel->setText(_currentCard.archived ? tr("Edit archived card")
                                                           : tr("Edit card"));
        ui->saveButton->setText(tr("Save"));
        ui->saveButton->setToolTip(
            withShortcut(tr("Save the changes to the card"), primaryShortcut));
        ui->saveButton->setShortcut(primaryShortcut);
        ui->createAndInsertLinkButton->setShortcut(QKeySequence());
    } else {
        ui->editHeaderLabel->setText(isNewCard ? tr("New card") : tr("No card selected"));
        ui->saveButton->setText(tr("Create card"));
        ui->saveButton->setToolTip(
            withShortcut(tr("Create card and keep the dialog open"), secondaryShortcut));
        ui->saveButton->setShortcut(secondaryShortcut);
        ui->createAndInsertLinkButton->setToolTip(
            withShortcut(tr("Create card, insert a link to it into the current note and close "
                            "the dialog"),
                         primaryShortcut));
        ui->createAndInsertLinkButton->setShortcut(primaryShortcut);
    }

    // Existing cards can be linked from the context menu of the card list
    ui->createAndInsertLinkButton->setVisible(!isExistingCard);
    ui->editHintLabel->setVisible(!isExistingCard && !isNewCard);
}

void NextcloudDeckDialog::selectCardInList(int cardId) {
    if (cardId < 1) {
        return;
    }

    const auto item = Utils::Gui::getTreeWidgetItemWithUserData(ui->cardItemTreeWidget, cardId);

    if (item != nullptr) {
        ui->cardItemTreeWidget->setCurrentItem(item);
    }
}

void NextcloudDeckDialog::resetEditFrameControls() {
    ui->editFrame->setEnabled(false);
    ui->titleLineEdit->setText(QString());
    ui->descriptionTextEdit->setPlainText(QString());
    ui->dueDateTimeCheckBox->setChecked(false);
    ui->dueDateTimeEdit->setEnabled(false);
    ui->dueDateAdjustFrame->setEnabled(false);
    setSaveButtonsEnabled(false);
    ui->archiveCardButton->setEnabled(false);
    ui->deleteCardButton->setEnabled(false);
    _currentCard = NextcloudDeckService::Card();
    ui->moveCardButton->setEnabled(false);
    updateEditState();
}

int NextcloudDeckDialog::selectedBoardId() const {
    return ui->boardStackComboBox->currentData(Qt::UserRole).toInt();
}

int NextcloudDeckDialog::selectedStackId() const {
    return ui->boardStackComboBox->currentData(Qt::UserRole + 1).toInt();
}

void NextcloudDeckDialog::configureDeckService(NextcloudDeckService &nextcloudDeckService) const {
    nextcloudDeckService.setBoardAndStackIds(selectedBoardId(), selectedStackId());
}

void NextcloudDeckDialog::configureDeckServiceForCard(NextcloudDeckService &nextcloudDeckService,
                                                      int cardId) const {
    if (_cards.contains(cardId)) {
        const auto card = _cards.value(cardId);
        nextcloudDeckService.setBoardAndStackIds(card.boardId, card.stackId);
        return;
    }

    configureDeckService(nextcloudDeckService);
}

void NextcloudDeckDialog::updateDeckControlsEnabledState() {
    const bool hasSelection = selectedBoardId() > 0 && selectedStackId() > 0;

    ui->newItemEdit->setEnabled(hasSelection);
    ui->cardItemTreeWidget->setEnabled(hasSelection);
    ui->showArchivedCardsCheckBox->setEnabled(hasSelection);
    ui->reloadCardListButton->setEnabled(hasSelection);
}

void NextcloudDeckDialog::jumpToCard(int id) {
    if (_cards.contains(id)) {
        _currentCard = _cards[id];

        ui->titleLineEdit->setText(_currentCard.title);
        ui->descriptionTextEdit->setPlainText(_currentCard.description);

        // Set the due date
        if (_currentCard.duedate.isValid()) {
            ui->dueDateTimeCheckBox->setChecked(true);
            ui->dueDateTimeEdit->setDateTime(_currentCard.duedate);
        } else {
            ui->dueDateTimeCheckBox->setChecked(false);
        }

        setSaveButtonsEnabled(true);
        ui->archiveCardButton->setEnabled(true);
        ui->deleteCardButton->setEnabled(true);
        ui->moveCardButton->setEnabled(!_currentCard.archived);
        ui->editFrame->setEnabled(true);
        updateEditState();
    } else {
        resetEditFrameControls();
    }
}

void NextcloudDeckDialog::on_cardItemTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                                   QTreeWidgetItem *previous) {
    Q_UNUSED(previous)
    if (_mode == Mode::InsertLinks) {
        return;
    }

    // in case all items were removed
    if (current == nullptr) {
        resetEditFrameControls();
        return;
    }

    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("deck/card/changed"));

    const int id = current->data(0, Qt::UserRole).toInt();

    // Find the card in our hash using the ID
    jumpToCard(id);
}

void NextcloudDeckDialog::setCardId(const int id, const int boardId) {
    if (id < 1) {
        return;
    }

    // Select the card after the initial loading
    if (_initialLoadPending) {
        _pendingCardId = id;
        _pendingBoardId = boardId;
        return;
    }

    if (boardId > 0) {
        selectBoardForCard(boardId, id);
    } else if (ui->cardItemTreeWidget->topLevelItemCount() == 0) {
        reloadCardList();
    }

    selectCardInList(id);
}

void NextcloudDeckDialog::on_newItemEdit_textChanged(const QString &arg1) {
    Utils::Gui::searchForTextInTreeWidget(
        ui->cardItemTreeWidget, arg1,
        Utils::Gui::TreeWidgetSearchFlags(Utils::Gui::TreeWidgetSearchFlag::EveryWordSearch));
    updateInsertLinksButton();
    updateCardCountLabel();
}

void NextcloudDeckDialog::on_newItemEdit_returnPressed() {
    if (_mode == Mode::InsertLinks) {
        insertSelectedCardLinks();
        return;
    }
    // The description and due date were changed by the user if they differ from the currently
    // selected card, in that case they must not be reset when creating a new card
    const QString description = ui->descriptionTextEdit->toPlainText();
    const bool dueDateEnabled = ui->dueDateTimeCheckBox->isChecked();
    const QDateTime dueDateTime = ui->dueDateTimeEdit->dateTime();
    const bool keepDescription = description != _currentCard.description;
    const bool keepDueDate = dueDateEnabled != _currentCard.duedate.isValid() ||
                             (dueDateEnabled && dueDateTime != _currentCard.duedate);

    resetEditFrameControls();
    ui->editFrame->setEnabled(true);
    updateEditState();
    ui->titleLineEdit->setText(ui->newItemEdit->text());
    ui->newItemEdit->clear();

    if (keepDescription) {
        ui->descriptionTextEdit->setPlainText(description);
    }

    ui->descriptionTextEdit->setFocus();

    // Keep an already entered due date, otherwise default to one hour from now
    ui->dueDateTimeEdit->setDateTime(keepDueDate ? dueDateTime
                                                 : QDateTime::currentDateTime().addSecs(3600));
    ui->dueDateTimeCheckBox->setChecked(keepDueDate ? dueDateEnabled : true);
}

void NextcloudDeckDialog::on_deleteCardButton_clicked() {
    if (Utils::Gui::question(this, tr("Delete card"),
                             tr("Do you want to delete the current Nextcloud Deck card?"),
                             QStringLiteral("nextcloud-deck-delete")) != QMessageBox::Yes) {
        return;
    }

    NextcloudDeckService nextcloudDeckService(this);
    bool isSuccess = nextcloudDeckService.deleteCard(_currentCard.id);

    if (isSuccess) {
        reloadCardList();
    }
}

void NextcloudDeckDialog::on_archiveCardButton_clicked() {
    if (Utils::Gui::question(this, tr("Archive card"),
                             tr("Do you want to archive the current Nextcloud Deck card?"),
                             QStringLiteral("nextcloud-deck-archive")) != QMessageBox::Yes) {
        return;
    }

    NextcloudDeckService nextcloudDeckService(this);
    const bool isSuccess = nextcloudDeckService.archiveCard(_currentCard.id);

    if (isSuccess) {
        reloadCardList();
    }
}

void NextcloudDeckDialog::on_boardStackComboBox_currentIndexChanged(int index) {
    Q_UNUSED(index)

    persistSelectedBoardAndStack();
    resetEditFrameControls();
    reloadCardList();
}

void NextcloudDeckDialog::openUrlInBrowserForItem(const QTreeWidgetItem *item) {
    if (item == nullptr) {
        return;
    }

    const int cardId = item->data(0, Qt::UserRole).toInt();
    NextcloudDeckService nextcloudDeckService(this);
    configureDeckServiceForCard(nextcloudDeckService, cardId);
    QDesktopServices::openUrl(QUrl(nextcloudDeckService.getCardLinkForId(cardId)));
}

void NextcloudDeckDialog::on_cardItemTreeWidget_itemDoubleClicked(QTreeWidgetItem *item,
                                                                  int column) {
    Q_UNUSED(column)

    if (_mode == Mode::InsertLinks) {
        insertSelectedCardLinks();
    } else {
        openUrlInBrowserForItem(item);
    }
}

void NextcloudDeckDialog::on_cardItemTreeWidget_customContextMenuRequested(const QPoint &pos) {
    if (_mode == Mode::InsertLinks) {
        return;
    }
    const QPoint globalPos = ui->cardItemTreeWidget->mapToGlobal(pos);
    QMenu menu;

    QAction *openUrlAction = menu.addAction(tr("&Open card in browser"));
    openUrlAction->setIcon(QIcon::fromTheme(QStringLiteral("text-html"),
                                            QIcon(":icons/breeze-qownnotes/16x16/text-html.svg")));
    QAction *cardLinkAction = menu.addAction(tr("&Add card link to note"));
    cardLinkAction->setIcon(QIcon::fromTheme(
        QStringLiteral("insert-link"), QIcon(":icons/breeze-qownnotes/16x16/insert-link.svg")));
    QAction *searchInNotesAction = menu.addAction(tr("&Search for card link in notes"));
    searchInNotesAction->setIcon(QIcon::fromTheme(
        QStringLiteral("edit-find"), QIcon(":icons/breeze-qownnotes/16x16/edit-find.svg")));

    // Add delete action for selected cards
    const int selectedCount = ui->cardItemTreeWidget->selectedItems().count();
    QAction *deleteCardsAction = nullptr;
    QAction *moveCardsAction = nullptr;

    if (selectedCount > 0) {
        menu.addSeparator();
        moveCardsAction = menu.addAction(tr("&Move %n card(s)…", "", selectedCount));
        moveCardsAction->setIcon(QIcon::fromTheme(
            QStringLiteral("edit-move"), QIcon(":icons/breeze-qownnotes/16x16/edit-move.svg")));
        for (const auto *selectedCard : ui->cardItemTreeWidget->selectedItems()) {
            if (_cards.value(selectedCard->data(0, Qt::UserRole).toInt()).archived) {
                moveCardsAction->setEnabled(false);
                break;
            }
        }
        const QString deleteLabel = tr("&Delete %n card(s)", "", selectedCount);
        deleteCardsAction = menu.addAction(deleteLabel);
        deleteCardsAction->setIcon(QIcon::fromTheme(
            QStringLiteral("edit-delete"), QIcon(":icons/breeze-qownnotes/16x16/edit-delete.svg")));
    }

    QAction *selectedItem = menu.exec(globalPos);

    if (selectedItem == nullptr) {
        return;
    }

    QTreeWidgetItem *item = ui->cardItemTreeWidget->currentItem();

    if (selectedItem == openUrlAction) {
        openUrlInBrowserForItem(item);
    } else if (selectedItem == cardLinkAction) {
        addCardLinkToCurrentNote(item);
    } else if (selectedItem == searchInNotesAction) {
        searchLinkInNotes(item);
    } else if (deleteCardsAction != nullptr && selectedItem == deleteCardsAction) {
        deleteSelectedCards();
    } else if (moveCardsAction != nullptr && selectedItem == moveCardsAction) {
        QList<int> cardIds;
        for (const auto *selectedCard : ui->cardItemTreeWidget->selectedItems()) {
            cardIds.append(selectedCard->data(0, Qt::UserRole).toInt());
        }
        moveCards(cardIds);
    }
}

void NextcloudDeckDialog::searchLinkInNotes(QTreeWidgetItem *item) {
    const int cardId = item->data(0, Qt::UserRole).toInt();

    if (cardId < 1) {
        return;
    }

    NextcloudDeckService nextcloudDeckService(this);
    configureDeckServiceForCard(nextcloudDeckService, cardId);
    const auto link = nextcloudDeckService.getCardLinkForId(cardId);
    Q_EMIT searchInNotes(link);
    close();
}

void NextcloudDeckDialog::on_showArchivedCardsCheckBox_toggled(bool checked) {
    Q_UNUSED(checked)
    // Preserve the currently selected card when toggling archived cards view
    int selectedCardId = _currentCard.id;
    reloadCardList();
    selectCardInList(selectedCardId);
}

void NextcloudDeckDialog::on_reloadCardListButton_clicked() {
    const int selectedCardId = _currentCard.id;
    reloadDeckData();
    selectCardInList(selectedCardId);
}

void NextcloudDeckDialog::addCardLinkToCurrentNote(const QTreeWidgetItem *item) {
    const int cardId = item->data(0, Qt::UserRole).toInt();
    if (cardId < 1) {
        return;
    }

    const auto title = item->text(0);
    NextcloudDeckService nextcloudDeckService(this);
    configureDeckServiceForCard(nextcloudDeckService, cardId);
    const auto linkText =
        QStringLiteral("[%1](%2)").arg(title, nextcloudDeckService.getCardLinkForId(cardId));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        mainWindow->writeToNoteTextEdit(linkText);
    }
#endif

    close();
}

void NextcloudDeckDialog::updateInsertLinksButton() {
    if (_insertLinksButton == nullptr) {
        return;
    }

    bool hasSelection = false;
    for (const auto *item : ui->cardItemTreeWidget->selectedItems()) {
        if (!item->isHidden() && item->data(0, Qt::UserRole).toInt() > 0) {
            hasSelection = true;
            break;
        }
    }
    _insertLinksButton->setEnabled(hasSelection);
}

void NextcloudDeckDialog::insertSelectedCardLinks() {
    QStringList links;
    NextcloudDeckService service(this);
    // Insert in visible list order, ignoring selections hidden by the search filter.
    for (int i = 0; i < ui->cardItemTreeWidget->topLevelItemCount(); ++i) {
        const auto *item = ui->cardItemTreeWidget->topLevelItem(i);
        const int cardId = item->data(0, Qt::UserRole).toInt();
        if (!item->isSelected() || item->isHidden() || cardId < 1) {
            continue;
        }
        configureDeckServiceForCard(service, cardId);
        links.append(
            QStringLiteral("[%1](%2)").arg(item->text(0), service.getCardLinkForId(cardId)));
    }

    if (!links.isEmpty()) {
        Q_EMIT cardLinksSelected(links.join(QLatin1Char('\n')));
        accept();
    }
}
