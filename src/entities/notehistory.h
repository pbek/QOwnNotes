#ifndef NOTEHISTORY_H
#define NOTEHISTORY_H

#include <QTextEdit>
#include <QDataStream>
#include "note.h"


class NoteHistoryItem
{
public:
    explicit NoteHistoryItem(Note *note = NULL, QTextEdit *textEdit = NULL);
    explicit NoteHistoryItem(QString noteName, int cursorPosition,
                             float relativeScrollBarPosition);
    friend QDebug operator<<(QDebug dbg, const NoteHistoryItem &item);
    QString getNoteName() const;
    int getCursorPosition() const;
    float getRelativeScrollBarPosition() const;
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

// we want to store the class to the settings
QDataStream &operator<<(QDataStream &out, const NoteHistoryItem &item);
QDataStream &operator>>(QDataStream &in, NoteHistoryItem &item);
Q_DECLARE_METATYPE(NoteHistoryItem)

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
