#include "notehistory.h"
#include <QDebug>

/*
 * NoteHistoryItem implementation
 */

NoteHistoryItem::NoteHistoryItem( Note *note, int cursorPosition )
{
    if ( note != NULL )
    {
        this->noteName = note->getName();
    }
    this->cursorPosition = cursorPosition;
}

QString NoteHistoryItem::getNoteName() const
{
    return noteName;
}

Note NoteHistoryItem::getNote()
{
    return Note::fetchByName( noteName );
}

int NoteHistoryItem::getCursorPosition() const
{
    return cursorPosition;
}

bool NoteHistoryItem::isNoteValid()
{
    Note note = getNote();
    return note.exists();
}

bool NoteHistoryItem::operator==(const NoteHistoryItem & item) const
{
    return ( ( this->cursorPosition == item.getCursorPosition() ) && ( this->noteName == item.getNoteName() ) );
}
void NoteHistoryItem::setCursorPosition(int value)
{
    cursorPosition = value;
}


QDebug operator<<(QDebug dbg, const NoteHistoryItem &item)
{
    dbg.nospace() << "NoteHistoryItem: <noteName>" << item.noteName << " <cursorPosition>" << item.cursorPosition;
    return dbg.space();
}


/*
 * NoteHistory implementation
 */

NoteHistory::NoteHistory()
{
    noteHistory = new QList<NoteHistoryItem>;
    currentIndex = 0;
}

void NoteHistory::setCurrentIndex( int index )
{
    previousIndex = currentIndex;
    currentIndex = index;
}

void NoteHistory::add( Note note, int cursorPosition )
{
    if ( !note.exists() )
    {
        return;
    }

    // if the last history item holds the current note remove it to let it get updated
    if ( !isEmpty() )
    {
        NoteHistoryItem item = noteHistory->last();

        if ( note.getName() == item.getNoteName() )
        {
            noteHistory->removeOne( item );
        }
    }

    NoteHistoryItem item = NoteHistoryItem( &note, cursorPosition );
    qDebug()<<" added to history: " << item;

    noteHistory->append( item );
    setCurrentIndex( lastIndex() );
}

void NoteHistory::updateCursorPositionForPreviousItem( int cursorPosition )
{
    if ( noteHistory->size() < 2 )
    {
        return;
    }

    NoteHistoryItem item = noteHistory->at( previousIndex );
    qDebug()<<"item: " << item;
    item.setCursorPosition( cursorPosition );
    qDebug()<<"updated item: " << item;
    noteHistory->replace( previousIndex, item );
}

bool NoteHistory::back()
{
    if ( ( currentIndex <= 0 ) || ( currentIndex > lastIndex() ) || isEmpty() )
    {
        return false;
    }

    if ( currentIndex > 0 )
    {
        setCurrentIndex( currentIndex - 1 );
    }

    currentHistoryItem = noteHistory->at( currentIndex );

    // check if note still exists, remove item if not
    if ( !currentHistoryItem.isNoteValid() )
    {
        noteHistory->removeOne( currentHistoryItem );
        return back();
    }

    return true;
}

bool NoteHistory::forward()
{
    if ( ( currentIndex < 0 ) || ( currentIndex >= lastIndex() ) || isEmpty() )
    {
        return false;
    }

    if ( currentIndex < ( lastIndex() ) )
    {
        setCurrentIndex( currentIndex + 1 );
    }

    currentHistoryItem = noteHistory->at( currentIndex );

    // check if note still exists, remove item if not
    if ( !currentHistoryItem.isNoteValid() )
    {
        noteHistory->removeOne( currentHistoryItem );
        return forward();
    }

    return true;
}

int NoteHistory::lastIndex()
{
    return noteHistory->size() - 1;
}

NoteHistoryItem NoteHistory::getCurrentHistoryItem()
{
    return currentHistoryItem;
}

bool NoteHistory::isEmpty()
{
    return noteHistory->size() == 0;
}

QDebug operator<<(QDebug dbg, const NoteHistory &history)
{
    dbg.nospace() << "NoteHistory: <index>" << history.currentIndex << " <noteHistorySize>" << history.noteHistory->size();
    return dbg.space();
}
