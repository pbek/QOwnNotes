#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include "test_notes.h"
#include "services/databaseservice.h"


//TestNotes::TestNotes()
//{
//}

void TestNotes::initTestCase()
{
    DatabaseService::createConnection();
    DatabaseService::setupTables();

    // generate a notes path
    notesPath = QDir::tempPath() + QDir::separator() + "qownnotes_test_" + QString::number( qrand() );
//    qDebug() << "generated notesPath:" << notesPath;

    // create temporary notes directory
    QDir dir;
    dir.mkpath( notesPath );

    // store notes path for notes
    QSettings settings;
    settings.setValue( "notesPath", notesPath );

    // create a note file
    noteName = "MyTestNote";
    noteFile = noteName + ".md";
    noteFileName = notesPath + QDir::separator() + noteFile;

    QFile file( noteFileName );
    if ( file.open( QIODevice::ReadWrite ) )
    {
        QTextStream stream( &file );
        stream << noteName << endl << "============" << endl << endl << "Some text";
        file.close();
    }
}

void TestNotes::cleanupTestCase()
{
    QDir dir( notesPath );

    // remove temporary notes directory
    if ( notesPath.startsWith( QDir::tempPath() ) && dir.exists( notesPath ) )
    {
        Q_FOREACH(QFileInfo info, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files, QDir::DirsFirst ) )
        {
            if (info.isFile())
            {
                QFile::remove(info.absoluteFilePath());
            }
        }

        dir.rmdir( notesPath );
    }
}

void TestNotes::testNoteCreating()
{
    QFile file( noteFileName );
    Note note;
    note.createFromFile( file );

    QVERIFY( note.getId() == 1 );
    QVERIFY( note.getName() == noteName );
    QVERIFY( note.getNoteText().startsWith( noteName ) );
}

void TestNotes::testNoteEncryption()
{
    QFile file( noteFileName );
    Note note;
    note.createFromFile( file );

    note.setCryptoPassword("test");
    note.encryptNoteText();

    QVERIFY( note.getId() == 2 );
    QVERIFY( note.getName() == noteName );
    QVERIFY( note.getNoteText() == "MyTestNote\n============\n\n<!-- BEGIN ENCRYPTED TEXT --\nVTVdShbeNi63fYLB7B56pg==\n-- END ENCRYPTED TEXT -->" );
}

void TestNotes::testNoteDecryption()
{
    auto note = Note::fetch(2);
    note.setCryptoPassword("test");

    QVERIFY( note.getId() == 2 );
    QVERIFY( note.getDecryptedNoteText() == "MyTestNote\n============\n\nSome text" );
}

void TestNotes::testNoteDecryptionFail()
{
    auto note = Note::fetch(2);
    note.setCryptoPassword("test2");

    QVERIFY( note.getId() == 2 );
    QVERIFY( note.getDecryptedNoteText() != "MyTestNote\n============\n\nSome text" );
}

//QTEST_MAIN(TestNotes)
