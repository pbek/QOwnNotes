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

    note.setCryptoPassword(QStringLiteral("test"));
    note.encryptNoteText();

    QVERIFY( note.getId() == 2 );
    QVERIFY( note.getName() == noteName );
    QVERIFY( note.getNoteText() == QStringLiteral("MyTestNote\n============\n\n<!-- BEGIN ENCRYPTED TEXT --\nVTVdShbeNi63fYLB7B56pg==\n-- END ENCRYPTED TEXT -->" ));
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

    QVERIFY( note.getId() == 2 );
    QVERIFY( note.getDecryptedNoteText() != QStringLiteral("MyTestNote\n============\n\nSome text") );
}

void TestNotes::testNoteToMarkdownHtml()
{
    QFile file( noteFileName );
    Note note;
    note.createFromFile( file );

    QString html = note.toMarkdownHtml("", 980, true);
    QString expectedHtml = QStringLiteral("<html><head><meta charset=\"utf-8\"/><style>h1 "
                                          "{ margin: 5px 0 20px 0; }h2, h3 { margin: 10px 0 15px 0; }"
                                          "img { max-width: 100%; }pre { background-color: #f1f1f1; "
                                          "border-radius: 5px; padding: 10px; }pre > code { padding: 0; }"
                                          "table {border-spacing: 0; border-style: solid; "
                                          "border-width: 1px; border-collapse: collapse; margin-top: 0.5em;}"
                                          "th, td {padding: 2px 5px;}a { color: #FF9137; "
                                          "text-decoration: none; } code { padding: 3px; overflow: auto;"
                                          " line-height: 1.45em; background-color: #f1f1f1; "
                                          "border-radius: 5px; color: #000000; } .code-comment { c"
                                          "olor: #75715E; font-style: italic;} .code-string "
                                          "{ color: #E6DB74;} .code-literal { color: #AE81FF;} .code-type "
                                          "{ color: #66D9EF;} .code-builtin { color: #A6E22E;} "
                                          ".code-keyword { color: #F92672;} .code-other { color: #F92672;}  "
                                          "</style></head><body class=\"export\"><h1 id=\"toc_0\">MyTestNote</h1>\n\n<p>Some text</p>\n</body></html>");

    QVERIFY(html == expectedHtml);
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
    CodeToHtmlConverter c1(comment.midRef(0, comment.length()), QStringLiteral("cpp"));
    QString outputMultiLineComment = c1.process();
    QString expectedOutputMultiLineComment = QStringLiteral("<span class=\"code-comment\">&#47;*hello my qownnotes blah blah single line*&#47;</span>\n");

    QVERIFY(outputMultiLineComment == expectedOutputMultiLineComment);
}

void TestNotes::testCodeToHtmlNumericLiterals()
{
    QString a = "123";
    QString a1 = "123+321";
    QString a2 = "(123)";
    QString a3 = "[123]";
    QString bad = "a123";
    QString a5 = "0x123";

    CodeToHtmlConverter c1(a.midRef(0, a.length()), QStringLiteral("c"));
    CodeToHtmlConverter c2(a1.midRef(0, a1.length()), QStringLiteral("c"));
    CodeToHtmlConverter c3(a2.midRef(0, a2.length()), QStringLiteral("c"));
    CodeToHtmlConverter c4(a3.midRef(0, a3.length()), QStringLiteral("c"));
    CodeToHtmlConverter c5(bad.midRef(0, bad.length()), QStringLiteral("c"));
    CodeToHtmlConverter c6(a5.midRef(0, a5.length()), QStringLiteral("c"));

    QString ar = c1.process();
    QString a1r = c2.process();
    QString a2r = c3.process();
    QString a3r = c4.process();
    QString badr = c5.process();
    QString a5r = c6.process();

    QString e = "<span class=\"code-literal\">123</span>";
    QString e1 = "<span class=\"code-literal\">123</span>+<span class=\"code-literal\">321</span>";
    QString e2 = "(<span class=\"code-literal\">123</span>)";
    QString e3 = "[<span class=\"code-literal\">123</span>]";
    QString eBad = "a123";
    QString e5 = "<span class=\"code-literal\">0x123</span>";

    QVERIFY(ar == e);
    QVERIFY(a1r == e1);
    QVERIFY(a2r == e2);
    QVERIFY(a3r == e3);
    QVERIFY(badr == eBad);
    QVERIFY(a5r == e5);
}

//QTEST_MAIN(TestNotes)
