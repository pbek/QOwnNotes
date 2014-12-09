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
#include <QMessageBox>
#include <QKeyEvent>
#include "note.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QString appNameAdd = "";

    #ifdef QT_DEBUG
      appNameAdd = "Debug";
    #endif

    QCoreApplication::setOrganizationDomain( "PBE" );
    QCoreApplication::setOrganizationName( "PBE" );
    QCoreApplication::setApplicationName( "QOwnNotes" + appNameAdd );
    QCoreApplication::setApplicationVersion( "0.1" );

    ui->setupUi(this);

    Note::createConnection();

    this->firstVisibleNoteListRow = 0;

    readSettings();
    setupMainSplitter();
    buildNotesIndex();
    loadNoteDirectoryList();

    QTimer *timer = new QTimer( this );
    QObject::connect( timer, SIGNAL( timeout()), this, SLOT( storeUpdatedNotesToDisk() ) );
    timer->start( 1000 );

    QObject::connect( &this->noteDirectoryWatcher, SIGNAL( directoryChanged( QString ) ), this, SLOT( notesDirectoryWasModified( QString ) ) );
    QObject::connect( &this->noteDirectoryWatcher, SIGNAL( fileChanged( QString ) ), this, SLOT( notesWereModified( QString ) ) );
    ui->searchLineEdit->installEventFilter(this);
    ui->notesListWidget->installEventFilter(this);
    ui->notesListWidget->setCurrentRow( 0 );
}

MainWindow::~MainWindow()
{
    storeUpdatedNotesToDisk();
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
    QSettings settings;
    QByteArray state = settings.value( "mainSplitterSizes" ).toByteArray();
    this->mainSplitter->restoreState( state );

    this->ui->gridLayout->layout()->addWidget( this->mainSplitter );
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
    QSettings settings;
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());

    this->notesPath = settings.value("General/notesPath").toString();

    // let us select a folder if we haven't find one in the settings
    if ( this->notesPath == "" )
    {
        selectOwnCloudFolder();
    }
}

void MainWindow::notesWereModified( const QString& str )
{
    qDebug() << "notesWereModified: " << str;

    QFileInfo fi( str );
    Note note = Note::fetchByFileName( fi.fileName() );

    // load note from disk if current note was changed
    if ( note.getFileName() == this->currentNote.getFileName() ) {
        if ( note.fileExists() )
        {
            qDebug() << "Current note was modified externaly!";

            switch( QMessageBox::information( this, "Note was modified externaly!",
                                              "The current note was modified outside of this application!\n"
                                              "Reload current note from disk?",
                                              "&Reload", "&Cancel", QString::null,
                                              0, 1 ) )
            {
                case 0:
                    note.updateNoteTextFromDisk();

                    {
                        const QSignalBlocker blocker( this->ui->noteTextEdit );
                        this->ui->noteTextEdit->setText( note.getNoteText() );
                    }
                    break;
                case 1:
                default:
                    break;
            }
        }
        else
        {
            qDebug() << "Current note was removed externaly!";

            switch( QMessageBox::information( this, "Note was removed externaly!",
                                              "Current note was removed outside of this application!\n"
                                              "Restore current note?",
                                              "&Restore", "&Cancel", QString::null,
                                              0, 1 ) )
            {
                case 0:
                    {
                        const QSignalBlocker blocker( this->noteDirectoryWatcher );

                        QString text = this->ui->noteTextEdit->toPlainText();
                        note.storeNewText( text );

                        // store note to disk again
                        note.storeNoteTextFileToDisk();

                        // rebuild and reload the notes directory list
                        buildNotesIndex();
                        loadNoteDirectoryList();

                        // fetch note new (because all the IDs have changed after the buildNotesIndex()
                        note.refetch();

                        // restore old selected row (but don't update the note text)
                        setCurrentNote( note, false );
                    }
                    break;
                case 1:
                default:
                    break;
            }
        }
    }
    else
    {
        qDebug() << "other note was changed: " << str;

        // rebuild and reload the notes directory list
        buildNotesIndex();
        loadNoteDirectoryList();
    }
}

void MainWindow::notesDirectoryWasModified( const QString& str )
{
    qDebug() << "notesDirectoryWasModified: " << str;

    // rebuild and reload the notes directory list
    buildNotesIndex();
    loadNoteDirectoryList();

    // also update the text of the text edit if current note has changed
    bool updateNoteText = !this->currentNote.exists();
    qDebug() << "updateNoteText: " << updateNoteText;

    // restore old selected row (but don't update the note text)
    setCurrentNote( this->currentNote, updateNoteText );
}


void MainWindow::storeUpdatedNotesToDisk()
{
//        qDebug() << "checkForNoteChanges";
    {
        const QSignalBlocker blocker( this->noteDirectoryWatcher );

        // For some reason this->noteDirectoryWatcher gets an event from this.
        // I didn't find an other solution than to wait yet.
        // All flushing and syncing didn't help
        int count = Note::storeDirtyNotesToDisk();

        // wait 100ms before the block on this->noteDirectoryWatcher is opened
        if ( count > 0 )
        {
            qDebug() << __func__ << " - 'count': " << count;

            QTime dieTime= QTime::currentTime().addMSecs( 100 );
            while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
        }
    }
}

void MainWindow::buildNotesIndex()
{
    // make sure we destroy nothing
    this->storeUpdatedNotesToDisk();

    QDir notesDir( this->notesPath );
//    qDebug() << this->notesPath;

    // only show text files
    QStringList filters;
    filters << "*.txt";

    // show newest entry first
    QStringList files = notesDir.entryList( filters, QDir::Files, QDir::Time );

    // delete all notes in the database first
    Note::deleteAll();

    // create all notes from the files
    Q_FOREACH( QString fileName, files )
    {
        // fetching the content of the file
        QFile file( Note::fullNoteFilePath( fileName ) );
        Note note;
        note.createFromFile( file );
    }

    // refetch current note (because all the IDs have changed after the buildNotesIndex()
    this->currentNote.refetch();
}

QString MainWindow::selectOwnCloudFolder() {
    QString path = this->notesPath;

    if ( path == "" )
    {
        path = QDir::homePath() + QDir::separator() + "ownCloud" + QDir::separator() + "Notes";
    }

    QString dir = QFileDialog::getExistingDirectory( this, tr( "Select ownCloud folder" ),
                                                 path,
                                                 QFileDialog::ShowDirsOnly );

    if ( dir != "" )
    {
        this->notesPath = dir;
        QSettings settings;
        settings.setValue( "General/notesPath", dir );
    }
    else
    {
        if ( this->notesPath == "" )
        {
            switch( QMessageBox::information( this, "No folder was selected",
                                              "You have to select your ownCloud notes folder to make this software work!",
                                              "&Retry", "&Exit", QString::null,
                                              0, 1 ) )
            {
                case 0:
                    selectOwnCloudFolder();
                    break;
                case 1:
                default:
                    // No other way to quit the application worked
                    QTimer::singleShot( 50, this, SLOT( on_action_Quit_triggered() ) );
                    break;
            }
        }
    }

    return this->notesPath;
}

void MainWindow::setCurrentNote( Note note )
{
    setCurrentNote( note, true );
}

void MainWindow::setCurrentNote( Note note, bool updateNoteText )
{
    this->currentNote = note;
    QString name = note.getName();

    // find and set the current item
    QList<QListWidgetItem*> items = this->ui->notesListWidget->findItems( name, Qt::MatchExactly );
    if ( items.count() > 0 )
    {
        const QSignalBlocker blocker( this->ui->notesListWidget );

        this->ui->notesListWidget->setCurrentItem( items[0] );
    }

    // update the text of the text edit
    if ( updateNoteText )
    {
        const QSignalBlocker blocker( this->ui->noteTextEdit );

        this->ui->noteTextEdit->setText( note.getNoteText() );
    }
}

void MainWindow::focusNoteTextEdit()
{
    // move the cursor to the 2nd line
    QTextCursor tmpCursor = ui->noteTextEdit->textCursor();
    tmpCursor.movePosition( QTextCursor::Start, QTextCursor::MoveAnchor );
    tmpCursor.movePosition( QTextCursor::Down, QTextCursor::MoveAnchor );
    ui->noteTextEdit->setTextCursor( tmpCursor );

    // focus note text edit
    ui->noteTextEdit->setFocus();
}

void MainWindow::removeCurrentNote()
{
    switch( QMessageBox::information( this, "Remove current note",
                                      "Remove current note?\n" + this->currentNote.getName(),
                                      "&Remove", "&Cancel", QString::null,
                                      0, 1 ) )
    {
        case 0:
            this->currentNote.remove( true );
            loadNoteDirectoryList();
            break;
        case 1:
        default:
            break;
    }
}


/*!
 * Internal events
 */

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue( "MainWindow/geometry", saveGeometry() );
    settings.setValue( "MainWindow/windowState", saveState() );
    settings.setValue( "mainSplitterSizes", this->mainSplitter->saveState() );
    QMainWindow::closeEvent( event );
}

//
// Event filters on the MainWindow
//
bool MainWindow::eventFilter(QObject* obj, QEvent *event)
{
    if ( obj == ui->searchLineEdit )
    {
        if ( event->type() == QEvent::KeyPress )
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            // set focus to the notes list if Key_Down or Key_Tab were pressed in the search line edit
            if ( ( keyEvent->key() == Qt::Key_Down ) || ( keyEvent->key() == Qt::Key_Tab ) )
            {
                // choose an other selected item if current item is invisible
                QListWidgetItem *item = ui->notesListWidget->currentItem();
                if ( ( item != NULL ) && ui->notesListWidget->currentItem()->isHidden() && ( this->firstVisibleNoteListRow >= 0 ) )
                {
                    ui->notesListWidget->setCurrentRow( this->firstVisibleNoteListRow );
                }

                // give the keyboard focus to the notes list widget
                ui->notesListWidget->setFocus();
                return true;
            }
        }
        return false;
    }
    else if ( obj == ui->notesListWidget )
    {
        if ( event->type() == QEvent::KeyPress )
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            // set focus to the note text edit if Key_Return or Key_Tab were pressed in the notes list
            if ( ( keyEvent->key() == Qt::Key_Return ) || ( keyEvent->key() == Qt::Key_Tab ) )
            {
                focusNoteTextEdit();
                return true;
            }
            else if ( ( keyEvent->key() == Qt::Key_Delete ) )
            {
                removeCurrentNote();
                return true;
            }
        }
        return false;
    }

    return QMainWindow::eventFilter(obj, event);
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
    setCurrentNote( note );

//    QString fileName = Note::fullNoteFilePath( note.getFileName() );
//    loadNote( fileName );
}

void MainWindow::on_noteTextEdit_textChanged()
{
//    qDebug() << "noteTextChanged";

    QString text = this->ui->noteTextEdit->toPlainText();
    this->currentNote.storeNewText( text );

    this->currentNote.refetch();

    qDebug() << __func__ << ": " << this->currentNote;




//    Note note = this->currentNote

//    {
//        const QSignalBlocker blocker( this->noteDirectoryWatcher );

//        this->currentNote.storeNoteTextFileToDisk();
//    }
}


void MainWindow::on_action_Quit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionSet_ownCloud_Folder_triggered()
{
    selectOwnCloudFolder();

    // TODO: reload files if folder was changed
}

void MainWindow::on_searchLineEdit_textChanged(const QString &arg1)
{
    // search notes when at least 2 characters were entered
    if ( arg1.count() >= 2 )
    {
        QList<QString> noteNameList = Note::searchAsNameList( arg1 );
        this->firstVisibleNoteListRow = -1;

        for(int i = 0; i < this->ui->notesListWidget->count(); ++i)
        {
            QListWidgetItem* item = this->ui->notesListWidget->item(i);
            if ( noteNameList.indexOf( item->text() ) < 0 )
            {
                item->setHidden( true );
            }
            else
            {
                if ( this->firstVisibleNoteListRow < 0 ) this->firstVisibleNoteListRow = i;
                item->setHidden( false );
            }
        }
    }
    // show all items otherwise
    else
    {
        this->firstVisibleNoteListRow = 0;

        for(int i = 0; i < this->ui->notesListWidget->count(); ++i)
        {
            QListWidgetItem* item = this->ui->notesListWidget->item(i);
            item->setHidden( false );
        }
    }
}

//
// set focus on search line edit if Ctrl + F was pressed
//
void MainWindow::on_action_Find_note_triggered()
{
    this->ui->searchLineEdit->setFocus();
}

//
// jump to found note or create a new one if not found
//
void MainWindow::on_searchLineEdit_returnPressed()
{
    QString text = this->ui->searchLineEdit->text();
    text = text.trimmed();

    // first let us search for the entered text
    Note note = Note::fetchByName( text );

    // if we can't find a note we create a new one
    if ( note.getId() == 0 )
    {
        note = Note();
        note.setName( text );
        note.setNoteText( text + "\n" );
        note.store();

        // store the note to disk
        {
            const QSignalBlocker blocker( this->noteDirectoryWatcher );

            note.storeNoteTextFileToDisk();
        }

        buildNotesIndex();
        loadNoteDirectoryList();

        // fetch note new (because all the IDs have changed after the buildNotesIndex()
        note.refetch();

//        // create a new widget item for the note list
//        QListWidgetItem* widgetItem = new QListWidgetItem();
//        widgetItem->setText( text );

//        // insert the note at the top of the note list
//        {
//            const QSignalBlocker blocker( this->ui->notesListWidget );

//            ui->notesListWidget->insertItem( 0, widgetItem );
//        }
    }

    // jump to the found or created note
    setCurrentNote( note );

    // focus the note text edit and set the cursor correctly
    focusNoteTextEdit();
}

void MainWindow::on_action_Remove_note_triggered()
{
    removeCurrentNote();
}
