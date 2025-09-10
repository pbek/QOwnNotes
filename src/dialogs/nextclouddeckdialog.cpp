#include "nextclouddeckdialog.h"

#include <QMenu>
#include <QShortcut>
#include <QTimeZone>

#include "mainwindow.h"
#include "services/metricsservice.h"
#include "services/nextclouddeckservice.h"
#include "services/settingsservice.h"
#include "ui_nextclouddeckdialog.h"
#include "utils/gui.h"

NextcloudDeckDialog::NextcloudDeckDialog(QWidget *parent, bool listMode)
    : MasterDialog(parent), ui(new Ui::NextcloudDeckDialog) {
    Q_UNUSED(listMode)
    ui->setupUi(this);
    afterSetupUI();
    setupUi();
    _currentCard = NextcloudDeckService::Card();
    ui->saveButton->setEnabled(false);
    ui->dueDateTimeCheckBox->setChecked(true);
    ui->newItemEdit->setFocus();

    // Set default due date to one hour from now
    ui->dueDateTimeEdit->setDateTime(QDateTime::currentDateTime().addSecs(3600));

    // Hide currently unused UI elements
    ui->showDueTodayItemsOnlyCheckBox->setHidden(true);
}

NextcloudDeckDialog::~NextcloudDeckDialog() { delete ui; }

void NextcloudDeckDialog::setupUi() {
    setupMainSplitter();
    refreshUi();

    ui->newItemEdit->installEventFilter(this);
    ui->cardItemTreeWidget->installEventFilter(this);
    ui->newItemEdit->setFocus();

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
        tr("Save the current todo item and insert a link"
           " to it into the current note"));
    //    connect(insertAction, SIGNAL(triggered()), this, SLOT(onSaveAndInsertButtonClicked()));

    QAction *importAction = noteMenu->addAction(tr("Import as note"));
    importAction->setIcon(
        QIcon::fromTheme(QStringLiteral("document-import"),
                         QIcon(":icons/breeze-qownnotes/16x16/document-import.svg")));
    importAction->setToolTip(tr("Import the current todo item as new note"));
    //    connect(importAction, SIGNAL(triggered()), this, SLOT(onImportAsNoteButtonClicked()));

    //    ui->noteButton->setMenu(noteMenu);

    // Clean up unused menu to prevent memory leak
    // TODO: Remove this entire section when noteButton functionality is implemented
    delete noteMenu;

    /*
     * Set up the reload button menu
     */
    auto *reloadMenu = new QMenu(this);

    QAction *reloadAction = reloadMenu->addAction(tr("Reload from server"));
    reloadAction->setIcon(QIcon::fromTheme(
        QStringLiteral("view-refresh"), QIcon(":icons/breeze-qownnotes/16x16/view-refresh.svg")));
    reloadAction->setToolTip(tr("Reload cards from server"));
    connect(reloadAction, SIGNAL(triggered()), this, SLOT(reloadCardList()));

    // Explicitly transfer ownership to the button (Qt 5.7 compatibility)
    ui->reloadCardListButton->setMenu(reloadMenu);
    // Menu ownership is now transferred to the button, no manual deletion needed
}

void NextcloudDeckDialog::on_saveButton_clicked() {
    ui->saveButton->setEnabled(false);
    NextcloudDeckService nextcloudDeckService(this);

    auto *dateTime = new QDateTime(ui->dueDateTimeEdit->dateTime());
    dateTime->setTimeZone(QTimeZone::systemTimeZone());
    // We want to set the seconds to 0
    dateTime->setTime(dateTime->time().addSecs(0 - dateTime->time().second()));
    const QString &title = ui->titleLineEdit->text();

    // Check if we're updating an existing card or creating a new one
    int cardIdToUpdate = (_currentCard.id > 0) ? _currentCard.id : -1;

    int cardId = nextcloudDeckService.storeCard(
        title, ui->descriptionTextEdit->toPlainText(),
        ui->dueDateTimeCheckBox->isChecked() ? dateTime : nullptr, cardIdToUpdate);

    // Clean up allocated dateTime to prevent memory leak
    delete dateTime;

    if (cardId > 0) {
        auto linkText =
            QStringLiteral("[%1](%2)").arg(title, nextcloudDeckService.getCardLinkForId(cardId));

#ifndef INTEGRATION_TESTS
        // Only insert the link if we're creating a new card, not updating an existing one
        if (cardIdToUpdate == -1) {
            MainWindow *mainWindow = MainWindow::instance();
            if (mainWindow != nullptr) {
                mainWindow->activeNoteTextEdit()->insertPlainText(linkText);
            }
        }
#endif

        // Reload the card list to reflect the changes
        reloadCardList();

        ui->newItemEdit->clear();
        ui->newItemEdit->setFocus();
    }

    // Close the dialog if we just created a new card and not updated an existing one
    if (cardIdToUpdate == -1) {
        close();
    }
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
    ui->saveButton->setEnabled(!arg1.isEmpty());
}

void NextcloudDeckDialog::on_dueDateTimeCheckBox_toggled(bool checked) {
    ui->dueDateTimeEdit->setEnabled(checked);
}

void NextcloudDeckDialog::setTitle(const QString &title) { ui->titleLineEdit->setText(title); }

void NextcloudDeckDialog::setupMainSplitter() {
    this->mainSplitter = new QSplitter(this);

    this->mainSplitter->addWidget(ui->selectFrame);
    this->mainSplitter->addWidget(ui->editFrame);

    // Restore splitter sizes
    SettingsService settings;
    QByteArray state =
        settings.value(QStringLiteral("NextcloudDeckDialog/mainSplitterState")).toByteArray();
    this->mainSplitter->restoreState(state);

    ui->gridLayout->layout()->addWidget(this->mainSplitter);
}

void NextcloudDeckDialog::refreshUi() { reloadCardList(); }

void NextcloudDeckDialog::reloadCardList() {
    NextcloudDeckService nextcloudDeckService(this);
    _cards = nextcloudDeckService.getCards();

    qDebug() << __func__ << " - 'cards': " << _cards;

    // Clear existing items (this should delete all child items)
    ui->cardItemTreeWidget->clear();

    // Populate the tree widget with cards
    QList<QTreeWidgetItem *> items;    // Collect items for batch insertion (more efficient)

    for (const auto &card : _cards) {
        // Create item with explicit parent - ownership transferred to tree widget
        auto *item = new QTreeWidgetItem();

        // Set the summary (title) in the first column
        item->setText(0, card.title);

        // Set the due date in the second column
        if (card.duedate.isValid()) {
            item->setText(1, card.duedate.toString("yyyy-MM-dd hh:mm"));
        } else {
            item->setText(1, tr("No due date"));
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

    // Auto-resize columns to content
    ui->cardItemTreeWidget->resizeColumnToContents(0);
    ui->cardItemTreeWidget->resizeColumnToContents(1);
}

void NextcloudDeckDialog::resetEditFrameControls() {
    ui->editFrame->setEnabled(false);
    ui->titleLineEdit->setText(QString());
    ui->descriptionTextEdit->setPlainText(QString());
    ui->dueDateTimeCheckBox->setChecked(false);
    ui->dueDateTimeEdit->setEnabled(false);
    ui->saveButton->setEnabled(false);
    _currentCard = NextcloudDeckService::Card();
}

void NextcloudDeckDialog::on_cardItemTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                                   QTreeWidgetItem *previous) {
    Q_UNUSED(previous)

    // in case all items were removed
    if (current == nullptr) {
        resetEditFrameControls();
        return;
    }

    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("deck/card/changed"));

    int id = current->data(0, Qt::UserRole).toInt();

    // Find the card in our hash using the ID
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

        ui->saveButton->setEnabled(true);
        ui->editFrame->setEnabled(true);
    } else {
        resetEditFrameControls();
    }
}

void NextcloudDeckDialog::on_newItemEdit_textChanged(const QString &arg1) {
    Utils::Gui::searchForTextInTreeWidget(
        ui->cardItemTreeWidget, arg1,
        Utils::Gui::TreeWidgetSearchFlags(Utils::Gui::TreeWidgetSearchFlag::EveryWordSearch));
}

void NextcloudDeckDialog::on_newItemEdit_returnPressed() {
    resetEditFrameControls();
    ui->editFrame->setEnabled(true);
    ui->titleLineEdit->setText(ui->newItemEdit->text());
    ui->descriptionTextEdit->setFocus();
    ui->newItemEdit->clear();
    // Set default due date to one hour from now
    ui->dueDateTimeEdit->setDateTime(QDateTime::currentDateTime().addSecs(3600));
    ui->dueDateTimeCheckBox->setChecked(true);
}
