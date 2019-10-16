#ifndef TESTNOTES_H
#define TESTNOTES_H

//#include "basetestcase.h"
//#include "models/key-models/keyfactory.h"
#include <QObject>
#include "entities/note.h"

class TestNotes : public QObject
{
    Q_OBJECT

//public:
//    TestNotes();

private:
    QString notesPath;
    QString noteFile;
    QString noteName;
    QString noteFileName;

//private slots:
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testNoteCreating();
    void testNoteEncryption();
    void testNoteDecryption();
    void testNoteDecryptionFail();
};

#endif // TESTNOTES_H
