#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include "test_notes.h"
#include "services/databaseservice.h"
#include "helpers/codetohtmlconverter.h"


//TestNotes::TestNotes()
//{
//}

void TestNotes::initTestCase()
{
    DatabaseService::createConnection();
    DatabaseService::setupTables();

    // generate a notes path
    notesPath = QDir::tempPath() + QDir::separator() + QStringLiteral("qownnotes_test_") + QString::number( qrand() );
    //qDebug() << "generated notesPath:" << notesPath;

    // create temporary notes directory
    QDir dir;
    dir.mkpath( notesPath );

    // store notes path for notes
    QSettings settings;
    settings.setValue( QStringLiteral("notesPath"), notesPath );

    // create a note file
    noteName = QStringLiteral("MyTestNote");
    noteFile = noteName + QStringLiteral(".md");
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
    note.createFromFile(file);

    QVERIFY(note.getId() == 1);
    QVERIFY(note.getName() == noteName);
    QVERIFY(note.getNoteText().startsWith(noteName));
}

void TestNotes::testNoteEncryption()
{
    QFile file(noteFileName);
    Note note;
    note.createFromFile(file);

    note.setCryptoPassword(QStringLiteral("test"));
    note.encryptNoteText();

    QVERIFY(note.getId() == 2);
    QVERIFY(note.getName() == noteName );
    QVERIFY(note.getNoteText() == QStringLiteral("MyTestNote\n============\n\n<!-- BEGIN ENCRYPTED TEXT --\nVTVdShbeNi63fYLB7B56pg==\n-- END ENCRYPTED TEXT -->"));
}

void TestNotes::testNoteDecryption()
{
    auto note = Note::fetch(2);
    note.setCryptoPassword("test");

    QVERIFY( note.getId() == 2 );
    QVERIFY( note.getDecryptedNoteText() == QStringLiteral("MyTestNote\n============\n\nSome text") );
}

void TestNotes::testNoteDecryptionFail()
{
    auto note = Note::fetch(2);
    note.setCryptoPassword("test2");

    QVERIFY(note.getId() == 2);
    QVERIFY(note.getDecryptedNoteText() != QStringLiteral("MyTestNote\n============\n\nSome text"));
}

void TestNotes::testNoteToMarkdownHtml()
{
    QFile file( noteFileName );
    Note note;
    note.createFromFile(file);

    QString html = note.toMarkdownHtml("", 980, true);
    QString expectedBody = QStringLiteral("<h1>MyTestNote</h1>\n<p>Some text</p>");

    QVERIFY(html.contains(expectedBody));
}

void TestNotes::testCodeToHtmlConversionPython()
{
    QString pythonCode = "import hello\n"
            "if this()\n"
                "\tthat\n"
            "# this is a comment\n";

    CodeToHtmlConverter c(pythonCode.midRef(0, pythonCode.length()), "python");
    QString outputPython = c.process();

    QString expectedOutputPython = QStringLiteral("<span class=\"code-other\">import</span> hello\n"
                             "<span class=\"code-keyword\">if</span> this()\n"
                             "\tthat\n"
                             "<span class=\"code-comment\"># this is a comment</span>\n");

    QVERIFY(outputPython == expectedOutputPython);
}

void TestNotes::testCodeToHtmlConversionHashComment()
{
    QString hashStyleComment = QStringLiteral("#hello my qownnotes blah blah\n");
    CodeToHtmlConverter c2(hashStyleComment.midRef(0, hashStyleComment.length()), QStringLiteral("bash"));
    QString outputHashStyleComment = c2.process();
    QString expectedOutputHashStyleComment = QStringLiteral("<span class=\"code-comment\">#hello my qownnotes blah blah</span>\n");

    QVERIFY(outputHashStyleComment == expectedOutputHashStyleComment);
}

void TestNotes::testCodeToHtmlConversionSingleLineComment()
{
    QString comment = QStringLiteral("//hello my qownnotes blah blah single line\n");
    QString comment2 = QStringLiteral("//hello my qownnotes blah blah single line\n");
    CodeToHtmlConverter c1(comment.midRef(0, comment.length()), QStringLiteral("cpp"));
    CodeToHtmlConverter c2(comment2.midRef(0, comment2.length()), QStringLiteral("py"));
    QString outputSingleLineComment = c1.process();
    QString outputSingleLineComment2 = c2.process();
    QString expectedOutputSingleLineComment = QStringLiteral("<span class=\"code-comment\">&#47;&#47;hello my qownnotes blah blah single line</span>\n");

    QVERIFY(outputSingleLineComment == expectedOutputSingleLineComment);
    QVERIFY(outputSingleLineComment2 != expectedOutputSingleLineComment);
}

void TestNotes::testCodeToHtmlConversionMultiLineComment()
{
    QString comment = QStringLiteral("/*hello my qownnotes blah blah single line*/\n");
    QString commentTrueMultiLine = QStringLiteral("/*hello my \nqownnotes blah \nblah single line*/\n");
    QString commentTrueMultiLineNoEnd = QStringLiteral("/*hello my \nqownnotes blah \nblah single line\n");

    CodeToHtmlConverter c1(comment.midRef(0, comment.length()), QStringLiteral("cpp"));
    CodeToHtmlConverter c2(commentTrueMultiLine.midRef(0, commentTrueMultiLine.length()), QStringLiteral("cpp"));
    CodeToHtmlConverter c3(commentTrueMultiLineNoEnd.midRef(0, commentTrueMultiLineNoEnd.length()), QStringLiteral("cpp"));

    QString outputMultiLineComment = c1.process();
    QString outputTrueMultiLine = c2.process();
    QString outputTrueMultiLineNoEnd = c3.process();

    QString expectedMultiLineComment = QStringLiteral("<span class=\"code-comment\">&#47;*hello my qownnotes blah blah single line*&#47;</span>\n");
    QString expectedTrueMultiLine = QStringLiteral("<span class=\"code-comment\">&#47;*hello my \nqownnotes blah \nblah single line*&#47;</span>\n");
    QString expectedTrueMultiLineNoEnd = QStringLiteral("<span class=\"code-comment\">&#47;*hello my \nqownnotes blah \nblah single line</span>\n");

    QVERIFY(outputMultiLineComment == expectedMultiLineComment);
    QVERIFY(outputTrueMultiLine == expectedTrueMultiLine);
    QVERIFY(outputTrueMultiLineNoEnd == expectedTrueMultiLineNoEnd);
}

void TestNotes::testCodeToHtmlNumericLiterals()
{
    QString a = "123";
    QString a1 = "123+321";
    QString a2 = "(123)";
    QString a3 = "[123]";
    QString bad = "a123";
    QString bad1 = "123a";
    QString bad2 = "12a3";
    QString bad3 = "a123b";
    QString bad4 = "12pxa";
    QString a5 = "0x123";
    QString a6 = "12px\n";
    QString a7 = "12em\n";

    CodeToHtmlConverter c1(a.midRef(0, a.length()), QStringLiteral("c"));
    CodeToHtmlConverter c2(a1.midRef(0, a1.length()), QStringLiteral("c"));
    CodeToHtmlConverter c3(a2.midRef(0, a2.length()), QStringLiteral("c"));
    CodeToHtmlConverter c4(a3.midRef(0, a3.length()), QStringLiteral("c"));

    CodeToHtmlConverter c5(bad.midRef(0, bad.length()), QStringLiteral("c"));
    CodeToHtmlConverter c6(bad1.midRef(0, bad1.length()), QStringLiteral("c"));
    CodeToHtmlConverter c7(bad2.midRef(0, bad2.length()), QStringLiteral("c"));
    CodeToHtmlConverter c8(bad3.midRef(0, bad3.length()), QStringLiteral("c"));
    CodeToHtmlConverter c13(bad4.midRef(0, bad4.length()), QStringLiteral("css"));

    CodeToHtmlConverter c9(a5.midRef(0, a5.length()), QStringLiteral("c"));
    CodeToHtmlConverter c10(a6.midRef(0, a6.length()), QStringLiteral("css"));
    CodeToHtmlConverter c11(a7.midRef(0, a7.length()), QStringLiteral("css"));


    QString ar = c1.process();
    QString a1r = c2.process();
    QString a2r = c3.process();
    QString a3r = c4.process();
    QString badResult = c5.process();
    QString badResult1 = c6.process();
    QString badResult2 = c7.process();
    QString badResult3 = c8.process();
    QString badResult4 = c13.process();
    QString a5r = c9.process();
    QString a6r = c10.process();
    QString a7r = c11.process();

    QString e = "<span class=\"code-literal\">123</span>";
    QString e1 = "<span class=\"code-literal\">123</span>+<span class=\"code-literal\">321</span>";
    QString e2 = "(<span class=\"code-literal\">123</span>)";
    QString e3 = "[<span class=\"code-literal\">123</span>]";
    QString expectedBad = "a123";
    QString expectedBad1 = "123a";
    QString expectedBad2 = "12a3";
    QString expectedBad3 = "a123b";
    QString expectedBad4 = "12pxa";
    QString e5 = "<span class=\"code-literal\">0x123</span>";
    QString e6 = "<span class=\"code-literal\">12</span><span class=\"code-keyword\">px</span>\n";
    QString e7 = "<span class=\"code-literal\">12</span><span class=\"code-keyword\">em</span>\n";

    QVERIFY(ar == e);
    QVERIFY(a1r == e1);
    QVERIFY(a2r == e2);
    QVERIFY(a3r == e3);
    QVERIFY(badResult == expectedBad);
    QVERIFY(badResult1 == expectedBad1);
    QVERIFY(badResult2 == expectedBad2);
    QVERIFY(badResult3 == expectedBad3);
    QVERIFY(badResult4 == expectedBad4);
    QVERIFY(a5r == e5);
    QVERIFY(a6r == e6);
    QVERIFY(a7r == e7);
}

//QTEST_MAIN(TestNotes)
