#include "entities/calendaritem.h"
#include "services/owncloudservice.h"
#include "dialogs/tododialog.h"
#include "ui_tododialog.h"

#include <QSettings>

TodoDialog::TodoDialog(SimpleCrypt *crypto, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TodoDialog)
{
    this->crypto = crypto;

    ui->setupUi(this);
    setupMainSplitter();
    loadTodoListData();
}

TodoDialog::~TodoDialog()
{
    delete ui;
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

void TodoDialog::loadTodoListData()
{
    QSettings settings;
    ui->todoListSelector->clear();
    ui->todoListSelector->addItems( settings.value( "ownCloud/todoCalendarEnabledList" ).toStringList() );
}

/**
 * @brief Fetches the items of the current todo list from ownCloud
 */
void TodoDialog::reloadTodoList()
{
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
        ui->todoList->setCurrentRow( 0 );
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
//    settings.setValue( "TodoDialog/menuBarGeometry", ui->menuBar->saveGeometry() );
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

    CalendarItem calItem = CalendarItem::fetchByUid( uid );
    if ( calItem.isFetched() )
    {
        ui->summaryEdit->setText( calItem.getSummary() );
        ui->summaryEdit->setCursorPosition( 0 );
        ui->descriptionEdit->setText( calItem.getDescription() );
        ui->prioritySlider->setValue( calItem.getPriority() );
    }
}

void TodoDialog::on_prioritySlider_valueChanged(int value)
{
    ui->prioritySlider->setToolTip( "priority: " + QString::number( value ) );
}
