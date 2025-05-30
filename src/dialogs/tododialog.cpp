#include "dialogs/tododialog.h"

#include <mainwindow.h>
#include <qgridlayout.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <services/metricsservice.h>
#include <utils/gui.h>

#include <QDebug>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QShortcut>
#include <QSplitter>

#include "entities/calendaritem.h"
#include "services/owncloudservice.h"
#include "services/settingsservice.h"
#include "ui_tododialog.h"

TodoDialog::TodoDialog(const QString &taskUid, QWidget *parent)
    : MasterDialog(parent), ui(new Ui::TodoDialog) {
    ui->setupUi(this);
    afterSetupUI();
    setupUi();

    connect(ui->todoItemTreeWidget, &TodoItemTreeWidget::calendarItemUpdated, this,
            &TodoDialog::updateCalendarItem);

    // init the description edit search frame
    ui->descriptionEdit->initSearchFrame(ui->descriptionEditSearchFrame);

    QString selectedText =
        MainWindow::instance()->activeNoteTextEdit()->textCursor().selectedText();

    // insert the selected note text in the new item edit
    if (!selectedText.isEmpty()) {
        ui->newItemEdit->setText(selectedText);
    }

    // jump to a task
    if (!taskUid.isEmpty()) {
        jumpToTask(taskUid);
    }

    _todoTagsScrollArea = new QScrollArea(this);
    _todoTagsScrollArea->setWidget(ui->tagsFrame);
    _todoTagsScrollArea->setMaximumHeight(80);
    ui->tagCloudLayout->setSizeConstraint(QLayout::SetFixedSize);
    ui->tagsLayout->addWidget(_todoTagsScrollArea);
    _todoTagsScrollArea->setVisible(false);
    ui->editFrame->setEnabled(false);
}

void TodoDialog::updateCalendarItem(CalendarItem item) {
    OwnCloudService *ownCloud = OwnCloudService::instance();
    bool result = ownCloud->updateICSDataOfCalendarItem(&item);

    qDebug() << __func__ << " - 'result': " << result;

    if (result) {
        // post the calendar item to the server (and reload note tree)
        ownCloud->postCalendarItemToServer(item, this);
    }
}

/**
 * Jumps to a task
 *
 * @param taskUid
 */
void TodoDialog::jumpToTask(const QString &taskUid) {
    if (taskUid.isEmpty()) {
        return;
    }

    CalendarItem calendarItem = CalendarItem::fetchByUid(taskUid);
    qDebug() << __func__ << " - 'calendarItem': " << calendarItem;

    if (calendarItem.exists()) {
        // set a calendar item uid to jump to later on
        _jumpToCalendarItemUid = taskUid;

        QString calendar = calendarItem.getCalendar();

        // if the calendar of the calendar item isn't the current one we
        // have to switch to it
        if (ui->todoListSelector->currentText() != calendar) {
            // select the correct calendar and then jump to the task item
            ui->todoListSelector->setCurrentText(calendar);
        } else {
            // jump directly to the correct task item
            jumpToTodoListItem();
        }
    }
}

TodoDialog::~TodoDialog() { delete ui; }

void TodoDialog::setupUi() {
    setupMainSplitter();
    refreshUi();

    ui->newItemEdit->installEventFilter(this);
    ui->todoItemTreeWidget->installEventFilter(this);
    ui->reminderDateTimeEdit->installEventFilter(this);

    ui->newItemEdit->setFocus();

    // adding shortcuts, that weren't working when defined in the ui file
    auto *shortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+S")), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(on_saveButton_clicked()));
    shortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+I")), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(onSaveAndInsertButtonClicked()));
    shortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+R")), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(on_removeButton_clicked()));

    /*
     * setup the note button menu
     */
    auto *noteMenu = new QMenu(this);

    QAction *insertAction = noteMenu->addAction(tr("Save and insert into note"));
    insertAction->setIcon(QIcon::fromTheme(
        QStringLiteral("document-save"), QIcon(":icons/breeze-qownnotes/16x16/document-save.svg")));
    insertAction->setToolTip(
        tr("Save the current todo item and insert a link"
           " to it into the current note"));
    connect(insertAction, SIGNAL(triggered()), this, SLOT(onSaveAndInsertButtonClicked()));

    QAction *importAction = noteMenu->addAction(tr("Import as note"));
    importAction->setIcon(
        QIcon::fromTheme(QStringLiteral("document-import"),
                         QIcon(":icons/breeze-qownnotes/16x16/document-import.svg")));
    importAction->setToolTip(tr("Import the current todo item as new note"));
    connect(importAction, SIGNAL(triggered()), this, SLOT(onImportAsNoteButtonClicked()));

    ui->noteButton->setMenu(noteMenu);

    /*
     * setup the reload button menu
     */
    auto *reloadMenu = new QMenu(this);

    QAction *reloadAction = reloadMenu->addAction(tr("Reload from server"));
    reloadAction->setIcon(QIcon::fromTheme(
        QStringLiteral("view-refresh"), QIcon(":icons/breeze-qownnotes/16x16/view-refresh.svg")));
    reloadAction->setToolTip(tr("Reload tasks from server"));
    connect(reloadAction, SIGNAL(triggered()), this, SLOT(reloadTodoList()));

    QAction *clearCacheAction = reloadMenu->addAction(tr("Clear cache and reload"));
    clearCacheAction->setIcon(QIcon::fromTheme(
        QStringLiteral("trash-empty"), QIcon(":icons/breeze-qownnotes/16x16/trash-empty.svg")));
    clearCacheAction->setToolTip(
        tr("Clear calendar cache and reload tasks "
           "from server"));
    connect(clearCacheAction, SIGNAL(triggered()), this, SLOT(clearCacheAndReloadTodoList()));

    connect(ui->tagsLineEdit, &QLineEdit::returnPressed, this,
            &TodoDialog::on_tagsLineEdit_returnPressed);

    ui->reloadTodoListButton->setMenu(reloadMenu);
}

/**
 * Refreshes the UI
 */
void TodoDialog::refreshUi() {
    loadTodoListData();

    ui->todoItemLoadingProgressBar->hide();

    SettingsService settings;

    {
        const QSignalBlocker blocker(ui->showCompletedItemsCheckBox);
        Q_UNUSED(blocker)

        bool showCompletedItems =
            settings.value(QStringLiteral("TodoDialog/showCompletedItems")).toBool();
        ui->showCompletedItemsCheckBox->setChecked(showCompletedItems);

        bool showDueTodayItemsOnly =
            settings.value(QStringLiteral("TodoDialog/showDueTodayItemsOnly")).toBool();
        ui->showDueTodayItemsOnlyCheckBox->setChecked(showDueTodayItemsOnly);
    }

    int index = CalendarItem::getCurrentCalendarIndex();

    if (index >= 0) {
        const QSignalBlocker blocker(ui->todoListSelector);
        Q_UNUSED(blocker)

        // set the index of the task list selector if we found it
        ui->todoListSelector->setCurrentIndex(index);
    } else {
        // if we didn't find the index store the new current item
        settings.setValue(QStringLiteral("TodoDialog/todoListSelectorSelectedItem"),
                          ui->todoListSelector->currentText());
    }

    // hide the reminder date time select
    ui->reminderDateTimeEdit->hide();

    // now load the task list items
    reloadTodoList();
}

void TodoDialog::setupMainSplitter() {
    this->mainSplitter = new QSplitter(this);

    this->mainSplitter->addWidget(ui->selectFrame);
    this->mainSplitter->addWidget(ui->editFrame);

    // restore splitter sizes
    SettingsService settings;
    QByteArray state = settings.value(QStringLiteral("TodoDialog/mainSplitterState")).toByteArray();
    this->mainSplitter->restoreState(state);

    ui->gridLayout->layout()->addWidget(this->mainSplitter);
}

/**
 * @brief Loads the calendar items from the settings to the task list selector
 */
void TodoDialog::loadTodoListData() {
    const QSignalBlocker blocker(ui->todoListSelector);
    Q_UNUSED(blocker)

    SettingsService settings;
    ui->todoListSelector->clear();
    ui->todoListSelector->addItems(
        settings.value(QStringLiteral("ownCloud/todoCalendarEnabledList")).toStringList());
}

/**
 * @brief Fetches the items of the current task list from ownCloud
 */
void TodoDialog::reloadTodoList() {
    ui->todoItemLoadingProgressBar->setValue(0);
    ui->todoItemLoadingProgressBar->show();
    OwnCloudService *ownCloud = OwnCloudService::instance();
    ownCloud->todoGetTodoList(ui->todoListSelector->currentText(), this);
}

/**
 * @brief Clears the calendar table and reloads the tasks from the server
 */
void TodoDialog::clearCacheAndReloadTodoList() {
    CalendarItem::removeAll();
    reloadTodoList();
}

/**
 * Reloads the task list from the SQLite database
 */
void TodoDialog::reloadTodoListItems() {
    QList<CalendarItem> calendarItemList =
        CalendarItem::fetchAllByCalendar(ui->todoListSelector->currentText());

    int itemCount = calendarItemList.count();
    MetricsService::instance()->sendEventIfEnabled(
        QStringLiteral("todo/list/loaded"), QStringLiteral("todo"),
        QStringLiteral("todo list loaded"), QString::number(itemCount) + " todo items", itemCount);

    {
        const QSignalBlocker blocker(ui->todoItemTreeWidget);
        Q_UNUSED(blocker)

        ui->todoItemTreeWidget->clear();
        QList<QTreeWidgetItem *> subItems;

        // add all top level calendar items to the todoItemTreeWidget
        QListIterator<CalendarItem> itr(calendarItemList);
        while (itr.hasNext()) {
            CalendarItem calItem = itr.next();

            // skip completed items if the "Show completed items" checkbox
            // is not checked
            if (!ui->showCompletedItemsCheckBox->isChecked()) {
                if (calItem.isCompleted()) {
                    continue;
                }
            }

            // skip items that are not due today if the "Show only items due today"
            // checkbox is checked
            if (ui->showDueTodayItemsOnlyCheckBox->isChecked()) {
                const auto alarmDate = calItem.getAlarmDate().date();
                const auto todayDate = QDate::currentDate();

                if (todayDate != alarmDate) {
                    continue;
                }
            }

            const QString uid = calItem.getUid();

            // skip items that were not fully loaded yet
            if (uid.isEmpty()) {
                continue;
            }

            const QString relatedUid = calItem.getRelatedUid();

            auto *item = new QTreeWidgetItem();
            item->setText(0, calItem.getSummary());
            item->setData(0, Qt::UserRole, uid);
            item->setData(0, Qt::UserRole + 1, relatedUid);
            item->setCheckState(0, calItem.isCompleted() ? Qt::Checked : Qt::Unchecked);
            item->setText(1, calItem.getAlarmDate().toString());
            item->setData(1, Qt::DisplayRole, calItem.getAlarmDate());
            item->setFlags(Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled |
                           Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);

            if (relatedUid.isEmpty()) {
                ui->todoItemTreeWidget->addTopLevelItem(item);
            } else {
                subItems.append(item);
            }
        }

        // decorate the root only if there are sub-items
        ui->todoItemTreeWidget->setRootIsDecorated(subItems.count() > 0);

        // add calendar sub-items to the todoItemTreeWidget as long as all
        // levels are filled
        int depth = 0;
        while (subItems.count() > 0 && depth++ < 20) {
            QListIterator<QTreeWidgetItem *> itemItr(subItems);

            while (itemItr.hasNext()) {
                QTreeWidgetItem *item = itemItr.next();
                QString relatedUid = item->data(0, Qt::UserRole + 1).toString();

                if (relatedUid.isEmpty()) {
                    continue;
                }

                auto parentItem =
                    Utils::Gui::getTreeWidgetItemWithUserData(ui->todoItemTreeWidget, relatedUid);

                if (parentItem != nullptr) {
                    parentItem->addChild(item);
                    parentItem->setExpanded(true);
                    subItems.removeOne(item);
                }
            }
        }
    }

    ui->todoItemTreeWidget->resizeColumnToContents(0);
    ui->todoItemTreeWidget->resizeColumnToContents(1);

    // set the current row of the task list to the first row
    jumpToTodoListItem();

    // set the focus to the description edit if we wanted to
    if (_setFocusToDescriptionEdit) {
        ui->descriptionEdit->setFocus();
        _setFocusToDescriptionEdit = false;
    }

    on_newItemEdit_textChanged();
}

/**
 * Jumps to the correct task list item
 */
void TodoDialog::jumpToTodoListItem() {
    if (ui->todoItemTreeWidget->topLevelItemCount() > 0) {
        QTreeWidgetItem *item = nullptr;

        // let us jump to a specific calendar item if it was set in the
        // constructor
        if (!_jumpToCalendarItemUid.isEmpty()) {
            item = findTodoItemTreeWidgetItemByUID(_jumpToCalendarItemUid);

            if (item != nullptr) {
                _jumpToCalendarItemUid = QLatin1String("");
            }
        }

        // try to find a possible last created calendar item
        if ((item == nullptr) && lastCreatedCalendarItem.isFetched()) {
            item = findTodoItemTreeWidgetItemByUID(lastCreatedCalendarItem.getUid());

            // clear the last created calendar item if we found it in the list
            if (item != nullptr) {
                lastCreatedCalendarItem = CalendarItem();
            }
        }

        if (item == nullptr) {
            // try to find the currently selected calendar item
            item = findTodoItemTreeWidgetItemByUID(currentCalendarItem.getUid());
        }

        if (item != nullptr) {
            ui->todoItemTreeWidget->setCurrentItem(item);
        }
    } else {
        resetEditFrameControls();
    }
}

void TodoDialog::clearTodoList() {
    const QSignalBlocker blocker(ui->todoItemTreeWidget);
    Q_UNUSED(blocker)
    ui->todoItemTreeWidget->clear();
    resetEditFrameControls();
}

void TodoDialog::resetEditFrameControls() {
    ui->editFrame->setEnabled(false);
    ui->summaryEdit->setText(QString());
    ui->descriptionEdit->setPlainText(QString());
    ui->tagsLineEdit->setText(QString());
    cleanTagButtons();
    ui->prioritySlider->setValue(0);
    ui->reminderCheckBox->setChecked(false);
    ui->reminderDateTimeEdit->hide();
    ui->saveButton->setEnabled(false);
    ui->noteButton->setEnabled(false);
    ui->removeButton->setEnabled(false);
    currentCalendarItem = CalendarItem();
}

/**
 * @brief Searches a task item by uid in the task list
 * @param uid
 * @return Returns the tree widget item of the task item in the task tree,
 * returns null if not found
 */
QTreeWidgetItem *TodoDialog::findTodoItemTreeWidgetItemByUID(const QString &uid) {
    return Utils::Gui::getTreeWidgetItemWithUserData(ui->todoItemTreeWidget, uid);
}

void TodoDialog::storeSettings() {
    SettingsService settings;
    settings.setValue(QStringLiteral("TodoDialog/geometry"), saveGeometry());
    settings.setValue(QStringLiteral("TodoDialog/mainSplitterState"),
                      this->mainSplitter->saveState());
    settings.setValue(QStringLiteral("TodoDialog/showCompletedItems"),
                      ui->showCompletedItemsCheckBox->checkState());
    settings.setValue(QStringLiteral("TodoDialog/showDueTodayItemsOnly"),
                      ui->showDueTodayItemsOnlyCheckBox->checkState());
    settings.setValue(QStringLiteral("TodoDialog/todoListSelectorSelectedItem"),
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
    currentCalendarItem.setTags(getTagString());
    currentCalendarItem.setModified(QDateTime::currentDateTime());
    currentCalendarItem.setAlarmDate(
        ui->reminderCheckBox->isChecked() ? ui->reminderDateTimeEdit->dateTime() : QDateTime());
    currentCalendarItem.store();
}

void TodoDialog::on_TodoDialog_finished(int result) {
    Q_UNUSED(result)

    storeSettings();
}

void TodoDialog::on_todoListSelector_currentIndexChanged(int index) {
    Q_UNUSED(index)

    ui->newItemEdit->clear();

    resetEditFrameControls();

    // store the todoListSelectorSelectedItem
    storeSettings();

    // reload the task list items
    reloadTodoList();
}

void TodoDialog::on_prioritySlider_valueChanged(int value) {
    QString priorityText;
    switch (value) {
        default:
        case 0:
            priorityText = QLatin1String("not set");
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            priorityText = QLatin1String("low");
            break;
        case 5:
            priorityText = QLatin1String("medium");
            break;
        case 6:
        case 7:
        case 8:
        case 9:
            priorityText = QLatin1String("high");
            break;
    }

    ui->prioritySlider->setToolTip("priority: " + priorityText);
}

void TodoDialog::on_showCompletedItemsCheckBox_clicked() {
    storeSettings();
    reloadTodoList();
}

void TodoDialog::on_saveButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("todo/item/stored"));

    updateCurrentCalendarItemWithFormData();

    OwnCloudService *ownCloud = OwnCloudService::instance();

    // update the local icsData from server
    ownCloud->updateICSDataOfCalendarItem(&currentCalendarItem);

    // post the calendar item to the server
    ownCloud->postCalendarItemToServer(currentCalendarItem, this);

    qDebug() << currentCalendarItem;

    SettingsService settings;
    if (settings.value(QStringLiteral("closeTodoListAfterSave")).toBool()) {
        close();
    }
}

void TodoDialog::todoItemLoadingProgressBarIncrement() {
    ui->todoItemLoadingProgressBar->show();
    int value = ui->todoItemLoadingProgressBar->value() + 1;

    if (value <= 0) {
        value = 1;
    }

    ui->todoItemLoadingProgressBar->setValue(value);
}

void TodoDialog::todoItemLoadingProgressBarHide() { ui->todoItemLoadingProgressBar->hide(); }

void TodoDialog::todoItemLoadingProgressBarSetMaximum(int value) {
    ui->todoItemLoadingProgressBar->setMaximum(value);
    todoItemLoadingProgressBarHideIfOnMaximum();
}

void TodoDialog::todoItemLoadingProgressBarHideIfOnMaximum() {
    if (ui->todoItemLoadingProgressBar->value() >= ui->todoItemLoadingProgressBar->maximum()) {
        ui->todoItemLoadingProgressBar->hide();
    }
}

void TodoDialog::on_todoItemLoadingProgressBar_valueChanged(int value) {
    Q_UNUSED(value)

    todoItemLoadingProgressBarHideIfOnMaximum();
}

void TodoDialog::createNewTodoItem(const QString &name, const QString &relatedUid) {
    CalendarItem calItem =
        CalendarItem::createNewTodoItem(name, ui->todoListSelector->currentText(), relatedUid);
    lastCreatedCalendarItem = calItem;

    // set the focus to the description edit after we loaded the tasks
    _setFocusToDescriptionEdit = true;

    OwnCloudService *ownCloud = OwnCloudService::instance();

    // post the calendar item to the server
    ownCloud->postCalendarItemToServer(calItem, this);

    //    if ( calItem.isFetched() )
    //    {
    //        qDebug() << __func__ << " - 'calItem': " << calItem;
    //        reloadTodoListItems();
    //    }
}

void TodoDialog::on_newItemEdit_returnPressed() {
    createNewTodoItem(ui->newItemEdit->text());
    ui->newItemEdit->clear();
}

/**
 * @brief Removes the currently selected task from the ownCloud server
 */
void TodoDialog::on_removeButton_clicked() {
    if (Utils::Gui::question(this, tr("Remove todo item"),
                             tr("Remove the selected todo item?\nThis cannot be undone!"),
                             QStringLiteral("remove-todo-items")) == QMessageBox::Yes) {
        CalendarItem calItem = currentCalendarItem;

        // remove the calendar item from the list widget
        // (this will update the currentCalendarItem)
        ui->todoItemTreeWidget->removeItemWidget(ui->todoItemTreeWidget->currentItem(), 0);

        // remove the calendar item from the database
        calItem.remove();

        // remove the calendar item from the ownCloud server
        // (this will reload the task list as well)
        OwnCloudService *ownCloud = OwnCloudService::instance();
        ownCloud->removeCalendarItem(calItem, this);

        resetEditFrameControls();
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
        currentCalendarItem.setAlarmDate(QDateTime{});
        ui->reminderDateTimeEdit->clear();
        ui->reminderDateTimeEdit->hide();
    }
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

void TodoDialog::on_newItemEdit_textChanged() {
    const QString &arg1 = ui->newItemEdit->text();

    // get all items
    QList<QTreeWidgetItem *> allItems = ui->todoItemTreeWidget->findItems(
        QLatin1String(""), Qt::MatchContains | Qt::MatchRecursive);

    // search todo item if at least 2 characters were entered
    if (arg1.count() >= 2) {
        QList<QString> uidList =
            CalendarItem::searchAsUidList(arg1, ui->todoListSelector->currentText());

        // hide all not found items
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            bool show = uidList.indexOf(item->data(0, Qt::UserRole).toString()) > -1;
            item->setHidden(!show);
        }

        // show items again that have visible children so that they are
        // really shown
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            if (Utils::Gui::isOneTreeWidgetItemChildVisible(item)) {
                item->setHidden(false);
                item->setExpanded(true);
            }
        }
    } else {
        // show all items otherwise
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            item->setHidden(false);
        }
    }

    // let's highlight the text from the search line edit
    searchForSearchLineTextInNoteTextEdit();
}

/**
 * highlights all occurrences of the search line text in the note text edit
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
            QTextEdit::ExtraSelection extra = QTextEdit::ExtraSelection();
            extra.format.setBackground(color);
            extra.cursor = ui->descriptionEdit->textCursor();
            extraSelections.append(extra);
        }
    }

    ui->descriptionEdit->setExtraSelections(extraSelections);
}

/**
 * Event filters on the task dialog
 */
bool TodoDialog::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (obj == ui->newItemEdit) {
            // set focus to the task list if Key_Down or Key_Tab
            // were pressed in the new item edit
            if ((keyEvent->key() == Qt::Key_Down) || (keyEvent->key() == Qt::Key_Tab)) {
                // choose another selected item if current item is invisible
                auto *item = ui->todoItemTreeWidget->currentItem();
                if ((item != nullptr) && item->isHidden() &&
                    (firstVisibleTodoItemTreeItem != nullptr)) {
                    ui->todoItemTreeWidget->setCurrentItem(firstVisibleTodoItemTreeItem);
                }

                // give the keyboard focus to the task list widget
                ui->todoItemTreeWidget->setFocus();
                return true;
            }

            return false;
        } else if (obj == ui->todoItemTreeWidget) {
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

    return MasterDialog::eventFilter(obj, event);
}

/**
 * Saves the current note and inserts a link to it in the current note of the
 * main window
 */
void TodoDialog::onSaveAndInsertButtonClicked() {
    on_saveButton_clicked();

    QString selectedText =
        MainWindow::instance()->activeNoteTextEdit()->textCursor().selectedText();

    QString taskUrl = "task://" + currentCalendarItem.getUid();

    // insert a link to the task in the current note
    QString summaryText = selectedText.isEmpty() ? currentCalendarItem.getSummary() : selectedText;
    QString insertText = "[" + summaryText + "](" + taskUrl + ")";

    MainWindow::instance()->activeNoteTextEdit()->textCursor().insertText(insertText);
    close();
}

/**
 * Imports the current task as new note
 */
void TodoDialog::onImportAsNoteButtonClicked() {
    QString name = ui->summaryEdit->text();
    QString text = ui->descriptionEdit->toPlainText();

    // create a new note with the task text
    MainWindow::instance()->createNewNote(
        name, text,
        MainWindow::CreateNewNoteOptions(MainWindow::CreateNewNoteOption::UseNameAsHeadline));
}

/**
 * @brief
 * @param current
 * @param previous
 */
void TodoDialog::on_todoItemTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                          QTreeWidgetItem *previous) {
    Q_UNUSED(previous)

    // in case all items were removed
    if (current == nullptr) {
        resetEditFrameControls();
        return;
    }
    cleanTagButtons();

    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("todo/item/changed"));

    QString uid = current->data(0, Qt::UserRole).toString();

    currentCalendarItem = CalendarItem::fetchByUid(uid);
    if (currentCalendarItem.isFetched()) {
        ui->summaryEdit->setText(currentCalendarItem.getSummary());
        ui->summaryEdit->setCursorPosition(0);
        ui->descriptionEdit->setPlainText(currentCalendarItem.getDescription());

        // Absolute monster of a regexp that checks for possible edge cases like \\,\\ and such
        QRegularExpression nonescapedCommas(
            R"(((?<!\\),(?!,))|((?<=\\\\),(?=\\\\))|((?<=\\\\),(?=\\)))");

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
        _todoTagsList =
            currentCalendarItem.getTags().split(QStringLiteral(","), QString::SkipEmptyParts);
#else
        _todoTagsList =
            currentCalendarItem.getTags().split(QStringLiteral(","), Qt::SkipEmptyParts);
#endif

        reloadCurrentTags();

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
        ui->noteButton->setEnabled(true);
        ui->removeButton->setEnabled(true);
        ui->editFrame->setEnabled(true);
    }
}

/**
 * @brief Updates the completed state of a calendar item on the ownCloud server
 * @param item
 * @param column
 */
void TodoDialog::on_todoItemTreeWidget_itemChanged(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column)

    QString uid = item->data(0, Qt::UserRole).toString();
    qDebug() << __func__ << " - 'item': " << item << " - 'uid': " << uid;

    CalendarItem calItem = CalendarItem::fetchByUid(uid);
    if (calItem.isFetched()) {
        calItem.updateCompleted(item->checkState(0) == Qt::Checked);
        calItem.store();

        OwnCloudService *ownCloud = OwnCloudService::instance();

        // post the calendar item to the server
        ownCloud->postCalendarItemToServer(calItem, this);
    }
}

void TodoDialog::on_todoItemTreeWidget_customContextMenuRequested(QPoint pos) {
    QTreeWidgetItem *item = ui->todoItemTreeWidget->currentItem();

    if (item == nullptr) {
        return;
    }

    const QPoint globalPos = ui->todoItemTreeWidget->mapToGlobal(pos);
    auto *menu = new QMenu();

    QAction *newTaskAction = menu->addAction(tr("Create sub-task"));

    QAction *selectedItem = menu->exec(globalPos);
    if (selectedItem) {
        if (selectedItem == newTaskAction) {
            bool ok;
            QString name = QInputDialog::getText(this, tr("Create new sub-task"), tr("Name:"),
                                                 QLineEdit::Normal, tr("New sub-task"), &ok);

            if (ok) {
                createNewTodoItem(name, item->data(0, Qt::UserRole).toString());
            }
        }
    }
}

void TodoDialog::on_showDueTodayItemsOnlyCheckBox_clicked() {
    on_showCompletedItemsCheckBox_clicked();
}

void TodoDialog::cleanTagButtons() {
    QLayoutItem *child;
    while ((child = ui->tagsFrame->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
}

void TodoDialog::reloadCurrentTags() {
    cleanTagButtons();
    _todoTagsScrollArea->setVisible(false);
    if (_todoTagsList.empty()) {
        return;
    }
    for (const auto &tag : _todoTagsList) {
        if (tag.isEmpty()) {
            continue;
        }
        // Don't show escaped commas or escaped backslashes
        const QString buttonText = QString(tag).replace("\\,", ",").replace("\\\\", "\\");
        QPushButton *tagButton = new QPushButton(buttonText, ui->tagsFrame);
        tagButton->setIcon(QIcon::fromTheme("tag-delete"));
        _todoTagsList.removeOne(tag);
        reloadCurrentTags();
        connect(tagButton, &QPushButton::released, this, [this]() {});
        ui->tagsFrame->layout()->addWidget(tagButton);
    }
    _todoTagsScrollArea->setVisible(true);
}

QString TodoDialog::getTagString() {
    // Remove any possible empty items
    _todoTagsList.removeAll(QString(""));
    _todoTagsList.removeAll(QStringLiteral(" "));
    QString fullTagString;
    // We can't use regular join since it also joins escaped commas
    for (const auto &str : _todoTagsList) {
        fullTagString.append(str + ",");
    }
    return fullTagString.remove(QRegularExpression(", *$"));
}

void TodoDialog::on_tagsLineEdit_returnPressed() {
    const auto newTag = ui->tagsLineEdit->text().simplified();
    if (_todoTagsList.contains(newTag) || newTag.isEmpty()) {
        return;
    }
    // Escape the backslashes and commas in tags
    _todoTagsList.append(QString(newTag).replace("\\", "\\\\").replace(",", "\\,"));
    ui->tagsLineEdit->clear();
    reloadCurrentTags();
}
