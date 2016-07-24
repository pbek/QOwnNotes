#include "entities/calendaritem.h"
#include "services/owncloudservice.h"
#include "dialogs/tododialog.h"
#include "ui_tododialog.h"
#include <QSettings>
#include <QMessageBox>
#include <QKeyEvent>
#include <QShortcut>
#include <services/metricsservice.h>

TodoDialog::TodoDialog(MainWindow *mainWindow, QString taskUid,
                       QWidget *parent) :
        MasterDialog(parent),
        ui(new Ui::TodoDialog) {
    _mainWindow = mainWindow;
    ui->setupUi(this);
    setupUi();

    // init the description edit search frame
    ui->descriptionEdit->initSearchFrame(ui->descriptionEditSearchFrame);

    QString selectedText =
            _mainWindow->activeNoteTextEdit()->textCursor().selectedText();

    // insert the selected note text in the new item edit
    if (!selectedText.isEmpty()) {
        ui->newItemEdit->setText(selectedText);
    }

    if (!taskUid.isEmpty()) {
        CalendarItem calendarItem = CalendarItem::fetchByUid(taskUid);
        qDebug() << __func__ << " - 'calendarItem': " << calendarItem;

        if (calendarItem.exists()) {
            // set a calendar item uid to jump to later on
            _jumpToCalendarItemUid = taskUid;

            QString calendar = calendarItem.getCalendar();
            // if the calendar of the calendar item isn't the current one we
            // have to switch to it
            if (ui->todoListSelector->currentText() != calendar) {
                ui->todoListSelector->setCurrentText(calendar);
            } else {
                // jump to the correct todo list item
                jumpToTodoListItem();
            }
        }
    }
}

TodoDialog::~TodoDialog() {
    delete ui;
}

void TodoDialog::setupUi() {
    setupMainSplitter();
    loadTodoListData();

    ui->todoItemLoadingProgressBar->hide();

    QSettings settings;

    {
        const QSignalBlocker blocker(ui->showCompletedItemsCheckBox);
        Q_UNUSED(blocker);

        bool showCompletedItems =
                settings.value("TodoDialog/showCompletedItems").toBool();
        ui->showCompletedItemsCheckBox->setChecked(showCompletedItems);
    }

    int index = CalendarItem::getCurrentCalendarIndex();

    if (index >= 0) {
        const QSignalBlocker blocker(ui->todoListSelector);
        Q_UNUSED(blocker);

        // set the index of the todo list selector if we found it
        ui->todoListSelector->setCurrentIndex(index);
    } else {
        // if we didn't find the index store the new current item
        settings.setValue("TodoDialog/todoListSelectorSelectedItem",
                          ui->todoListSelector->currentText());
    }

    // hide the reminder date time select
    ui->reminderDateTimeEdit->hide();

    // now load the todo list items
    reloadTodoList();

    ui->newItemEdit->installEventFilter(this);
    ui->todoList->installEventFilter(this);
    ui->reminderDateTimeEdit->installEventFilter(this);

    ui->newItemEdit->setFocus();

    // adding shortcuts, that weren't working when defined in the ui file
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    QObject::connect(shortcut, SIGNAL(activated()),
                     this, SLOT(on_saveButton_clicked()));
    shortcut = new QShortcut(QKeySequence("Ctrl+I"), this);
    QObject::connect(shortcut, SIGNAL(activated()),
                     this, SLOT(on_saveAndInsertButton_clicked()));
    shortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
    QObject::connect(shortcut, SIGNAL(activated()),
                     this, SLOT(on_removeButton_clicked()));

}

void TodoDialog::setupMainSplitter() {
    this->mainSplitter = new QSplitter;

    this->mainSplitter->addWidget(ui->selectFrame);
    this->mainSplitter->addWidget(ui->editFrame);

    // restore splitter sizes
    QSettings settings;
    QByteArray state =
            settings.value("TodoDialog/mainSplitterState").toByteArray();
    this->mainSplitter->restoreState(state);

    ui->gridLayout->layout()->addWidget(this->mainSplitter);
}

/**
 * @brief Loads the calendar items from the settings to the todo list selector
 */
void TodoDialog::loadTodoListData() {
    const QSignalBlocker blocker(ui->todoListSelector);
    Q_UNUSED(blocker);

    QSettings settings;
    ui->todoListSelector->clear();
    ui->todoListSelector->addItems(
            settings.value("ownCloud/todoCalendarEnabledList").toStringList());
}

/**
 * @brief Fetches the items of the current todo list from ownCloud
 */
void TodoDialog::reloadTodoList() {
    ui->todoItemLoadingProgressBar->setValue(0);
    ui->todoItemLoadingProgressBar->show();
    OwnCloudService *ownCloud = new OwnCloudService(this);
    ownCloud->todoGetTodoList(ui->todoListSelector->currentText(), this);
}

/**
 * Reloads the todo list from the SQLite database
 */
void TodoDialog::reloadTodoListItems() {
    QList<CalendarItem> calendarItemList = CalendarItem::fetchAllByCalendar(
            ui->todoListSelector->currentText());

    int itemCount = calendarItemList.count();
    MetricsService::instance()->sendEventIfEnabled(
            "todo/list/loaded",
            "todo",
            "todo list loaded",
            QString::number(itemCount) + " todo items",
            itemCount);

    {
        const QSignalBlocker blocker(ui->todoList);
        Q_UNUSED(blocker);

        ui->todoList->clear();

        QListIterator<CalendarItem> itr(calendarItemList);
        while (itr.hasNext()) {
            CalendarItem calItem = itr.next();

            // skip completed items if the "show completed items" checkbox
            // is not checked
            if (!ui->showCompletedItemsCheckBox->checkState()) {
                if (calItem.isCompleted()) {
                    continue;
                }
            }

            QString uid = calItem.getUid();

            // skip items that were not fully loaded yet
            if (uid == "") {
                continue;
            }

            QListWidgetItem *item = new QListWidgetItem(calItem.getSummary());
            item->setData(Qt::UserRole, uid);
            item->setCheckState(
                    calItem.isCompleted() ? Qt::Checked : Qt::Unchecked);
            item->setFlags(
                    Qt::ItemIsDragEnabled |
                    Qt::ItemIsDropEnabled |
                    Qt::ItemIsEnabled |
                    Qt::ItemIsUserCheckable |
                    Qt::ItemIsSelectable);

            ui->todoList->addItem(item);
        }
    }

    // set the current row of the todo list to the first row
    jumpToTodoListItem();

    // set the focus to the description edit if we wanted to
    if (_setFocusToDescriptionEdit) {
        ui->descriptionEdit->setFocus();
        _setFocusToDescriptionEdit = false;
    }
}

/**
 * Jumps to the correct todo list item
 */
void TodoDialog::jumpToTodoListItem() {
    // set the current row of the todo list to the first row
    if (ui->todoList->count() > 0) {
        int row = -1;

        // let us jump to a specific calendar item if it was set in the
        // constructor
        if (!_jumpToCalendarItemUid.isEmpty()) {
            row = findTodoItemRowByUID(_jumpToCalendarItemUid);

            if (row != 1) {
                _jumpToCalendarItemUid = "";
            }
        }

        // try to find a possible last created calendar item
        if ((row == -1) && lastCreatedCalendarItem.isFetched()) {
            row = findTodoItemRowByUID(lastCreatedCalendarItem.getUid());

            // clear the last created calendar item if we found it in the list
            if (row > -1) {
                lastCreatedCalendarItem = CalendarItem();
            }
        }

        if (row == -1) {
            // try to find the currently selected calendar item
            row = findTodoItemRowByUID(currentCalendarItem.getUid());
        }

        ui->todoList->setCurrentRow(row >= 0 ? row : 0);
    } else {
        resetEditFrameControls();
    }
}

void TodoDialog::clearTodoList() {
    const QSignalBlocker blocker(ui->todoList);
    Q_UNUSED(blocker);
    ui->todoList->clear();
    resetEditFrameControls();
}

void TodoDialog::resetEditFrameControls() {
    ui->summaryEdit->setText("");
    ui->descriptionEdit->setText("");
    ui->prioritySlider->setValue(0);
    ui->reminderCheckBox->setChecked(false);
    ui->reminderDateTimeEdit->hide();
    ui->saveButton->setEnabled(false);
    ui->saveAndInsertButton->setEnabled(false);
    ui->removeButton->setEnabled(false);
    currentCalendarItem = CalendarItem();
}

/**
 * @brief Searches a todo item by uid in the todo list
 * @param uid
 * @return Returns the row of the todo item in the todo list, returns -1 if not found
 */
int TodoDialog::findTodoItemRowByUID(QString uid) {
    int count = ui->todoList->count();
    if (count == 0) {
        return -1;
    }

    for (int i = 0; i < count; i++) {
        QListWidgetItem *item = ui->todoList->item(i);
        if (item->data(Qt::UserRole).toString() == uid) {
            return i;
        }
    }

    return -1;
}

void TodoDialog::storeSettings() {
    QSettings settings;
    settings.setValue("TodoDialog/geometry", saveGeometry());
    settings.setValue("TodoDialog/mainSplitterState",
                      this->mainSplitter->saveState());
    settings.setValue("TodoDialog/showCompletedItems",
                      ui->showCompletedItemsCheckBox->checkState());
    settings.setValue("TodoDialog/todoListSelectorSelectedItem",
                      ui->todoListSelector->currentText());
}

/**
 * @brief updates the current calendar item with the data from the edit form
 */
void TodoDialog::updateCurrentCalendarItemWithFormData() {
    int priority = ui->prioritySlider->value();

    if (priority > 0) {
        // 1 is the highest priority and 9 is the lowest priority
        priority = 10 - priority;
    }

    currentCalendarItem.setPriority(priority);
    currentCalendarItem.setSummary(ui->summaryEdit->text());
    currentCalendarItem.setDescription(ui->descriptionEdit->toPlainText());
    currentCalendarItem.setModified(QDateTime::currentDateTime());
    currentCalendarItem.setAlarmDate(
            ui->reminderCheckBox->isChecked() ?
            ui->reminderDateTimeEdit->dateTime() : QDateTime());
    currentCalendarItem.store();
}

void TodoDialog::on_TodoDialog_finished(int result) {
    Q_UNUSED(result);

    storeSettings();
}

void TodoDialog::on_todoListSelector_currentIndexChanged(const QString &arg1) {
    Q_UNUSED(arg1);

    // store the todoListSelectorSelectedItem
    storeSettings();

    // reload the todo list items
    reloadTodoList();
}

void TodoDialog::on_todoList_currentItemChanged(
        QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous);

    // in case all items were removed
    if (current == NULL) {
        resetEditFrameControls();
        return;
    }

    MetricsService::instance()->sendVisitIfEnabled("todo/item/changed");

    QString uid = current->data(Qt::UserRole).toString();

    currentCalendarItem = CalendarItem::fetchByUid(uid);
    if (currentCalendarItem.isFetched()) {
        ui->summaryEdit->setText(currentCalendarItem.getSummary());
        ui->summaryEdit->setCursorPosition(0);
        ui->descriptionEdit->setText(currentCalendarItem.getDescription());

        QDateTime alarmDate = currentCalendarItem.getAlarmDate();
        ui->reminderCheckBox->setChecked(alarmDate.isValid());
        ui->reminderDateTimeEdit->setDateTime(alarmDate);
        on_reminderCheckBox_clicked();

        int priority = currentCalendarItem.getPriority();

        if (priority > 0) {
            // 1 is the highest priority and 9 is the lowest priority
            priority = 10 - priority;
        }

        ui->prioritySlider->setValue(priority);
        on_prioritySlider_valueChanged(priority);

        ui->saveButton->setEnabled(true);
        ui->saveAndInsertButton->setEnabled(true);
        ui->removeButton->setEnabled(true);
    }
}

void TodoDialog::on_prioritySlider_valueChanged(int value) {
    QString priorityText;
    switch (value) {
        default:
        case 0:
            priorityText = "not set";
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            priorityText = "low";
            break;
        case 5:
            priorityText = "medium";
            break;
        case 6:
        case 7:
        case 8:
        case 9:
            priorityText = "high";
            break;
    }

    ui->prioritySlider->setToolTip("priority: " + priorityText);
}

void TodoDialog::on_showCompletedItemsCheckBox_clicked() {
    storeSettings();
    reloadTodoList();
}

void TodoDialog::on_saveButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled("todo/item/stored");

    updateCurrentCalendarItemWithFormData();

    OwnCloudService *ownCloud = new OwnCloudService(this);

    // update the local icsData from server
    ownCloud->updateICSDataOfCalendarItem(&currentCalendarItem);

    // post the calendar item to the server
    ownCloud->postCalendarItemToServer(currentCalendarItem, this);

    qDebug() << currentCalendarItem;
}

void TodoDialog::todoItemLoadingProgressBarIncrement() {
    ui->todoItemLoadingProgressBar->show();
    int value = ui->todoItemLoadingProgressBar->value() + 1;

    if (value <= 0) {
        value = 1;
    }

    ui->todoItemLoadingProgressBar->setValue(value);
}

void TodoDialog::todoItemLoadingProgressBarHide() {
    ui->todoItemLoadingProgressBar->hide();
}

void TodoDialog::todoItemLoadingProgressBarSetMaximum(int value) {
    ui->todoItemLoadingProgressBar->setMaximum(value);
    todoItemLoadingProgressBarHideIfOnMaximum();
}

void TodoDialog::todoItemLoadingProgressBarHideIfOnMaximum() {
    if (ui->todoItemLoadingProgressBar->value() >=
            ui->todoItemLoadingProgressBar->maximum()) {
        ui->todoItemLoadingProgressBar->hide();
    }
}

void TodoDialog::on_todoItemLoadingProgressBar_valueChanged(int value) {
    Q_UNUSED(value);

    todoItemLoadingProgressBarHideIfOnMaximum();
}

void TodoDialog::on_newItemEdit_returnPressed() {
    CalendarItem calItem = CalendarItem::createNewTodoItem(
            ui->newItemEdit->text(),
            ui->todoListSelector->currentText());
    lastCreatedCalendarItem = calItem;

    // set the focus to the description edit after we loaded the tasks
    _setFocusToDescriptionEdit = true;

    OwnCloudService *ownCloud = new OwnCloudService(this);

    // post the calendar item to the server
    ownCloud->postCalendarItemToServer(calItem, this);

//    if ( calItem.isFetched() )
//    {
//        qDebug() << __func__ << " - 'calItem': " << calItem;
//        reloadTodoListItems();
//    }

    ui->newItemEdit->clear();
}

/**
 * @brief Removes the currently selected task from the ownCloud server
 */
void TodoDialog::on_removeButton_clicked() {
    if (QMessageBox::information(
                this, tr("Remove todo item"),
                tr("Remove the selected todo item?\nThis cannot be undone!"),
                tr("&Remove"), tr("&Cancel"), QString::null,
                0, 1) == 0) {
        CalendarItem calItem = currentCalendarItem;

        // remove the calendar item from the list widget
        // (this will update the currentCalendarItem)
        ui->todoList->takeItem(ui->todoList->currentRow());

        // remove the calendar item from the database
        calItem.remove();

        // remove the calendar item from the ownCloud server
        // (this will reload the todo list as well)
        OwnCloudService *ownCloud = new OwnCloudService(this);
        ownCloud->removeCalendarItem(calItem, this);
    }
}

/**
 * @brief Updates the completed state of a calendar item on the ownCloud server
 * @param item
 */
void TodoDialog::on_todoList_itemChanged(QListWidgetItem *item) {
    qDebug() << __func__ << " - 'item': " << item;
    QString uid = item->data(Qt::UserRole).toString();

    CalendarItem calItem = CalendarItem::fetchByUid(uid);
    if (calItem.isFetched()) {
        calItem.updateCompleted(item->checkState() == Qt::Checked);
        calItem.store();

        OwnCloudService *ownCloud = new OwnCloudService(this);

        // post the calendar item to the server
        ownCloud->postCalendarItemToServer(calItem, this);
    }
}

/**
* @brief Determines whether to show or hide the reminder date time edit
*/
void TodoDialog::on_reminderCheckBox_clicked() {
    if (ui->reminderCheckBox->isChecked()) {
        QDateTime alarmDate = currentCalendarItem.getAlarmDate();

        // if no alarm date was set use the current date plus 1h
        if (!alarmDate.isValid()) {
            alarmDate = QDateTime::currentDateTime().addSecs(3600);
        }

        ui->reminderDateTimeEdit->setDateTime(alarmDate);
        ui->reminderDateTimeEdit->show();
    } else {
        ui->reminderDateTimeEdit->hide();
    }
}

void TodoDialog::on_reloadTodoListButton_clicked() {
    reloadTodoList();
}

/**
 * Pressing return in the summary text line of a task saves it
 */
void TodoDialog::on_summaryEdit_returnPressed() {
    // save the task if the save button is enabled
    if (ui->saveButton->isEnabled()) {
        on_saveButton_clicked();
    }
}

void TodoDialog::on_newItemEdit_textChanged(const QString &arg1) {
    // search notes when at least 2 characters were entered
    if (arg1.count() >= 2) {
        QList<QString> noteNameList = CalendarItem::searchAsUidList(
                arg1, ui->todoListSelector->currentText());
        firstVisibleTodoListRow = -1;

        for (int i = 0; i < ui->todoList->count(); ++i) {
            QListWidgetItem *item = ui->todoList->item(i);
            if (noteNameList.indexOf(item->data(Qt::UserRole).toString()) < 0) {
                item->setHidden(true);
            } else {
                if (firstVisibleTodoListRow < 0) {
                    firstVisibleTodoListRow = i;
                }
                item->setHidden(false);
            }
        }
    } else {  // show all items otherwise
        firstVisibleTodoListRow = 0;

        for (int i = 0; i < ui->todoList->count(); ++i) {
            QListWidgetItem *item = ui->todoList->item(i);
            item->setHidden(false);
        }
    }

    // let's highlight the text from the search line edit
    searchForSearchLineTextInNoteTextEdit();
}

/**
 * highlights all occurrences of tje search line text in the note text edit
 */
void TodoDialog::searchForSearchLineTextInNoteTextEdit() {
    QString searchString = ui->descriptionEdit->toPlainText();
    searchInDescriptionTextEdit(searchString);
}

/**
 * highlights all occurrences of str in the note text edit
 */
void TodoDialog::searchInDescriptionTextEdit(QString &str) {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (str.count() >= 2) {
        ui->descriptionEdit->moveCursor(QTextCursor::Start);
        QColor color = QColor(0, 180, 0, 100);

        while (ui->descriptionEdit->find(str)) {
            QTextEdit::ExtraSelection extra;
            extra.format.setBackground(color);
            extra.cursor = ui->descriptionEdit->textCursor();
            extraSelections.append(extra);
        }
    }

    ui->descriptionEdit->setExtraSelections(extraSelections);
}

/**
 * Event filters on the Todo Dialog
 */
bool TodoDialog::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (obj == ui->newItemEdit) {
            // set focus to the todo list if Key_Down or Key_Tab
            // were pressed in the new item edit
            if ((keyEvent->key() == Qt::Key_Down) ||
                    (keyEvent->key() == Qt::Key_Tab)) {
                // choose an other selected item if current item is invisible
                QListWidgetItem *item = ui->todoList->currentItem();
                if ((item != NULL) && ui->todoList->currentItem()->isHidden() &&
                    (firstVisibleTodoListRow >= 0)) {
                    ui->todoList->setCurrentRow(firstVisibleTodoListRow);
                }

                // give the keyboard focus to the todo list widget
                ui->todoList->setFocus();
                return true;
            }

            return false;
        } else if (obj == ui->todoList) {
            // set focus to the description edit if the tab key is pressed
            if (keyEvent->key() == Qt::Key_Tab) {
                ui->descriptionEdit->setFocus();
                return true;
            } else if ((keyEvent->key() == Qt::Key_Delete) ||
                       (keyEvent->key() == Qt::Key_Backspace)) {
                on_removeButton_clicked();
                return true;
            }

            return false;
        } else if (obj == ui->reminderDateTimeEdit) {
            // store the task and set focus to the description edit if the
            // return key is pressed
            if (keyEvent->key() == Qt::Key_Return) {
                on_saveButton_clicked();
                ui->descriptionEdit->setFocus();
                return true;
            }

            return false;
        }
    }

    return QDialog::eventFilter(obj, event);
}

/**
 * Saves the current note and inserts a link to it in the current note of the
 * main window
 */
void TodoDialog::on_saveAndInsertButton_clicked()
{
    on_saveButton_clicked();

    QString selectedText =
            _mainWindow->activeNoteTextEdit()->textCursor().selectedText();

    QString taskUrl = "task://" + currentCalendarItem.getUid();

    // insert a link to the task in the current note
    QString summaryText = selectedText.isEmpty() ?
                          currentCalendarItem.getSummary() : selectedText;
    QString insertText = "[" + summaryText + "](" + taskUrl + ")";

    _mainWindow->activeNoteTextEdit()->textCursor().insertText(insertText);
    close();
}
