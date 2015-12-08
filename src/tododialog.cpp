#include "tododialog.h"
#include "ui_tododialog.h"

#include <QSettings>

TodoDialog::TodoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TodoDialog)
{
    ui->setupUi(this);
    setupMainSplitter();
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
    QByteArray state = settings.value( "TodoDialog/mainSplitterSizes" ).toByteArray();
    this->mainSplitter->restoreState( state );

    this->ui->gridLayout->layout()->addWidget( this->mainSplitter );
}

void TodoDialog::storeSettings()
{
    QSettings settings;
    settings.setValue( "TodoDialog/geometry", saveGeometry() );
    settings.setValue( "TodoDialog/mainSplitterSizes", this->mainSplitter->saveState() );
//    settings.setValue( "TodoDialog/menuBarGeometry", ui->menuBar->saveGeometry() );
}

void TodoDialog::on_TodoDialog_finished(int result)
{
    storeSettings();
}
