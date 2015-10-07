#include "notesearchdialog.h"
#include "ui_notesearchdialog.h"
#include <QKeyEvent>
#include <QDebug>
#include "note.h"

NoteSearchDialog::NoteSearchDialog(QString dialogTitle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NoteSearchDialog)
{
    ui->setupUi(this);
    this->firstVisibleNoteListRow = 0;

    if ( dialogTitle != "" )
    {
        this->setWindowTitle( dialogTitle );
    }

    QStringList nameList = Note::fetchNoteNames();
    ui->searchLineEdit->installEventFilter(this);
    ui->notesListWidget->addItems( nameList );
    ui->notesListWidget->setCurrentRow( 0 );
}

NoteSearchDialog::~NoteSearchDialog()
{
    delete ui;
}

void NoteSearchDialog::on_searchLineEdit_textChanged(const QString &arg1)
{
    // search notes when at least 2 characters were entered
    if ( arg1.count() >= 2 )
    {
        QList<QString> noteNameList = Note::searchAsNameList( arg1, true );
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

QString NoteSearchDialog::getSelectedNoteName()
{
    return ui->notesListWidget->currentRow() > -1 ? ui->notesListWidget->currentItem()->text() : "";
}

//
// Event filters on the NoteSearchDialog
//
bool NoteSearchDialog::eventFilter(QObject* obj, QEvent *event)
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
                //focusNoteTextEdit();
                return true;
            }
        }
        return false;
    }

    return NoteSearchDialog::eventFilter(obj, event);
}


/*
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
*/

void NoteSearchDialog::on_notesListWidget_doubleClicked(const QModelIndex &index)
{
    this->close();
    this->setResult( QDialog::Accepted );
}
