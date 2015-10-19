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

void NoteHistory::add( Note note, int cursorPosition )
{
    // don't add the item if the note in the last item is the same as the note that should be added
    if ( ( !isEmpty() ) && ( note.getName() == noteHistory->last().getNoteName() ) )
    {
        return;
    }

    NoteHistoryItem item = NoteHistoryItem( &note, cursorPosition );
    noteHistory->append( item );
    currentIndex = noteHistory->size() - 1;
}

bool NoteHistory::back()
{
    if ( ( currentIndex < 0 ) || ( currentIndex > lastIndex() ) || isEmpty() )
    {
        return false;
    }

    if ( currentIndex > 0 )
    {
        currentIndex--;
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
    if ( ( currentIndex < 0 ) || ( currentIndex > lastIndex() ) || isEmpty() )
    {
        return false;
    }

    if ( currentIndex < ( lastIndex() ) )
    {
        currentIndex++;
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
