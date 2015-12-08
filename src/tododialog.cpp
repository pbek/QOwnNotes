#include "owncloudservice.h"
#include "tododialog.h"
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
 * @brief Fetches the items of the current todo list
 */
void TodoDialog::reloadTodoList()
{
    OwnCloudService *ownCloud = new OwnCloudService( crypto, this );
    ownCloud->todoGetTodoList( ui->todoListSelector->currentText(), this );
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
