#include "notehistory.h"
#include <QScrollBar>
#include <QDebug>

/*
 * NoteHistoryItem implementation
 */

NoteHistoryItem::NoteHistoryItem(Note *note, QTextEdit *textEdit) {
    if (note != NULL) {
        _noteName = note->getName();
    }

    if (textEdit != NULL) {
        _cursorPosition = textEdit->textCursor().position();
        _relativeScrollBarPosition = getTextEditScrollBarRelativePosition(
                textEdit);
    }
}

/**
 * Returns the relative note text edit scrollbar position (0..1)
 */
float NoteHistoryItem::getTextEditScrollBarRelativePosition(
        QTextEdit *textEdit) {
    QScrollBar *scrollBar = textEdit->verticalScrollBar();
    int max = scrollBar->maximum();

    return max > 0 ?
           static_cast<float>(scrollBar->sliderPosition()) / max : 0;
}

QString NoteHistoryItem::getNoteName() const {
    return _noteName;
}

Note NoteHistoryItem::getNote() {
    return Note::fetchByName(_noteName);
}

int NoteHistoryItem::getCursorPosition() const {
    return _cursorPosition;
}

/**
 * Restores the position in a text edit
 *
 * @param textEdit
 */
void NoteHistoryItem::restoreTextEditPosition(QTextEdit *textEdit) {
    // set the cursor position
    QTextCursor c = textEdit->textCursor();
    c.setPosition(_cursorPosition);
    textEdit->setTextCursor(c);

    // set the scroll bar position
    QScrollBar *scrollBar = textEdit->verticalScrollBar();
    scrollBar->setSliderPosition(static_cast<int>(
                            scrollBar->maximum() *_relativeScrollBarPosition));
}

bool NoteHistoryItem::isNoteValid() {
    Note note = getNote();
    return note.exists();
}

bool NoteHistoryItem::operator==(const NoteHistoryItem &item) const {
    return this->_noteName == item.getNoteName();
}

QDebug operator<<(QDebug dbg, const NoteHistoryItem &item) {
    dbg.nospace() << "NoteHistoryItem: <noteName>" << item._noteName <<
    " <cursorPosition>" << item._cursorPosition <<
    " <relativeScrollBarPosition>" << item._relativeScrollBarPosition;
    return dbg.space();
}


/*
 * NoteHistory implementation
 */

NoteHistory::NoteHistory() {
    noteHistory = new QList<NoteHistoryItem>;
    currentIndex = 0;
}

void NoteHistory::add(Note note, QTextEdit *textEdit) {
    if (!note.exists()) {
        return;
    }

    NoteHistoryItem item(&note, textEdit);

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

void NoteHistory::updateCursorPositionOfNote(Note note, QTextEdit *textEdit) {
    if (isEmpty()) {
        return;
    }

    NoteHistoryItem item(&note, textEdit);

    // create history entry if it does not exist (for renamed notes)
    if (!noteHistory->contains(item)) {
        add(note, textEdit);
    }

    int position = noteHistory->indexOf(item);
    qDebug() << "updated item: " << item;
    noteHistory->replace(position, item);
}

/**
 * Gets the last history item of a note in the note history
 *
 * @param note
 * @return
 */
NoteHistoryItem NoteHistory::getLastItemOfNote(Note note) {
    if (!isEmpty()) {
        for (int i = 0; i < noteHistory->count(); i++) {
            NoteHistoryItem item = noteHistory->at(i);

            if (item.getNote().getId() == note.getId()) {
                return item;
            }
        }
    }

    return NoteHistoryItem();
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
