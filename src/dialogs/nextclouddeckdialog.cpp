#include "nextclouddeckdialog.h"

#include <QMenu>
#include <QShortcut>
#include <QTimeZone>

#include "mainwindow.h"
#include "services/nextclouddeckservice.h"
#include "services/settingsservice.h"
#include "ui_nextclouddeckdialog.h"

NextcloudDeckDialog::NextcloudDeckDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::NextcloudDeckDialog) {
    ui->setupUi(this);
    afterSetupUI();
    setupUi();

    ui->dueDateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->saveButton->setEnabled(false);
    ui->dueDateTimeCheckBox->setChecked(true);
    ui->titleLineEdit->setFocus();
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

    /*
     * Set up the reload button menu
     */
    auto *reloadMenu = new QMenu(this);

    QAction *reloadAction = reloadMenu->addAction(tr("Reload from server"));
    reloadAction->setIcon(QIcon::fromTheme(
        QStringLiteral("view-refresh"), QIcon(":icons/breeze-qownnotes/16x16/view-refresh.svg")));
    reloadAction->setToolTip(tr("Reload cards from server"));
    connect(reloadAction, SIGNAL(triggered()), this, SLOT(reloadCardList()));

    ui->reloadCardListButton->setMenu(reloadMenu);
}

void NextcloudDeckDialog::on_saveButton_clicked() {
    ui->saveButton->setEnabled(false);
    NextcloudDeckService nextcloudDeckService(this);

    auto *dateTime = new QDateTime(ui->dueDateTimeEdit->dateTime());
    dateTime->setTimeZone(QTimeZone::systemTimeZone());
    // We want to set the seconds to 0
    dateTime->setTime(dateTime->time().addSecs(0 - dateTime->time().second()));
    const QString &title = ui->titleLineEdit->text();
    int cardId =
        nextcloudDeckService.createCard(title, ui->descriptionTextEdit->toPlainText(),
                                        ui->dueDateTimeCheckBox->isChecked() ? dateTime : nullptr);

    if (cardId > 0) {
        auto linkText =
            QStringLiteral("[%1](%2)").arg(title, nextcloudDeckService.getCardLinkForId(cardId));

#ifndef INTEGRATION_TESTS
        MainWindow *mainWindow = MainWindow::instance();
        if (mainWindow != nullptr) {
            mainWindow->activeNoteTextEdit()->insertPlainText(linkText);
        }
#endif
    }

    close();
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
    // TODO: Show select frame
    ui->selectFrame->setVisible(false);
    return;

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
    auto cards = nextcloudDeckService.getCards();

    qDebug() << __func__ << " - 'cards': " << cards;

    // Clear existing items
    ui->cardItemTreeWidget->clear();

    // Populate the tree widget with cards
    for (const auto &card : cards) {
        auto *item = new QTreeWidgetItem(ui->cardItemTreeWidget);

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
    }

    // Auto-resize columns to content
    ui->cardItemTreeWidget->resizeColumnToContents(0);
    ui->cardItemTreeWidget->resizeColumnToContents(1);
}
