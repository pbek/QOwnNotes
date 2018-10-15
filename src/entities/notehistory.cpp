#include "notehistory.h"
#include <QScrollBar>
#include <QDebug>

/*
 * NoteHistoryItem implementation
 */

NoteHistoryItem::NoteHistoryItem(Note *note, QPlainTextEdit *textEdit) {
    _noteName = "";
    _noteSubFolderPathData = "";
    _cursorPosition = 0;
    _relativeScrollBarPosition = 0;

    if (note != NULL) {
        _noteName = note->getName();
        _noteSubFolderPathData = note->noteSubFolderPathData();
    }

    if (textEdit != NULL) {
        _cursorPosition = textEdit->textCursor().position();
        _relativeScrollBarPosition = getTextEditScrollBarRelativePosition(
                textEdit);
    }
}

NoteHistoryItem::NoteHistoryItem(QString noteName,
                                 QString noteSubFolderPathData,
                                 int cursorPosition,
                                 float relativeScrollBarPosition) {
    _noteName = noteName;
    _noteSubFolderPathData = noteSubFolderPathData;
    _cursorPosition = cursorPosition;
    _relativeScrollBarPosition = relativeScrollBarPosition;
}

/**
 * Returns the relative note text edit scrollbar position (0..1)
 */
float NoteHistoryItem::getTextEditScrollBarRelativePosition(
        QPlainTextEdit *textEdit) {
    QScrollBar *scrollBar = textEdit->verticalScrollBar();
    int max = scrollBar->maximum();

    return max > 0 ?
           static_cast<float>(scrollBar->sliderPosition()) / max : 0;
}

QString NoteHistoryItem::getNoteName() const {
    return _noteName;
}

QString NoteHistoryItem::getNoteSubFolderPathData() const {
    return _noteSubFolderPathData;
}

Note NoteHistoryItem::getNote() {
    NoteSubFolder noteSubFolder = NoteSubFolder::fetchByPathData(
            _noteSubFolderPathData);
    return Note::fetchByName(_noteName, noteSubFolder.getId());
}

int NoteHistoryItem::getCursorPosition() const {
    return _cursorPosition;
}

float NoteHistoryItem::getRelativeScrollBarPosition() const {
    return _relativeScrollBarPosition;
}

/**
 * Restores the position in a text edit
 *
 * @param textEdit
 */
void NoteHistoryItem::restoreTextEditPosition(QPlainTextEdit *textEdit) {
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
    return _noteName == item.getNoteName() &&
            _noteSubFolderPathData == item.getNoteSubFolderPathData();
}

QDebug operator<<(QDebug dbg, const NoteHistoryItem &item) {
    dbg.nospace() << "NoteHistoryItem: <noteName>" << item._noteName <<
    " <noteSubFolderPathData>" << item._noteSubFolderPathData <<
    " <cursorPosition>" << item._cursorPosition <<
    " <relativeScrollBarPosition>" << item._relativeScrollBarPosition;
    return dbg.space();
}

/**
 * Stream operator for storing the class to QSettings
 *
 * @param out
 * @param item
 * @return
 */
QDataStream &operator<<(QDataStream &out, const NoteHistoryItem &item) {
    out << item.getNoteName() << item.getNoteSubFolderPathData()
        << item.getCursorPosition() << item.getRelativeScrollBarPosition();

    return out;
}

/**
 * Stream operator for loading the class from QSettings
 *
 * @param in
 * @param item
 * @return
 */
QDataStream &operator>>(QDataStream &in, NoteHistoryItem &item) {
    QString noteName;
    QString noteSubFolderPathData;
    int cursorPosition;
    float relativeScrollBarPosition;

    in >> noteName >> noteSubFolderPathData >> cursorPosition >>
       relativeScrollBarPosition;
    item = NoteHistoryItem(noteName, noteSubFolderPathData, cursorPosition,
            relativeScrollBarPosition);

    return in;
}


/*
 * NoteHistory implementation
 */

NoteHistory::NoteHistory() {
    noteHistory = new QList<NoteHistoryItem>;
    currentIndex = 0;
}

void NoteHistory::add(Note note, QPlainTextEdit *textEdit) {
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

void NoteHistory::updateCursorPositionOfNote(Note note, QPlainTextEdit *textEdit) {
    if (isEmpty()) {
        return;
    }

    NoteHistoryItem item(&note, textEdit);

    // create history entry if it does not exist (for renamed notes)
    if (!noteHistory->contains(item)) {
        add(note, textEdit);
    }

    int position = noteHistory->indexOf(item);

    // check if we really found the item
    if (position > -1) {
        noteHistory->replace(position, item);
    }
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

/**
 * Returns a list of NoteHistoryItems of the note history
 *
 * @return
 */
QList<NoteHistoryItem> NoteHistory::noteHistoryItems() const {
    QList<NoteHistoryItem> items;

    for (int i = 0; i < noteHistory->count(); i++) {
        NoteHistoryItem item = noteHistory->at(i);

        items.append(item);
    }

    return items;
}

/**
 * Appends a NoteHistoryItem to the note history
 *
 * @param item
 */
void NoteHistory::addNoteHistoryItem(NoteHistoryItem item) {
    noteHistory->append(item);
}

/**
 * Stream operator for storing the class to QSettings
 * Note: This method doesn't seem to get called when serializing for
 *       writing to the settings
 *
 * @param out
 * @param history
 * @return
 */
QDataStream &operator<<(QDataStream &out, const NoteHistory &history) {
    Q_FOREACH(NoteHistoryItem item, history.noteHistoryItems()) {
        out << item;
    }

    return out;
}

/**
 * Stream operator for loading the class from QSettings
 *
 * @param in
 * @param history
 * @return
 */
QDataStream &operator>>(QDataStream &in, NoteHistory &history) {
    NoteHistoryItem item;

    while (!in.atEnd()) {
        in >> item;
        history.addNoteHistoryItem(item);
    }

    return in;
}
