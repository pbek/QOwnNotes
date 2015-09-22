#include "versiondialog.h"
#include "ui_versiondialog.h"
#include <QSettings>
#include <QDebug>
#include <QScriptValueIterator>
#include <QPushButton>

VersionDialog::VersionDialog(QScriptValue versions, MainWindow *mainWindow, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VersionDialog)
{
    this->mainWindow = mainWindow;
    ui->setupUi(this);
    setupMainSplitter();

    QPushButton *button;
    ui->buttonBox->clear();

    QIcon restoreIcon;
    restoreIcon.setThemeName( "view-restore" );

    QIcon cancelIcon;
    cancelIcon.setThemeName( "application-exit" );

    button = new QPushButton( tr( "&Restore selected version" ) );
    button->setProperty( "ActionRole", Restore );
    button->setDefault( false );
    button->setIcon( restoreIcon );
    ui->buttonBox->addButton( button, QDialogButtonBox::ActionRole );

    button = new QPushButton( tr( "&Cancel" ) );
    button->setProperty( "ActionRole", Cancel );
    button->setIcon( cancelIcon );
    button->setDefault( true );
    ui->buttonBox->addButton( button, QDialogButtonBox::ActionRole );

    connect( this->ui->buttonBox, SIGNAL( clicked( QAbstractButton* ) ), SLOT( dialogButtonClicked( QAbstractButton* ) ) );
    connect( this, SIGNAL( finished(int) ), this, SLOT( storeSettings() ) );

    QString itemName;
    QString diffHtml;
    ui->versionListWidget->clear();
    diffList = new QStringList();
    dataList = new QStringList();

    // init the iterator for the verions
    QScriptValueIterator versionsIterator( versions );

    // iterate over the versions
    while ( versionsIterator.hasNext() ) {
        versionsIterator.next();
//        qDebug() << versionsIterator.name() << ": " << versionsIterator.value().property( "timestamp" ).toString() << " - " << versionsIterator.value().property( "humanReadableTimestamp" ).toString() << " - " << versionsIterator.value().property( "diffHtml" ).toString();

        itemName = versionsIterator.value().property( "humanReadableTimestamp" ).toString();

        if ( itemName == "" ) {
            continue;
        }

        diffHtml = versionsIterator.value().property( "diffHtml" ).toString();
        diffHtml.replace( "<ins>", "<span style='background-color: rgb(214, 255, 199)'>" );
        diffHtml.replace( "</ins>", "</span>" );
        diffHtml.replace( "<del>", "<span style='background-color: rgb(255, 215, 215)'>" );
        diffHtml.replace( "</del>", "</span>" );
        diffHtml.replace( "\n", "<br />" );

        ui->versionListWidget->addItem( itemName );
        diffList->append( diffHtml );
        dataList->append( versionsIterator.value().property( "data" ).toString() );
    }

    ui->versionListWidget->setCurrentRow( 0 );
    ui->diffBrowser->setHtml( diffList->at( 0 ) );
}

void VersionDialog::setupMainSplitter()
{
    this->versionSplitter = new QSplitter;

    this->versionSplitter->addWidget(ui->versionListWidget);
    this->versionSplitter->addWidget(ui->diffBrowser);

    // restore splitter sizes
    QSettings settings;
    QByteArray state = settings.value( "versionSplitterSizes" ).toByteArray();
    this->versionSplitter->restoreState( state );

    this->ui->gridLayout->layout()->addWidget( this->versionSplitter );
    this->ui->gridLayout->layout()->addWidget( ui->buttonBox );
}

void VersionDialog::storeSettings()
{
    // store the splitter sizes
    QSettings settings;
    settings.setValue( "versionSplitterSizes", this->versionSplitter->saveState() );
}

VersionDialog::~VersionDialog()
{
    delete ui;
}

void VersionDialog::on_versionListWidget_currentRowChanged(int currentRow)
{
    ui->diffBrowser->setHtml( diffList->value( currentRow ) );
}

void VersionDialog::dialogButtonClicked( QAbstractButton *button )
{
    int actionRole = button->property("ActionRole").toInt();

    if ( actionRole == Restore )
    {
        mainWindow->setCurrentNoteText( dataList->value( ui->versionListWidget->currentRow() ) );
    }

    this->close();
}
