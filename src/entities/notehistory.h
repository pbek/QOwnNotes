#ifndef NOTEHISTORY_H
#define NOTEHISTORY_H

#include <QTextEdit>
#include "note.h"


class NoteHistoryItem
{
public:
    explicit NoteHistoryItem(Note *note = NULL, QTextEdit *textEdit = NULL);
    friend QDebug operator<<(QDebug dbg, const NoteHistoryItem &item);
    QString getNoteName() const;
    int getCursorPosition() const;
    Note getNote();
    bool isNoteValid();
    bool operator ==(const NoteHistoryItem &item) const;
    void restoreTextEditPosition(QTextEdit *textEdit);

private:
    QString _noteName;
    int _cursorPosition;
    float _relativeScrollBarPosition;
    static float getTextEditScrollBarRelativePosition(QTextEdit *textEdit);
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
    void add(Note note, QTextEdit *textEdit);
    friend QDebug operator<<(QDebug dbg, const NoteHistory &history);
    bool back();
    bool forward();
    bool isEmpty();
    NoteHistoryItem getCurrentHistoryItem();
    void updateCursorPositionOfNote(Note note, QTextEdit *textEdit);
    void clear();

    NoteHistoryItem getLastItemOfNote(Note note);
};

#endif // NOTEHISTORY_H
