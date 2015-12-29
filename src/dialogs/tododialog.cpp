#include "entities/calendaritem.h"
#include "services/owncloudservice.h"
#include "dialogs/tododialog.h"
#include "ui_tododialog.h"

#include <QSettings>
#include <QMessageBox>

TodoDialog::TodoDialog(SimpleCrypt *crypto, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TodoDialog)
{
    this->crypto = crypto;

    ui->setupUi(this);
    setupUi();
    ui->reminderDateTimeEdit->hide();
}

TodoDialog::~TodoDialog()
{
    delete ui;
}

void TodoDialog::setupUi()
{
    setupMainSplitter();
    loadTodoListData();

    ui->todoItemLoadingProgressBar->hide();

    QSettings settings;

    {
        const QSignalBlocker blocker( this->ui->showCompletedItemsCheckBox );

        bool showCompletedItems = settings.value( "TodoDialog/showCompletedItems" ).toBool();
        ui->showCompletedItemsCheckBox->setChecked( showCompletedItems );
    }

    int index = CalendarItem::getCurrentCalendarIndex();

    if ( index >= 0 )
    {
        const QSignalBlocker blocker( this->ui->todoListSelector );

        // set the index of the todo list selector if we found it
        ui->todoListSelector->setCurrentIndex( index );
    }

    // now load the todo list items
    reloadTodoList();
}

void TodoDialog::setupMainSplitter()
{
    this->mainSplitter = new QSplitter;

    this->mainSplitter->addWidget( ui->selectFrame );
    this->mainSplitter->addWidget( ui->editFrame );

    // restore splitter sizes
    QSettings settings;
    QByteArray state = settings.value( "TodoDialog/mainSplitterState" ).toByteArray();
    this->mainSplitter->restoreState( state );

    this->ui->gridLayout->layout()->addWidget( this->mainSplitter );
}

/**
 * @brief Loads the calendar items from the settings to the todo list selector
 */
void TodoDialog::loadTodoListData()
{
    const QSignalBlocker blocker( this->ui->todoListSelector );

    QSettings settings;
    ui->todoListSelector->clear();
    ui->todoListSelector->addItems( settings.value( "ownCloud/todoCalendarEnabledList" ).toStringList() );
}

/**
 * @brief Fetches the items of the current todo list from ownCloud
 */
void TodoDialog::reloadTodoList()
{
    ui->todoItemLoadingProgressBar->setValue( 0 );
    ui->todoItemLoadingProgressBar->show();
    OwnCloudService *ownCloud = new OwnCloudService( crypto, this );
    ownCloud->todoGetTodoList( ui->todoListSelector->currentText(), this );
}

/**
 * @brief Reloads the todo list from the sqlite database
 */
void TodoDialog::reloadTodoListItems()
{
    QList<CalendarItem> calendarItemList = CalendarItem::fetchAllByCalendar( ui->todoListSelector->currentText() );

    {
        const QSignalBlocker blocker( this->ui->todoList );

        ui->todoList->clear();

        QListIterator<CalendarItem> itr ( calendarItemList );
        while ( itr.hasNext() )
        {
           CalendarItem calItem = itr.next();

           // skip completed items if the "show completed items" ckeckbox is not checked
           if ( !ui->showCompletedItemsCheckBox->checkState() )
           {
               if ( calItem.isCompleted() ) {
                   continue;
               }
           }

           QString uid = calItem.getUid();

           // skip items that were not fully loaded yet
           if ( uid == "" ) {
               continue;
           }

           QListWidgetItem *item = new QListWidgetItem( calItem.getSummary() );
           item->setWhatsThis( uid );
           item->setCheckState( calItem.isCompleted() ? Qt::Checked : Qt::Unchecked );
           item->setFlags( Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable );

           ui->todoList->addItem( item );
        }
    }

    // set the current row of the todo list to the first row
    if ( ui->todoList->count() > 0 )
    {
        int row = -1;

        // try to find a possible last created calendar item
        if ( lastCreatedCalendarItem.isFetched() )
        {
            row = findTodoItemRowByUID( lastCreatedCalendarItem.getUid() );

            // clear the last created calendar item if we found it in the list
            if ( row > -1 )
            {
                lastCreatedCalendarItem = CalendarItem();
            }
        }

        if ( row == -1 )
        {
            // try to find the currently selected calendar item
            row = findTodoItemRowByUID( currentCalendarItem.getUid() );
        }

//        qDebug() << __func__ << " - 'currentCalendarItem.getUid()': " << currentCalendarItem.getUid();
//        qDebug() << __func__ << " - 'currentCalendarItem.getSummary()': " << currentCalendarItem.getSummary();
//        qDebug() << __func__ << " - 'row': " << row;

        ui->todoList->setCurrentRow( row >= 0 ? row : 0 );
    }
    else
    {
        resetEditFrameControls();
    }
}

void TodoDialog::clearTodoList()
{
    const QSignalBlocker blocker( this->ui->todoList );
    ui->todoList->clear();
    resetEditFrameControls();
}

void TodoDialog::resetEditFrameControls()
{
    ui->summaryEdit->setText( "" );
    ui->descriptionEdit->setText( "" );
    ui->prioritySlider->setValue( 0 );
    ui->reminderCheckBox->setChecked( false );
    currentCalendarItem = CalendarItem();
}

/**
 * @brief Searches a todo item by uid in the todo list
 * @param uid
 * @return Returns the row of the todo item in the todo list, returns -1 if not found
 */
int TodoDialog::findTodoItemRowByUID( QString uid )
{
    int count = ui->todoList->count();
    if ( count == 0 )  {
        return -1;
    }

    for ( int i = 0; i < count; i++ )
    {
        QListWidgetItem *item = ui->todoList->item( i );
        if ( item->whatsThis() == uid )
        {
            return i;
        }
    }

    return -1;
}

void TodoDialog::storeSettings()
{
    QSettings settings;
    settings.setValue( "TodoDialog/geometry", saveGeometry() );
    settings.setValue( "TodoDialog/mainSplitterState", this->mainSplitter->saveState() );
    settings.setValue( "TodoDialog/showCompletedItems", ui->showCompletedItemsCheckBox->checkState() );
    settings.setValue( "TodoDialog/todoListSelectorSelectedItem", ui->todoListSelector->currentText() );
}

/**
 * @brief updates the current calendar item with the data from the edit form
 */
void TodoDialog::updateCurrentCalendarItemWithFormData()
{
    int priority = ui->prioritySlider->value();

    if ( priority > 0 )
    {
        // 1 is the highest priority and 9 is the lowest priority
        priority = 10 - priority;
    }

    currentCalendarItem.setPriority( priority );
    currentCalendarItem.setSummary( ui->summaryEdit->text() );
    currentCalendarItem.setDescription( ui->descriptionEdit->toPlainText() );
    currentCalendarItem.setModified( QDateTime::currentDateTime() );
    currentCalendarItem.store();
}

void TodoDialog::on_TodoDialog_finished(int result)
{
    storeSettings();
}

void TodoDialog::on_todoListSelector_currentIndexChanged(const QString &arg1)
{
    reloadTodoList();
}

void TodoDialog::on_todoList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    QString uid = current->whatsThis();

    currentCalendarItem = CalendarItem::fetchByUid( uid );
    if ( currentCalendarItem.isFetched() )
    {
        ui->summaryEdit->setText( currentCalendarItem.getSummary() );
        ui->summaryEdit->setCursorPosition( 0 );
        ui->descriptionEdit->setText( currentCalendarItem.getDescription() );

        QDateTime alarmDate = currentCalendarItem.getAlarmDate();
        ui->reminderCheckBox->setChecked( !alarmDate.isNull() );
        ui->reminderDateTimeEdit->setDateTime( alarmDate );

        int priority = currentCalendarItem.getPriority();

        if ( priority > 0 )
        {
            // 1 is the highest priority and 9 is the lowest priority
            priority = 10 - priority;
        }

        ui->prioritySlider->setValue( priority );
        on_prioritySlider_valueChanged( priority );
    }
}

void TodoDialog::on_prioritySlider_valueChanged(int value)
{
    QString priorityText;
    switch ( value )
    {
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

    ui->prioritySlider->setToolTip( "priority: " + priorityText );
}

void TodoDialog::on_showCompletedItemsCheckBox_clicked()
{
    storeSettings();
    reloadTodoList();
}

void TodoDialog::on_saveButton_clicked()
{
    updateCurrentCalendarItemWithFormData();

    OwnCloudService *ownCloud = new OwnCloudService( crypto, this );

    // update the local icsData from server
    ownCloud->updateICSDataOfCalendarItem( &currentCalendarItem );

    // post the calendar item to the server
    ownCloud->postCalendarItemToServer( currentCalendarItem, this );

    qDebug()<< currentCalendarItem;
}

void TodoDialog::todoItemLoadingProgressBarIncrement()
{
    ui->todoItemLoadingProgressBar->show();
    int value = ui->todoItemLoadingProgressBar->value() + 1;

    if ( value <= 0 ) {
        value = 1;
    }

    ui->todoItemLoadingProgressBar->setValue( value );
}

void TodoDialog::todoItemLoadingProgressBarHide()
{
    ui->todoItemLoadingProgressBar->hide();
}

void TodoDialog::todoItemLoadingProgressBarSetMaximum( int value )
{
    ui->todoItemLoadingProgressBar->setMaximum( value );
    todoItemLoadingProgressBarHideIfOnMaximum();
}

void TodoDialog::todoItemLoadingProgressBarHideIfOnMaximum()
{
    if ( ui->todoItemLoadingProgressBar->value() >= ui->todoItemLoadingProgressBar->maximum() )
    {
        ui->todoItemLoadingProgressBar->hide();
    }
}

void TodoDialog::on_todoItemLoadingProgressBar_valueChanged( int value )
{
    todoItemLoadingProgressBarHideIfOnMaximum();
}

void TodoDialog::on_newItemEdit_returnPressed()
{
    CalendarItem calItem = CalendarItem::createNewTodoItem( ui->newItemEdit->text(), ui->todoListSelector->currentText() );
    lastCreatedCalendarItem = calItem;

    OwnCloudService *ownCloud = new OwnCloudService( crypto, this );

    // post the calendar item to the server
    ownCloud->postCalendarItemToServer( calItem, this );

//    if ( calItem.isFetched() )
//    {
//        qDebug() << __func__ << " - 'calItem': " << calItem;
//        reloadTodoListItems();
//    }

    ui->newItemEdit->clear();
}

/**
 * @brief Removes the currently selected todo list item from the ownCloud server
 */
void TodoDialog::on_removeButton_clicked()
{
    if ( QMessageBox::information( this, "Remove todo item",
                                      "Remove the selected todo item?\nThis cannot be undone!",
                                      "&Remove", "&Cancel", QString::null,
                                      0, 1 ) == 0 )
    {
        CalendarItem calItem = currentCalendarItem;

        // remove the calendar item from the list widget (this will update the currentCalendarItem)
        ui->todoList->takeItem( ui->todoList->currentRow() );

        // remove the calendar item from the database
        calItem.remove();

        // remove the calendar item from the ownCloud server (this will reload the todo list as well)
        OwnCloudService *ownCloud = new OwnCloudService( crypto, this );
        ownCloud->removeCalendarItem( calItem, this );
    }
}

/**
 * @brief Updates the completed state of a calendar item on the ownCloud server
 * @param item
 */
void TodoDialog::on_todoList_itemChanged(QListWidgetItem *item)
{
    qDebug() << __func__ << " - 'item': " << item;
    QString uid = item->whatsThis();

    CalendarItem calItem = CalendarItem::fetchByUid( uid );
    if ( calItem.isFetched() )
    {
        calItem.updateCompleted( item->checkState() == Qt::Checked );
        calItem.store();

        OwnCloudService *ownCloud = new OwnCloudService( crypto, this );

        // post the calendar item to the server
        ownCloud->postCalendarItemToServer( calItem, this );
    }
}

void TodoDialog::on_reminderCheckBox_clicked(bool checked)
{
    if ( checked )
    {
        ui->reminderDateTimeEdit->show();
    }
    else
    {
        ui->reminderDateTimeEdit->hide();
    }
}
