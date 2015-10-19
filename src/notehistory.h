#ifndef NOTEHISTORY_H
#define NOTEHISTORY_H

#include "note.h"


class NoteHistoryItem
{
public:
    explicit NoteHistoryItem( Note *note = NULL, int cursorPosition = 0 );
    friend QDebug operator<<(QDebug dbg, const NoteHistoryItem &item);
    QString getNoteName() const;
    int getCursorPosition() const;
    Note getNote();
    bool isNoteValid();
    bool operator ==(const NoteHistoryItem &item) const;

private:
    QString noteName;
    int cursorPosition;
};

class NoteHistory
{
private:
    QList<NoteHistoryItem> *noteHistory;
    int currentIndex;
    NoteHistoryItem currentHistoryItem;

    int lastIndex();

public:
    explicit NoteHistory();
    void add( Note note, int cursorPosition = 0 );
    friend QDebug operator<<(QDebug dbg, const NoteHistory &history);
    bool back();
    bool forward();
    bool isEmpty();
    NoteHistoryItem getCurrentHistoryItem();
};

#endif // NOTEHISTORY_H
