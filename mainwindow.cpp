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
#include "note.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    Note::createConnection();

    // TODO: make notes path selectable
    this->notesPath = QDir::homePath() + QDir::separator() + "ownCloud" + QDir::separator() + "Notes.bak";
    QSettings settings( "PBE", "QNotes" );
    settings.setValue( "General/notesPath", this->notesPath );

    readSettings();
    setupMainSplitter();
    buildNotesIndex();
    loadNoteDirectoryList();

    QList<Note> noteList = Note::fetchAll();

//    QTimer *timer = new QTimer( this );
//    QObject::connect( timer, SIGNAL( timeout()), this, SLOT( checkForNoteChanges() ) );
//    timer->start( 1000 );

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
    {
        const QSignalBlocker blocker( this->ui->noteTextEdit );

        {
            const QSignalBlocker blocker2( this->ui->notesListWidget );

            this->ui->notesListWidget->clear();

            QStringList nameList = Note::fetchNoteNames();
            this->ui->notesListWidget->addItems( nameList );
        }
    }

    // watch the notes directory for changes
    this->noteDirectoryWatcher.addPath( this->notesPath );

    QStringList fileNameList = Note::fetchNoteFileNames();

    // watch all the notes for changes
    Q_FOREACH( QString fileName, fileNameList )
    {
        this->noteDirectoryWatcher.addPath( Note::fullNoteFilePath( fileName ) );
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
    {
        const QSignalBlocker blocker( this->ui->noteTextEdit );
        this->ui->noteTextEdit->setText( noteText );
    }
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

    QFileInfo fi( str );
    qDebug() << fi.fileName();

    Note note = Note::fetchByFileName( fi.fileName() );

    // load note from disk if current note was changed
    if ( note.getFileName() == this->currentNote.getFileName() ) {
        qDebug() << "Current note was changed!";
        note.updateNoteTextFromDisk();

        {
            const QSignalBlocker blocker( this->ui->noteTextEdit );
            this->ui->noteTextEdit->setText( note.getNoteText() );
        }
    }

//    // reload the notes directory list
//    this->loadNoteDirectoryList();

//    // restore old selected row
//    this->ui->notesListWidget->blockSignals( true );
//    this->ui->notesListWidget->setCurrentRow( note.getId() - 1 );
//    this->ui->notesListWidget->blockSignals( false );
}

void MainWindow::checkForNoteChanges()
{
    Note::storeDirtyNotesToDisk();
//    qDebug() << "checkForNoteChanges";
}

void MainWindow::buildNotesIndex()
{
    QDir notesDir( this->notesPath );
//    qDebug() << this->notesPath;

    // only show text files
    QStringList filters;
    filters << "*.txt";

    // show newest entry first
    QStringList files = notesDir.entryList( filters, QDir::Files, QDir::Time );

    Q_FOREACH( QString fileName, files )
    {
        // fetching the content of the file
        QFile file( Note::fullNoteFilePath( fileName ) );
        if ( file.open( QIODevice::ReadOnly ) )
        {
            QTextStream in( &file );

            // qDebug() << file.size() << in.readAll();
            QString noteText = in.readAll();
            file.close();

            // create a nicer name by removing ".txt"
            QString base = fileName;
            base.chop( 4 );

            // store note
            Note::addNote( base, fileName, noteText );
//            qDebug() << fileName;
        }
    }
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

    int id = this->ui->notesListWidget->currentIndex().row() + 1;

    Note note = Note::fetch( id );
    this->currentNote = note;

    {
        const QSignalBlocker blocker( this->ui->noteTextEdit );
        // no signals here
        this->ui->noteTextEdit->setText( note.getNoteText() );
    }

//    QString fileName = Note::fullNoteFilePath( note.getFileName() );
//    loadNote( fileName );
}

void MainWindow::on_noteTextEdit_textChanged()
{
    // TODO: don't save after every text change, use a delay of one second to save
    qDebug() << "textChanged";

//    int id = this->ui->notesListWidget->currentIndex().row() + 1;
//    Note note = Note::fetch( id );

    QString text = this->ui->noteTextEdit->toPlainText();

    // BUG: this signal is not blocked somehow!
//    qDebug() << "blocking";

    {
        const QSignalBlocker blocker( this->noteDirectoryWatcher );
        // no signals here
        this->currentNote.storeNewText( text );
        this->currentNote.storeNoteTextFileToDisk();
    }
//    this->noteDirectoryWatcher.blockSignals( true );
//    this->currentNote.storeNewText( text );
//    this->currentNote.storeNoteTextFileToDisk();
//    qDebug() << "unblocking";
//    this->noteDirectoryWatcher.blockSignals( false );

//    QString fileName = fullNoteFilePath( note.getFileName() );
//    storeNote( fileName, text );
}

