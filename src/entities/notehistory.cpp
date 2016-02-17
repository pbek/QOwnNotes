#include "notehistory.h"
#include <QDebug>

/*
 * NoteHistoryItem implementation
 */

NoteHistoryItem::NoteHistoryItem(Note *note, int cursorPosition) {
    if (note != NULL) {
        this->noteName = note->getName();
    }
    this->cursorPosition = cursorPosition;
}

QString NoteHistoryItem::getNoteName() const {
    return noteName;
}

Note NoteHistoryItem::getNote() {
    return Note::fetchByName(noteName);
}

int NoteHistoryItem::getCursorPosition() const {
    return cursorPosition;
}

bool NoteHistoryItem::isNoteValid() {
    Note note = getNote();
    return note.exists();
}

bool NoteHistoryItem::operator==(const NoteHistoryItem &item) const {
    return this->noteName == item.getNoteName();
}

void NoteHistoryItem::setCursorPosition(int value) {
    cursorPosition = value;
}


QDebug operator<<(QDebug dbg, const NoteHistoryItem &item) {
    dbg.nospace() << "NoteHistoryItem: <noteName>" << item.noteName <<
    " <cursorPosition>" << item.cursorPosition;
    return dbg.space();
}


/*
 * NoteHistory implementation
 */

NoteHistory::NoteHistory() {
    noteHistory = new QList<NoteHistoryItem>;
    currentIndex = 0;
}

void NoteHistory::add(Note note, int cursorPosition) {
    if (!note.exists()) {
        return;
    }

    NoteHistoryItem item(&note, cursorPosition);

    if (noteHistory->contains(item)) {
        // decrease current index if we are going to remove an item before it
        if (noteHistory->indexOf(item) < currentIndex) {
            currentIndex--;
        }

        // remove found item
        noteHistory->removeAll(item);
    }

    noteHistory->prepend(item);

    if (currentIndex < lastIndex()) {
        // increase current index
        currentIndex++;
    }

    noteHistory->move(0, currentIndex);
    currentIndex = noteHistory->indexOf(item);

    qDebug() << " added to history: " << item;
}

void NoteHistory::updateCursorPositionOfNote(Note note, int cursorPosition) {
    if (isEmpty()) {
        return;
    }

    NoteHistoryItem item(&note, cursorPosition);

    // create history entry if it does not exist (for renamed notes)
    if (!noteHistory->contains(item)) {
        add(note, cursorPosition);
    }

    int position = noteHistory->indexOf(item);
    qDebug() << "updated item: " << item;
    noteHistory->replace(position, item);
}

bool NoteHistory::back() {
    if ((currentIndex < 0) || (currentIndex > lastIndex()) || isEmpty()) {
        return false;
    } else if (currentIndex == 0) {
        currentIndex = lastIndex();
    } else {
        currentIndex--;
    }

    currentHistoryItem = noteHistory->at(currentIndex);

    // check if note still exists, remove item if not
    if (!currentHistoryItem.isNoteValid()) {
        noteHistory->removeAll(currentHistoryItem);
        return back();
    }

    return true;
}

bool NoteHistory::forward() {
    if ((currentIndex < 0) || (currentIndex > lastIndex()) || isEmpty()) {
        return false;
    } else if (currentIndex == lastIndex()) {
        currentIndex = 0;
    } else {
        currentIndex++;
    }

    currentHistoryItem = noteHistory->at(currentIndex);

    // check if note still exists, remove item if not
    if (!currentHistoryItem.isNoteValid()) {
        noteHistory->removeAll(currentHistoryItem);
        return forward();
    }

    return true;
}

int NoteHistory::lastIndex() {
    return noteHistory->size() - 1;
}

NoteHistoryItem NoteHistory::getCurrentHistoryItem() {
    return currentHistoryItem;
}

bool NoteHistory::isEmpty() {
    return noteHistory->size() == 0;
}

/**
 * @brief Clears the note history
 */
void NoteHistory::clear() {
    noteHistory->clear();
    currentIndex = 0;
}

QDebug operator<<(QDebug dbg, const NoteHistory &history) {
    dbg.nospace() << "NoteHistory: <index>" << history.currentIndex <<
    " <noteHistorySize>" << history.noteHistory->size();
    return dbg.space();
}
