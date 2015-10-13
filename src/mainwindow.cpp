#include "mainwindow.h"
#include "notesearchdialog.h"
#include "owncloudservice.h"
#include "ui_mainwindow.h"
#include <QSplitter>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QSettings>
#include <QTimer>
#include <QKeyEvent>
#include <QDesktopServices>
#include <QActionGroup>
#include <QSystemTrayIcon>
#include "diff_match_patch/diff_match_patch.h"
#include "notediffdialog.h"
#include "build_number.h"
#include "version.h"
#include "aboutdialog.h"
#include "settingsdialog.h"


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
    QCoreApplication::setApplicationVersion( "version " + QString( VERSION ) + " - build " + QString::number( BUILD ) );

    ui->setupUi(this);
    this->setWindowTitle( "QOwnNotes - version " + QString( VERSION ) + " - build " + QString::number( BUILD ) );
    QActionGroup *sorting = new QActionGroup( this );
    sorting->addAction( ui->actionAlphabetical );
    sorting->addAction( ui->actionBy_date );
    sorting->setExclusive( true );

    ui->searchInNoteWidget->hide();

    Note::createConnection();

    this->firstVisibleNoteListRow = 0;

    // set our signal mapper
    this->signalMapper = new QSignalMapper(this);

    readSettings();
    setupCrypto();

    // set sorting
    ui->actionBy_date->setChecked( !sortAlphabetically );
    ui->actionAlphabetical->setChecked( sortAlphabetically );

    // set the show in system tray checkbox
    ui->actionShow_system_tray->setChecked( showSystemTray );

    createSystemTrayIcon();
    setupMainSplitter();
    buildNotesIndex();
    loadNoteDirectoryList();
    this->noteDiffDialog = new NoteDiffDialog();

    // look if we need to save something every 10 sec
    QTimer *timer = new QTimer( this );
    QObject::connect( timer, SIGNAL( timeout()), this, SLOT( storeUpdatedNotesToDisk() ) );
    timer->start( 10000 );

    QObject::connect( &this->noteDirectoryWatcher, SIGNAL( directoryChanged( QString ) ), this, SLOT( notesDirectoryWasModified( QString ) ) );
    QObject::connect( &this->noteDirectoryWatcher, SIGNAL( fileChanged( QString ) ), this, SLOT( notesWereModified( QString ) ) );
    ui->searchLineEdit->installEventFilter(this);
    ui->notesListWidget->installEventFilter(this);
    ui->searchInNoteWidget->installEventFilter(this);
    ui->noteTextEdit->installEventFilter(this);
    ui->notesListWidget->setCurrentRow( 0 );

    // setup markdown highlighting
    highlighter = new HGMarkdownHighlighter( ui->noteTextEdit->document(), 1000 );

    // set the tab stop to the width of 4 spaces in the editor
    const int tabStop = 4;
    QFont font = ui->noteTextEdit->font();
    QFontMetrics metrics(font);
    ui->noteTextEdit->setTabStopWidth( tabStop * metrics.width( ' ' ) );

    // set the edit mode for the note text edit
    this->setNoteTextEditMode( true );

    // load the recent note folder list in the menu
    this->loadRecentNoteFolderListMenu();

    this->updateService = new UpdateService(this);
    this->updateService->checkForUpdates( UpdateService::AppStart );

    QTimer *updateCheckTimer = new QTimer(this);
    connect( updateCheckTimer, SIGNAL( timeout() ), this, SLOT( updateCheckTimerTimeout() ) );
    // update check every 2h
    updateCheckTimer->start( 7200000 );

    // update the current folder tooltip
    updateCurrentFolderTooltip();
}

MainWindow::~MainWindow()
{
    storeUpdatedNotesToDisk();
    delete ui;
}


/*!
 * Methods
 */

/*
 * Loads the menu entries for the recent note folders
 */
void MainWindow::loadRecentNoteFolderListMenu()
{
    QSettings settings;
    QStringList recentNoteFolders = settings.value( "recentNoteFolders" ).toStringList();

    int maxItems = 15;
    // remove items if there are too many
    if ( recentNoteFolders.length() > maxItems )
    {
        // remove an item as long as there are too many of them
        do
        {
            recentNoteFolders.removeAt( maxItems );
        }
        while ( recentNoteFolders.length() > maxItems );

        settings.setValue( "recentNoteFolders", recentNoteFolders );
    }

    // clear menu list
    this->ui->menuRecentNoteFolders->clear();

    // populate menu list
    Q_FOREACH( QString noteFolder, recentNoteFolders )
    {
        // add a menu entry
        QAction *action = this->ui->menuRecentNoteFolders->addAction( noteFolder );
        QObject::connect( action, SIGNAL( triggered() ), signalMapper, SLOT( map() ) );

        // add a parameter to changeNoteFolder with the signal mapper
        signalMapper->setMapping( action, noteFolder );
        QObject::connect( signalMapper, SIGNAL( mapped( const QString & ) ), this, SLOT( changeNoteFolder( const QString & ) ) );
    }
}

/*
 * Set a new note folder
 */
void MainWindow::changeNoteFolder( const QString &folderName )
{
    QString oldPath = this->notesPath;

    // reload notes if notes folder was changed
    if ( oldPath != folderName )
    {
        // store everything before changing folder
        storeUpdatedNotesToDisk();

        // update the recent note folder list
        storeRecentNoteFolder( this->notesPath, folderName );

        // change notes path
        this->notesPath = folderName;

        // store notesPath setting
        QSettings settings;
        settings.setValue( "notesPath", folderName );

        buildNotesIndex();
        loadNoteDirectoryList();

        const QSignalBlocker blocker( this->ui->noteTextEdit );
        {
            this->ui->noteTextEdit->clear();
        }

        const QSignalBlocker blocker2( this->ui->searchLineEdit );
        {
            this->ui->searchLineEdit->clear();
        }

        this->ui->noteTextView->clear();

        // update the current folder tooltip
        updateCurrentFolderTooltip();
    }
}

/*
 * Adds and removes a folder from the recent note folders
 */
void MainWindow::storeRecentNoteFolder( QString addFolderName, QString removeFolderName )
{
    QSettings settings;
    QStringList recentNoteFolders = settings.value( "recentNoteFolders" ).toStringList();

    recentNoteFolders.removeAll( addFolderName );
    recentNoteFolders.removeAll( removeFolderName );

    if ( addFolderName != removeFolderName )
    {
        recentNoteFolders.prepend( addFolderName );
    }

    settings.setValue( "recentNoteFolders", recentNoteFolders );

    // reload menu
    loadRecentNoteFolderListMenu();
}

int MainWindow::openNoteDiffDialog( Note changedNote )
{
    if ( this->noteDiffDialog->isVisible() )
    {
        this->noteDiffDialog->close();
    }

    QString text1 = this->ui->noteTextEdit->toPlainText();

    changedNote.updateNoteTextFromDisk();
    QString text2 = changedNote.getNoteText();

//    qDebug() << __func__ << " - 'text1': " << text1;
//    qDebug() << __func__ << " - 'text2': " << text2;

    diff_match_patch *diff = new diff_match_patch();
    QList<Diff> diffList = diff->diff_main( text1, text2 );

    QString html = diff->diff_prettyHtml( diffList );
//    qDebug() << __func__ << " - 'html': " << html;

    this->noteDiffDialog = new NoteDiffDialog( this, html );
    this->noteDiffDialog->exec();

    int result = this->noteDiffDialog->resultActionRole();
    return result;
}

void MainWindow::setupMainSplitter()
{
    this->mainSplitter = new QSplitter;

    this->mainSplitter->addWidget(ui->notesListWidget);
    this->mainSplitter->addWidget(ui->noteTabWidget);

    // restore splitter sizes
    QSettings settings;
    QByteArray state = settings.value( "mainSplitterSizes" ).toByteArray();
    this->mainSplitter->restoreState( state );

    this->ui->gridLayout->layout()->addWidget( this->mainSplitter );
}

void MainWindow::createSystemTrayIcon()
{
    trayIcon = new QSystemTrayIcon( this );
    trayIcon->setIcon( QIcon( ":/images/icon.png" ) );
    connect( trayIcon, SIGNAL( activated(QSystemTrayIcon::ActivationReason ) ),
            this, SLOT( systemTrayIconClicked( QSystemTrayIcon::ActivationReason ) ) );
    if (showSystemTray)
    {
        trayIcon->show();
    }
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

    // sort alphabetically again in neccessery
    if ( sortAlphabetically )
    {
        ui->notesListWidget->sortItems( Qt::AscendingOrder );
    }

//    QStringList directoryList = this->noteDirectoryWatcher.directories();

//    Q_FOREACH(QString directory, directoryList)
//        qDebug() << "Directory name" << directory <<"\n";

}

void MainWindow::readSettings()
{
    QSettings settings;
    sortAlphabetically = settings.value( "SortingModeAlphabetically", QVariant( false ) ).toBool();
    showSystemTray = settings.value( "ShowSystemTray", QVariant( false ) ).toBool();
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());
    ui->menuBar->restoreGeometry(settings.value("MainWindow/menuBarGeometry").toByteArray());

    // check legacy setting
    this->notesPath = settings.value( "General/notesPath" ).toString();

    // migration: remove old setting if we found one and store new one
    if ( this->notesPath != "" )
    {
        settings.setValue( "notesPath", this->notesPath );
        settings.remove( "General/notesPath" );
    }
    else
    {
        this->notesPath = settings.value("notesPath").toString();
    }

    // let us select a folder if we haven't find one in the settings
    if ( this->notesPath == "" )
    {
        selectOwnCloudNotesFolder();
    }

    // migration: remove notes path from recent note folders
    if ( this->notesPath != "" )
    {
        QStringList recentNoteFolders = settings.value( "recentNoteFolders" ).toStringList();
        if ( recentNoteFolders.contains( this->notesPath ) )
        {
            recentNoteFolders.removeAll( this->notesPath );
            settings.setValue( "recentNoteFolders", recentNoteFolders );
        }
    }
}

void MainWindow::updateNoteTextFromDisk( Note note )
{
    note.updateNoteTextFromDisk();
    note.store();
    this->currentNote = note;

    {
        const QSignalBlocker blocker( this->ui->noteTextEdit );
        this->setNoteTextFromNote( &note );
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
            // updating the current note text straight away if we didn't change it since the last store
            if ( !note.getHasDirtyData() )
            {
                updateNoteTextFromDisk( note );
                return;
            }

            // fetch current text
            QString text1 = this->ui->noteTextEdit->toPlainText();

            // fetch text of note from disk
            note.updateNoteTextFromDisk();
            QString text2 = note.getNoteText();

            // skip dialog if texts are equal
            if ( text1 == text2 )
            {
                return;
            }

            qDebug() << "Current note was modified externaly!";

            int result = openNoteDiffDialog( note );
            switch( result )
            {
                // overwrite file with local changes
                case NoteDiffDialog::Overwrite:
                    {
                        const QSignalBlocker blocker( this->noteDirectoryWatcher );
                        this->currentNote.store();
                        this->currentNote.storeNoteTextFileToDisk();
                        this->ui->statusBar->showMessage( tr("stored current note to disk"), 1000 );

                        // just to make sure everything is uptodate
//                        this->currentNote = note;
//                        this->setNoteTextFromNote( &note, true );

                        // wait 100ms before the block on this->noteDirectoryWatcher is opened, otherwise we get the event
                        waitMsecs( 100 );
                    }
                    break;

                // reload note file from disk
                case NoteDiffDialog::Reload:
                    updateNoteTextFromDisk( note );
                    break;

                case NoteDiffDialog::Cancel:
                case NoteDiffDialog::Ignore:
                default:
                    // do nothing
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
                        this->ui->statusBar->showMessage( tr("stored current note to disk"), 1000 );

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
       // setCurrentNote(this->currentNote  , false);

        // rebuild and reload the notes directory list
        buildNotesIndex();
        loadNoteDirectoryList();
        setCurrentNote(this->currentNote, false);
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
        int count = Note::storeDirtyNotesToDisk( this->currentNote );
//        qDebug() << __func__ << " - 'this->currentNote': " << this->currentNote;

        if ( count > 0 )
        {
            qDebug() << __func__ << " - 'count': " << count;

            this->ui->statusBar->showMessage( tr("stored %1 note(s) to disk").arg( count ), 1000 );

            // wait 100ms before the block on this->noteDirectoryWatcher is opened, otherwise we get the event
            waitMsecs( 100 );

            loadNoteDirectoryList();

            // just to make sure everything is uptodate
            this->currentNote.refetch();
        }
    }
}

void MainWindow::waitMsecs( int msecs ) {
    QTime dieTime= QTime::currentTime().addMSecs( msecs );
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
}

void MainWindow::buildNotesIndex()
{
    // make sure we destroy nothing
    storeUpdatedNotesToDisk();

    QDir notesDir( this->notesPath );
//    qDebug() << this->notesPath;

    // only show text files
    QStringList filters;
    filters << "*.txt";

    // show newest entry first
    QStringList files = notesDir.entryList( filters, QDir::Files, QDir::Time );

    // add some notes if there aren't any
    if ( files.count() == 0 )
    {
        qDebug() << "No notes! We will add some...";
        QStringList filenames;
        QString filename;
        QString destinationFile;

        filenames.append( "Welcome to QOwnNotes.txt" );
        filenames.append( "GitHub Flavored Markdown.txt" );
        filenames.append( "Markdown Showcase.txt" );

        // copy note files to the notes path
        for ( int i = 0; i < filenames.size(); ++i )
        {
            filename = filenames.at( i );
            destinationFile = this->notesPath + "/" + filename;
            QFile::copy( ":/demonotes/" + filename, destinationFile );
            // set read/write permissions for the owner and user
            QFile::setPermissions( destinationFile, QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser );
        }

        // fetch all files again
        files = notesDir.entryList( filters, QDir::Files, QDir::Time );

        // jump to the welcome note in the note selector in 500ms
        QTimer::singleShot( 500, this, SLOT( jumpToWelcomeNote() ) );
    }

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

/**
 * @brief Jumps to the welcome note in the note selector
 */
void MainWindow::jumpToWelcomeNote()
{
    // jump to the 3rd note, assuming that it is the welcome note
    this->ui->notesListWidget->setCurrentRow( 2 );
}

QString MainWindow::selectOwnCloudNotesFolder() {
    QString path = this->notesPath;

    if ( path == "" )
    {
        path = QDir::homePath() + QDir::separator() + "ownCloud" + QDir::separator() + "Notes";
    }

    // TODO: We sometimes seem to get a "QCoreApplication::postEvent: Unexpected null receiver" here.
    QString dir = QFileDialog::getExistingDirectory( this, tr( "Select ownCloud folder" ),
                                                 path,
                                                 QFileDialog::ShowDirsOnly );

    QDir d = QDir( dir );

    if ( d.exists() )
    {
        // lets remove trailing slashes
        dir = d.path();

        // update the recent note folder list
        storeRecentNoteFolder( this->notesPath, dir );

        this->notesPath = dir;
        QSettings settings;
        settings.setValue( "notesPath", dir );

        // update the current folder tooltip
        updateCurrentFolderTooltip();
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
                    selectOwnCloudNotesFolder();
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

void MainWindow::setCurrentNote( Note note, bool updateNoteText, bool updateSelectedNote )
{
    this->currentNote = note;
    QString name = note.getName();

    // set the note text edit to readonly if note file is not writeable
    QFileInfo *f = new QFileInfo( this->notesPath + QDir::separator() + note.getFileName() );
    ui->noteTextEdit->setReadOnly( !f->isWritable() );

    // find and set the current item
    if ( updateSelectedNote )
    {
        QList<QListWidgetItem*> items = this->ui->notesListWidget->findItems( name, Qt::MatchExactly );
        if ( items.count() > 0 )
        {
            const QSignalBlocker blocker( this->ui->notesListWidget );

            this->ui->notesListWidget->setCurrentItem( items[0] );
        }
    }

    // update the text of the text edit
    if ( updateNoteText )
    {
        const QSignalBlocker blocker( this->ui->noteTextEdit );
        this->setNoteTextFromNote( &note );
    }
}

void MainWindow::focusNoteTextEdit()
{
    // move the cursor to the 4nd line
    QTextCursor tmpCursor = ui->noteTextEdit->textCursor();
    tmpCursor.movePosition( QTextCursor::Start, QTextCursor::MoveAnchor );
    tmpCursor.movePosition( QTextCursor::Down, QTextCursor::MoveAnchor );
    tmpCursor.movePosition( QTextCursor::Down, QTextCursor::MoveAnchor );
    tmpCursor.movePosition( QTextCursor::Down, QTextCursor::MoveAnchor );
    ui->noteTextEdit->setTextCursor( tmpCursor );

    // focus note text edit
    ui->noteTextEdit->setFocus();
}

void MainWindow::removeCurrentNote()
{
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    switch( QMessageBox::information( this, "Remove current note",
                                      "Remove current note: <strong>" + this->currentNote.getName() + "</strong>?",
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

void MainWindow::storeSettings()
{
    QSettings settings;
    settings.setValue( "MainWindow/geometry", saveGeometry() );
    settings.setValue( "MainWindow/windowState", saveState() );
    settings.setValue( "mainSplitterSizes", this->mainSplitter->saveState() );
    settings.setValue( "MainWindow/menuBarGeometry", ui->menuBar->saveGeometry() );
    settings.setValue( "SortingModeAlphabetically", sortAlphabetically );
    settings.setValue( "ShowSystemTray", showSystemTray );
}


/*!
 * Internal events
 */

void MainWindow::closeEvent(QCloseEvent *event)
{
    storeSettings();
    QMainWindow::closeEvent( event );
}

//
// Event filters on the MainWindow
//
bool MainWindow::eventFilter(QObject* obj, QEvent *event)
{
    if ( event->type() == QEvent::KeyPress )
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if ( obj == ui->searchLineEdit )
        {
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
            return false;
        }
        else if ( obj == ui->notesListWidget )
        {
            // set focus to the note text edit if Key_Return or Key_Tab were pressed in the notes list
            if ( ( keyEvent->key() == Qt::Key_Return ) || ( keyEvent->key() == Qt::Key_Tab ) )
            {
                setNoteTextEditMode( true );
                focusNoteTextEdit();
                return true;
            }
            else if ( ( keyEvent->key() == Qt::Key_Delete ) )
            {
                removeCurrentNote();
                return true;
            }
            return false;
        }
        else if ( obj == ui->searchInNoteWidget )
        {
            if ( keyEvent->key() == Qt::Key_Escape )
            {
                closeSearchInNoteWidget();
                return true;
            }
            else if ( ( keyEvent->modifiers().testFlag( Qt::ShiftModifier ) && ( keyEvent->key() == Qt::Key_Return ) ) || ( keyEvent->key() == Qt::Key_Up ) )
            {
                doSearchInNote( false );
                return true;
            }
            else if ( ( keyEvent->key() == Qt::Key_Return ) || ( keyEvent->key() == Qt::Key_Down ) )
            {
                doSearchInNote();
                return true;
            }
            return false;
        }
        else if ( obj == ui->noteTextEdit )
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            if ( ( keyEvent->key() == Qt::Key_Escape ) && ui->searchInNoteWidget->isVisible() )
            {
                closeSearchInNoteWidget();
                return true;
            }
            return false;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

/**
 * highlights all occurrences of str in the note text edit
 */
void MainWindow::searchInNoteTextEdit( QString &str )
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    QList<QTextEdit::ExtraSelection> extraSelections2;

    if ( str.count() >= 2 )
    {
        ui->noteTextEdit->moveCursor( QTextCursor::Start );
        ui->noteTextView->moveCursor( QTextCursor::Start );
        QColor color = QColor( 0, 180, 0, 100 );

        while( ui->noteTextEdit->find( str ) )
        {
            QTextEdit::ExtraSelection extra;
            extra.format.setBackground( color );

            extra.cursor = ui->noteTextEdit->textCursor();
            extraSelections.append( extra );
        }

        while( ui->noteTextView->find( str ) )
        {
            QTextEdit::ExtraSelection extra;
            extra.format.setBackground( color );

            extra.cursor = ui->noteTextView->textCursor();
            extraSelections2.append( extra );
        }
    }

    ui->noteTextEdit->setExtraSelections( extraSelections );
    ui->noteTextView->setExtraSelections( extraSelections2 );
}

/**
 * highlights all occurrences of tje search line text in the note text edit
 */
void MainWindow::searchForSearchLineTextInNoteTextEdit()
{
    QString searchString = ui->searchLineEdit->text();
    searchInNoteTextEdit( searchString );
}

/**
 * switch edit mode for noteTextEdit on or off
 */
void MainWindow::setNoteTextEditMode( bool isInEditMode )
{
    this->noteTextEditIsInEditMode = isInEditMode;
    this->ui->actionToggleEditMode->setChecked( isInEditMode );
    this->ui->actionToggleEditMode->setToolTip( "Toogle edit mode - currently " + QString( isInEditMode ? "editing" : "viewing" ) );

    // set the tab index
    {
        const QSignalBlocker blocker( this->ui->noteTabWidget );
        this->ui->noteTabWidget->setCurrentIndex( isInEditMode ? 0 : 1 );
    }

    // make sure the current note is set
    if ( this->currentNote.exists() )
    {
        this->setCurrentNote( this->currentNote, true );
    }

    // restore search after switching between modes
    if ( ui->searchLineEdit->text() != "" )
    {
        searchForSearchLineTextInNoteTextEdit();
    }
}

/**
 * set the right note text according to whether noteText is in edit mode or not
 */
void MainWindow::setNoteTextFromNote( Note *note )
{
    setNoteTextFromNote( note, false );
}

/**
 * set the right note text according to whether noteText is in edit mode or not
 */
void MainWindow::setNoteTextFromNote( Note *note, bool updateNoteTextViewOnly )
{
    if ( !updateNoteTextViewOnly )
    {
        this->ui->noteTextEdit->setText( note->getNoteText() );
    }

    this->ui->noteTextView->setHtml( note->toMarkdownHtml() );
}

void MainWindow::setupCrypto()
{
    QSettings settings;
    qint64 cryptoKey = settings.value( "cryptoKey" ).toUInt();

    // generate a key if we don't have one
    if ( cryptoKey == 0 )
    {
        cryptoKey = qrand();
        settings.setValue( "cryptoKey", cryptoKey );
    }

    this->crypto = SimpleCrypt( cryptoKey );
}

/**
 * Sets the text of the current note.
 * This is a public callback function for the version dialog.
 *
 * @brief MainWindow::setCurrentNoteText
 * @param text
 */
void MainWindow::setCurrentNoteText( QString text )
{
    currentNote.setNoteText( text );
    ui->noteTextEdit->setText( text );
    this->ui->noteTextView->setHtml( currentNote.toMarkdownHtml() );
}

/**
 * Creates a new note (to restore a trashed note)
 * This is a public callback function for the trash dialog.
 *
 * @brief MainWindow::createNewNote
 * @param name
 * @param text
 */
void MainWindow::createNewNote( QString name, QString text )
{
    QFile* f = new QFile( this->notesPath + QDir::separator() + name + ".txt" );

    // change the name and headline if note exists
    if ( f->exists() )
    {
        QDateTime currentDate = QDateTime::currentDateTime();
        name.append( " " + currentDate.toString( Qt::ISODate ).replace( ":", "." ) );

        QString preText = name + "\n";

        for ( int i = 0; i < name.length(); i++ )
        {
            preText.append( "=" );
        }

        preText.append( "\n\n" );
        text.prepend( preText );
    }

    ui->searchLineEdit->setText( name );
    on_searchLineEdit_returnPressed();
    ui->noteTextEdit->setText( text );
}

/**
 * @brief Restores a trashed note on the server.
 * @param name
 * @param text
 *
 * This is a public callback function for the trash dialog.
 */
void MainWindow::restoreTrashedNoteOnServer( QString fileName, int timestamp )
{
    OwnCloudService *ownCloud = new OwnCloudService( &this->crypto, this );
    ownCloud->restoreTrashedNoteOnServer( this->notesPath, fileName, timestamp, this );
}

/**
 * @brief Removes selected notes after a confirmation
 */
void MainWindow::removeSelectedNotes()
{
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    int selectedItemsCount = ui->notesListWidget->selectedItems().size();

    if ( QMessageBox::information( this, "Remove selected notes",
                                      "Remove <strong>" + QString::number( selectedItemsCount ) + "</strong> selected note(s)?\n\nIf the trash is enabled on your ownCloud server you should be able to restore them from there.",
                                      "&Remove", "&Cancel", QString::null,
                                      0, 1 ) == 0 )
    {
        const QSignalBlocker blocker( this->noteDirectoryWatcher );

        Q_FOREACH( QListWidgetItem* item, ui->notesListWidget->selectedItems() )
        {
            QString name = item->text();
            Note note = Note::fetchByName( name );
            note.remove( true );
            qDebug() << "Removed note " << name;
        }

        loadNoteDirectoryList();
    }
}

/**
 * @brief Select all notes
 */
void MainWindow::selectAllNotes()
{
    ui->notesListWidget->selectAll();
}

/**
 * @brief Moves selected notes after a confirmation
 * @param destinationFolder
 */
void MainWindow::moveSelectedNotesToFolder( QString destinationFolder )
{
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    int selectedItemsCount = ui->notesListWidget->selectedItems().size();

    if ( QMessageBox::information( this, "Move selected notes",
                                      "Move " + QString::number( selectedItemsCount ) + " selected note(s) to <strong>" + destinationFolder + "</strong>?",
                                      "&Move", "&Cancel", QString::null,
                                      0, 1 ) == 0 )
    {
        const QSignalBlocker blocker( this->noteDirectoryWatcher );

        Q_FOREACH( QListWidgetItem* item, ui->notesListWidget->selectedItems() )
        {
            QString name = item->text();
            Note note = Note::fetchByName( name );

            // move note
            bool result = note.move( destinationFolder );
            if ( result )
            {
                qDebug() << "Note was moved:" << note.getName();
            }
            else
            {
                qDebug() << "Could not move note:" << note.getName();
            }
        }

        loadNoteDirectoryList();
    }
}

/**
 * @brief Copies selected notes after a confirmation
 * @param destinationFolder
 */
void MainWindow::copySelectedNotesToFolder( QString destinationFolder )
{
    int selectedItemsCount = ui->notesListWidget->selectedItems().size();

    if ( QMessageBox::information( this, "Copy selected notes",
                                      "Copy " + QString::number( selectedItemsCount ) + " selected note(s) to <strong>" + destinationFolder + "</strong>?",
                                      "&Copy", "&Cancel", QString::null,
                                      0, 1 ) == 0 )
    {
        int copyCount = 0;
        Q_FOREACH( QListWidgetItem* item, ui->notesListWidget->selectedItems() )
        {
            QString name = item->text();
            Note note = Note::fetchByName( name );

            // copy note
            bool result = note.copy( destinationFolder );
            if ( result )
            {
                copyCount++;
                qDebug() << "Note was copied:" << note.getName();
            }
            else
            {
                qDebug() << "Could not copy note:" << note.getName();
            }
        }

        QMessageBox::information( this, "Done", QString::number( copyCount ) + " note(s) were copied to <strong>" + destinationFolder + "</strong>." );
    }
}

/**
 * @brief Updates the current folder tooltip
 */
void MainWindow::updateCurrentFolderTooltip()
{
    ui->actionSet_ownCloud_Folder->setStatusTip( "Current notes folder: " + this->notesPath );
    ui->actionSet_ownCloud_Folder->setToolTip( "Set the notes folder. Current notes folder: " + this->notesPath );
}

/**
 * @brief Opens the settings dialog
 */
void MainWindow::openSettingsDialog()
{
    // open the settings dialog
    SettingsDialog *dialog = new SettingsDialog( &crypto, this );
    dialog->exec();
}

/**
 * @brief Handles the linking of text
 */
void MainWindow::handleTextNoteLinking()
{
    NoteSearchDialog *dialog = new NoteSearchDialog( "Search for a note to link to", this );
    dialog->exec();
    if ( dialog->result() == QDialog::Accepted  )
    {
        QString noteName = dialog->getSelectedNoteName();
        QString noteNameForLink = Note::generateTextForLink( noteName );

        if ( noteName != "" )
        {
            QString selectedText = ui->noteTextEdit->textCursor().selectedText();
            QString newText;

            if ( selectedText != "" )
            {
                newText = "[" + selectedText + "](note://" + noteNameForLink + ")";
            }
            else
            {
                newText = "<note://" + noteNameForLink + ">";
            }
            ui->noteTextEdit->textCursor().insertText( newText );
        }
    }
}

void MainWindow::closeSearchInNoteWidget()
{
    ui->searchInNoteWidget->hide();
    ui->noteTextEdit->setFocus();
}

/**
 * @brief Searches for text in the current note
 */
void MainWindow::doSearchInNote( bool searchDown )
{
    QString text = ui->searchInNoteEdit->text();

    if ( text == "" )
    {
        ui->searchInNoteEdit->setStyleSheet( "* { background: none; }" );
        return;
    }

    QTextDocument::FindFlag options = searchDown ? QTextDocument::FindFlag (0) : QTextDocument::FindBackward;
    bool found = ui->noteTextEdit->find( text, options );

    // start at the top if not found
    if ( !found )
    {
        ui->noteTextEdit->moveCursor( searchDown ? QTextCursor::Start : QTextCursor::End );
        found = ui->noteTextEdit->find( text, options );
    }

    // add a background color according if we found the text or not
    QString colorCode = found ? "#D5FAE2" : "#FAE9EB";
    ui->searchInNoteEdit->setStyleSheet( "* { background: " + colorCode + "; }" );
}

/**
 *
 *
 * Slots implementations
 *
 *
 */

void MainWindow::on_notesListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);
    qDebug() << "currentItemChanged " << current->text();

    Note note = Note::fetchByName( current->text() );
    setCurrentNote( note, true, false );

    // let's highlight the text from the search line edit
    searchForSearchLineTextInNoteTextEdit();

    // also do a "in note search" if the widget is visible
    if ( ui->searchInNoteWidget->isVisible() )
    {
        doSearchInNote();
    }
}

void MainWindow::on_noteTextEdit_textChanged()
{
//    qDebug() << "noteTextChanged";

    Note note = this->currentNote;
//    qDebug() << __func__ << " - 'this->getFilename()': " << note.getFileName();
    note.updateNoteTextFromDisk();
    QString noteTextFromDisk = note.getNoteText();

    QString text = this->ui->noteTextEdit->toPlainText();

    if ( text != noteTextFromDisk )
    {
        this->currentNote.storeNewText( text );
        this->currentNote.refetch();

        // qDebug() << __func__ << ": " << this->currentNote;
    }
}


void MainWindow::on_action_Quit_triggered()
{
    storeSettings();
    QApplication::quit();
}

void MainWindow::on_actionSet_ownCloud_Folder_triggered()
{
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    QString oldPath = this->notesPath;
    selectOwnCloudNotesFolder();

    // reload notes if notes folder was changed
    if ( oldPath != this->notesPath )
    {
        buildNotesIndex();
        loadNoteDirectoryList();

        const QSignalBlocker blocker( this->ui->noteTextEdit );
        {
            this->ui->noteTextEdit->clear();
        }

        this->ui->noteTextView->clear();
    }
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

    // let's highlight the text from the search line edit
    searchForSearchLineTextInNoteTextEdit();
}

//
// set focus on search line edit if Ctrl + F was pressed
//
void MainWindow::on_action_Find_note_triggered()
{
    this->ui->searchLineEdit->setFocus();
    this->ui->searchLineEdit->selectAll();
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
        // create a headline in new notes by adding "=====" as second line
        QString noteText = text + "\n";
        for ( int i = 0; i < text.length(); i++ )
        {
            noteText.append( "=" );
        }
        noteText.append( "\n\n" );

        note = Note();
        note.setName( text );
        note.setNoteText( noteText );
        note.store();

        // store the note to disk
        {
            const QSignalBlocker blocker( this->noteDirectoryWatcher );

            note.storeNoteTextFileToDisk();
            this->ui->statusBar->showMessage( tr("stored current note to disk"), 1000 );
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

    // go into edit mode
    setNoteTextEditMode( true );

    // focus the note text edit and set the cursor correctly
    focusNoteTextEdit();
}

void MainWindow::on_action_Remove_note_triggered()
{
    removeCurrentNote();
}

void MainWindow::on_actionAbout_QOwnNotes_triggered()
{
    AboutDialog *dialog = new AboutDialog( this );
    dialog->exec();
}

//
// hotkey to create new note with date in name
//
void MainWindow::on_action_Note_note_triggered()
{
    QDateTime currentDate = QDateTime::currentDateTime();

    // replacing ":" with "_" for Windows systems
    QString text = "Note " + currentDate.toString( Qt::ISODate ).replace( ":", "." );
    this->ui->searchLineEdit->setText( text );
    on_searchLineEdit_returnPressed();
}

void MainWindow::on_actionToggleEditMode_triggered()
{
    this->setNoteTextEditMode( this->ui->actionToggleEditMode->isChecked() );
}

void MainWindow::on_noteTabWidget_currentChanged(int index)
{
    this->setNoteTextEditMode( index == 0 );
}

/*
 * Handles urls in the noteTextView
 *
 * examples:
 * - <note://MyNote> opens the note "MyNote"
 * - <note://my-note-with-spaces-in-the-name> opens the note "My Note with spaces in the name"
 * - <http://www.qownnotes.org> opens the webpage
 * - <file:///path/to/my/file/QOwnNotes.pdf> opens the file "/path/to/my/file/QOwnNotes.pdf" if the operating system supports that handler
 */
void MainWindow::on_noteTextView_anchorClicked(const QUrl &url)
{
    if ( url.scheme() == "note" )
    {
        QString fileName = url.host() + ".txt";

        // this makes it possible to search for filenames containing spaces
        // instead of spaces a "-" has to be used in the note link
        // example: note://my-note-with-spaces-in-the-name
        fileName = fileName.replace( "-", "?" ).replace( "_", "?" );

        // we need to search for the case sensitive filename, we only get it lowercase by QUrl
        QDir currentDir = QDir( this->notesPath );
        QStringList files;

        // search for files with that name
        files = currentDir.entryList( QStringList( fileName ), QDir::Files | QDir::NoSymLinks );

        // did we find files?
        if ( files.length() > 0 )
        {
            // take the first found file
            fileName = files.at( 0 );

            // does the file name has more than 4 character (something.txt)?
            if ( fileName.length() > 4 )
            {
                // truncate the last for characters (.txt)
                fileName = fileName.left( fileName.length() - 4 );

                // try to fetch note
                Note note = Note::fetchByName( fileName );

                // does this note really exist?
                if ( note.isFetched() )
                {
                    // set current note
                    setCurrentNote( note );
                }
            }
        }
    }
    else
    {
        QDesktopServices::openUrl( url );
    }
}

/*
 * Manually check for updates
 */
void MainWindow::on_actionCheck_for_updates_triggered()
{
    this->updateService->checkForUpdates( UpdateService::Manual );
}

/*
 * Open the issue page
 */
void MainWindow::on_actionReport_problems_or_ideas_triggered()
{
    QDesktopServices::openUrl( QUrl( "https://github.com/pbek/QOwnNotes/issues" ) );
}

void MainWindow::on_actionAlphabetical_triggered(bool checked)
{
    if ( checked )
    {
        sortAlphabetically = true;
        ui->notesListWidget->sortItems( Qt::AscendingOrder );
    }
}

void MainWindow::on_actionBy_date_triggered(bool checked)
{
    if ( checked )
    {
        sortAlphabetically = false;
        loadNoteDirectoryList();
    }
}

void MainWindow::systemTrayIconClicked(QSystemTrayIcon::ActivationReason reason)
{
    if ( reason == QSystemTrayIcon::Trigger )
    {
        if (this->isVisible())
        {
            this->hide();
        }
        else
        {
            this->show();
        }
    }
}

void MainWindow::on_actionShow_system_tray_triggered(bool checked)
{
    showSystemTray = checked;
    if ( checked )
    {
        trayIcon->show();
    }
    else
    {
        trayIcon->hide();
    }
}

void MainWindow::on_action_Settings_triggered()
{
    // open the settings dialog
    openSettingsDialog();
}

void MainWindow::on_actionShow_versions_triggered()
{
    OwnCloudService *ownCloud = new OwnCloudService( &this->crypto, this );
    ownCloud->loadVersions( this->notesPath, this->currentNote.getFileName(), this );
}

void MainWindow::on_actionShow_trash_triggered()
{
    OwnCloudService *ownCloud = new OwnCloudService( &this->crypto, this );
    ownCloud->loadTrash( this->notesPath, this );
}

void MainWindow::updateCheckTimerTimeout()
{
    qDebug() << "updateCheck";
    this->updateService->checkForUpdates( UpdateService::Periodic );
}

void MainWindow::on_notesListWidget_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = ui->notesListWidget->mapToGlobal( pos );
    QMenu noteMenu;
    QMenu *moveDestinationMenu;
    QMenu *copyDestinationMenu;
    QSettings settings;
    QStringList recentNoteFolders = settings.value( "recentNoteFolders" ).toStringList();

    // show copy and move menu entries only if there is at least one notes folder
    if ( recentNoteFolders.size() > 0 )
    {
        moveDestinationMenu = noteMenu.addMenu( "&Move notes to..." );
        copyDestinationMenu = noteMenu.addMenu( "&Copy notes to..." );

        // add actions for the recent note folders
        Q_FOREACH( QString noteFolder, recentNoteFolders )
        {
            if ( noteFolder != this->notesPath )
            {
                moveDestinationMenu->addAction( noteFolder );
                copyDestinationMenu->addAction( noteFolder );
            }
        }
    }

    QAction* removeAction = noteMenu.addAction( "&Remove notes" );
    noteMenu.addSeparator();
    QAction* selectAllAction = noteMenu.addAction( "Select &all notes" );

    QAction* selectedItem = noteMenu.exec( globalPos );
    if ( selectedItem )
    {
        // move notes
        if ( selectedItem->parent() == moveDestinationMenu )
        {
            QString destinationFolder = selectedItem->text();
            moveSelectedNotesToFolder( destinationFolder );
        }
        // copy notes
        else if ( selectedItem->parent() == copyDestinationMenu )
        {
            QString destinationFolder = selectedItem->text();
            copySelectedNotesToFolder( destinationFolder );
        }
        // remove notes
        else if ( selectedItem == removeAction )
        {
            removeSelectedNotes();
        }
        // select all notes
        else if ( selectedItem == selectAllAction )
        {
            selectAllNotes();
        }
    }
}

void MainWindow::on_actionSelect_all_notes_triggered()
{
    selectAllNotes();
}

/**
 * @brief create the additional menu entries for the note text edit field
 * @param pos
 */
void MainWindow::on_noteTextEdit_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = ui->notesListWidget->mapToGlobal( pos );
    QMenu *menu = ui->noteTextEdit->createStandardContextMenu();

    menu->addSeparator();

    QString linkTextActionName = ui->noteTextEdit->textCursor().selectedText() != "" ? "&Link selected text to note" : "Insert &link note";
    QAction* linkTextAction = menu->addAction( linkTextActionName );
    linkTextAction->setShortcut( tr( "Ctrl+L" ) );

    QAction* selectedItem = menu->exec( globalPos );
    if ( selectedItem )
    {
        if ( selectedItem == linkTextAction )
        {
            // handle the linking of text with a note
            handleTextNoteLinking();
        }
    }
}

void MainWindow::on_actionInsert_Link_to_note_triggered()
{
    // handle the linking of text with a note
    handleTextNoteLinking();
}

void MainWindow::on_action_Find_text_in_note_triggered()
{
    ui->searchInNoteEdit->setFocus();
    ui->searchInNoteEdit->selectAll();
    ui->searchInNoteWidget->show();

    // if we are not in edit mode go into edit mode
    if ( ui->noteTabWidget->currentIndex() != 0 )
    {
        this->setNoteTextEditMode( true );
    }

    doSearchInNote();
}

void MainWindow::on_searchInNoteCloseButton_clicked()
{
    closeSearchInNoteWidget();
}

void MainWindow::on_searchInNoteDownButton_clicked()
{
    doSearchInNote();
}

void MainWindow::on_searchInNoteEdit_textChanged(const QString &arg1)
{
    Q_UNUSED( arg1 );
    doSearchInNote();
}

void MainWindow::on_searchInNoteUpButton_clicked()
{
    doSearchInNote( false );
}
