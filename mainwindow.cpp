#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSplitter>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QSettings>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // TODO: make notes path selectable
    this->notesPath = QDir::homePath() + QDir::separator() + "ownCloud" + QDir::separator() + "Notes.bak";

    readSettings();
    setupMainSplitter();
    buildNotesIndex();
    loadNoteDirectoryList();

    QTimer *timer = new QTimer( this );
    QObject::connect( timer, SIGNAL( timeout()), this, SLOT( checkForNoteChanges() ) );
    timer->start( 1000 );

    QObject::connect( &this->noteDirectoryWatcher, SIGNAL( directoryChanged( QString ) ), this, SLOT( notesWereModified( QString ) ) );
    QObject::connect( &this->noteDirectoryWatcher, SIGNAL( fileChanged( QString ) ), this, SLOT( notesWereModified( QString ) ) );
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*!
 * Methods
 */

void MainWindow::setupMainSplitter()
{
    this->mainSplitter = new QSplitter;

    this->mainSplitter->addWidget(ui->notesListWidget);
    this->mainSplitter->addWidget(ui->noteTextEdit);

    // restore splitter sizes
    QSettings settings("PBE", "QNotes");
    QByteArray state = settings.value( "mainSplitterSizes" ).toByteArray();
    this->mainSplitter->restoreState( state );

    this->ui->centralWidget->layout()->addWidget( this->mainSplitter );
}

void MainWindow::loadNoteDirectoryList()
{
    QDir notesDir( this->notesPath );

    // only show text files
    QStringList filters;
    filters << "*.txt";

    // show newest entry first
    QStringList files = notesDir.entryList( filters, QDir::Files, QDir::Time );

    this->ui->noteTextEdit->blockSignals( true );
    this->ui->notesListWidget->blockSignals( true );

//    while( this->ui->notesListWidget->count()>0 )
//    {
//      this->ui->notesListWidget->takeItem(0);
//    }

    this->ui->notesListWidget->clear();

    this->ui->notesListWidget->addItems( files );
    this->ui->noteTextEdit->blockSignals( false );
    this->ui->notesListWidget->blockSignals( false );

    // watch the notes directory for changes
    this->noteDirectoryWatcher.addPath( this->notesPath );

    // watch all the notes for changes
    Q_FOREACH( QString fileName, files )
    {
        this->noteDirectoryWatcher.addPath( fullNoteFilePath( fileName ) );
    }

//    QStringList directoryList = this->noteDirectoryWatcher.directories();

//    Q_FOREACH(QString directory, directoryList)
//        qDebug() << "Directory name" << directory <<"\n";

}

void MainWindow::loadNote( QString &fileName )
{
    QFile file( fileName );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in( &file );

    // qDebug() << file.size() << in.readAll();
    QString noteText = in.readAll();
    file.close();

    // we don't want a "textChange()" fired up
    this->ui->noteTextEdit->blockSignals( true );
    this->ui->noteTextEdit->setText( noteText );
    this->ui->noteTextEdit->blockSignals( false );
}

void MainWindow::storeNote( QString &fileName, QString &text )
{
    QFile file( fileName );
    file.open( QIODevice::WriteOnly | QIODevice::Text );
    QTextStream out( &file );
    out << text;
    file.flush();
    file.close();
}

void MainWindow::readSettings()
{
    QSettings settings("PBE", "QNotes");
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());
}

void MainWindow::notesWereModified( const QString& str )
{
    qDebug() << "notesWereModified: " << str;
    Q_UNUSED(str);
//    QMessageBox::information(this,"Directory Modified", "Your Directory is modified");

    // reaload the notes directory list
    this->loadNoteDirectoryList();
}

void MainWindow::checkForNoteChanges()
{
//    qDebug() << "checkForNoteChanges";
}

void MainWindow::buildNotesIndex()
{
    QDir notesDir( this->notesPath );

    // only show text files
    QStringList filters;
    filters << "*.txt";

    // show newest entry first
    QStringList files = notesDir.entryList( filters, QDir::Files, QDir::Time );

    this->notesTextHash.clear();
    this->notesNameHash.clear();

    Q_FOREACH( QString fileName, files )
    {
        // fetching the content of the file
        QFile file( fullNoteFilePath( fileName ) );
        if ( file.open( QIODevice::ReadOnly ) )
        {
            QTextStream in( &file );

            // qDebug() << file.size() << in.readAll();
            QString noteText = in.readAll();
            file.close();

            this->notesTextHash.insert( fileName, noteText );
        }

        // create a nicer name by removing ".txt"
        QString base = fileName;
        base.chop( 4 );
        this->notesNameHash.insert( fileName, base );
    }
}

QString MainWindow::fullNoteFilePath( QString fileName )
{
    return this->notesPath + QDir::separator() + fileName;
}



/*!
 * Internal events
 */

void MainWindow::closeEvent(QCloseEvent *event)
 {
    QSettings settings( "PBE", "QNotes" );
    settings.setValue( "MainWindow/geometry", saveGeometry() );
    settings.setValue( "MainWindow/windowState", saveState() );
    settings.setValue( "mainSplitterSizes", this->mainSplitter->saveState() );
    QMainWindow::closeEvent( event );
 }



 /*!
  * Slots implementation
  */

void MainWindow::on_notesListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);
    qDebug() << "currentItemChanged " << current->text();
    QString fileName = fullNoteFilePath( current->text() );
    loadNote( fileName );
}

void MainWindow::on_noteTextEdit_textChanged()
{
    // TODO: don't save after every text change, use a delay of one second to save
    qDebug() << "textChanged";
    QString fileName = fullNoteFilePath( this->ui->notesListWidget->currentItem()->text() );
    QString text = this->ui->noteTextEdit->toPlainText();
    storeNote( fileName, text );
}

