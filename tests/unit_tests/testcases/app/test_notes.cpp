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
    QString commentpy = QStringLiteral("#hello my qownnotes blah blah single line\n");

    CodeToHtmlConverter c1(comment.midRef(0, comment.length()), QStringLiteral("cpp"));
    CodeToHtmlConverter c2(comment2.midRef(0, comment2.length()), QStringLiteral("py"));
    CodeToHtmlConverter c3(commentpy.midRef(0, commentpy.length()), QStringLiteral("py"));

    QString outputSingleLineComment = c1.process();
    QString outputSingleLineComment2 = c2.process();
    QString outputCommentPy = c3.process();

    QString expectedOutputSingleLineComment = QStringLiteral("<span class=\"code-comment\">&#47;&#47;hello my qownnotes blah blah single line</span>\n");
    QString expectedCommentPy = QStringLiteral("<span class=\"code-comment\">#hello my qownnotes blah blah single line</span>\n");

    QVERIFY(outputSingleLineComment == expectedOutputSingleLineComment);
    QVERIFY(outputSingleLineComment2 != expectedOutputSingleLineComment);
    QVERIFY(outputCommentPy == expectedCommentPy);
}

void TestNotes::testCodeToHtmlConversionMultiLineComment()
{
    QString comment = QStringLiteral("/*hello my qownnotes blah blah single line*/\n");
    QString commentTrueMultiLine = QStringLiteral("/*hello my \nqownnotes blah \nblah single line*/\n");
    QString commentTrueMultiLineNoEnd = QStringLiteral("/*hello my \nqownnotes blah \nblah single line\n");
    QString notAComment = QStringLiteral("not/ a /comment");

    CodeToHtmlConverter c1(comment.midRef(0, comment.length()), QStringLiteral("cpp"));
    CodeToHtmlConverter c2(commentTrueMultiLine.midRef(0, commentTrueMultiLine.length()), QStringLiteral("cpp"));
    CodeToHtmlConverter c3(commentTrueMultiLineNoEnd.midRef(0, commentTrueMultiLineNoEnd.length()), QStringLiteral("cpp"));
    CodeToHtmlConverter c4(notAComment.midRef(0, notAComment.length()), "c");
    CodeToHtmlConverter c5(notAComment.midRef(0, notAComment.length()), "css");

    QString outputMultiLineComment = c1.process();
    QString outputTrueMultiLine = c2.process();
    QString outputTrueMultiLineNoEnd = c3.process();
    QString outputNotAComment = c4.process();
    QString outputCSSNotAComment = c5.process();

    QString expectedMultiLineComment = QStringLiteral("<span class=\"code-comment\">&#47;*hello my qownnotes blah blah single line*&#47;</span>\n");
    QString expectedTrueMultiLine = QStringLiteral("<span class=\"code-comment\">&#47;*hello my \nqownnotes blah \nblah single line*&#47;</span>\n");
    QString expectedTrueMultiLineNoEnd = QStringLiteral("<span class=\"code-comment\">&#47;*hello my \nqownnotes blah \nblah single line</span>\n");
    QString expectedNotAComment = QStringLiteral("not&#47; a &#47;comment");
    QString expectedCSSNotAComment = QStringLiteral("not&#47; a &#47;comment");

    QVERIFY(outputMultiLineComment == expectedMultiLineComment);
    QVERIFY(outputTrueMultiLine == expectedTrueMultiLine);
    QVERIFY(outputTrueMultiLineNoEnd == expectedTrueMultiLineNoEnd);
    QVERIFY(outputNotAComment == expectedNotAComment);
    QVERIFY(outputCSSNotAComment == expectedCSSNotAComment);
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

void TestNotes::testCodeToHtmlStringLiterals()
{
    QString s = "\"hello\"";
    /*single char escapes */
    QString sa = "\"hell\\a\"";
    QString sb = "\"hell\\b\"";
    QString se = "\"hell\\e\"";
    QString sf = "\"hell\\f\"";
    QString sn = "\"hell\\n\"";
    QString sr = "\"hell\\r\"";
    QString st = "\"hell\\t\"";
    QString sv = "\"hell\\v\"";
    QString sslash = "\" hell\\\\ \"";
    QString squote = "\" hell \\\" \"";
    QString sq = "\"hell\\?\"";

    CodeToHtmlConverter c1(s.midRef(0, s.length()), QStringLiteral("c"));
    /*single char escapes expect */
    CodeToHtmlConverter c2(sa.midRef(0, sa.length()), QStringLiteral("c"));
    CodeToHtmlConverter c3(sb.midRef(0, sb.length()), QStringLiteral("c"));
    CodeToHtmlConverter c4(se.midRef(0, se.length()), QStringLiteral("c"));
    CodeToHtmlConverter c5(sf.midRef(0, sf.length()), QStringLiteral("c"));
    CodeToHtmlConverter c6(sn.midRef(0, sn.length()), QStringLiteral("c"));
    CodeToHtmlConverter c7(sr.midRef(0, sr.length()), QStringLiteral("c"));
    CodeToHtmlConverter c8(st.midRef(0, st.length()), QStringLiteral("c"));
    CodeToHtmlConverter c9(sv.midRef(0, sv.length()), QStringLiteral("c"));
    CodeToHtmlConverter c10(sslash.midRef(0, sslash.length()), QStringLiteral("c"));
    CodeToHtmlConverter c11(sq.midRef(0, sq.length()), QStringLiteral("c"));
    CodeToHtmlConverter c12(squote.midRef(0, squote.length()), QStringLiteral("c"));

    QString sR = c1.process();
    QString saR = c2.process();
    QString sbR = c3.process();
    QString seR = c4.process();
    QString sfR = c5.process();
    QString snR = c6.process();
    QString srR = c7.process();
    QString stR = c8.process();
    QString svR = c9.process();
    QString sslashR = c10.process();
    QString sqR = c11.process();
    QString squoteR = c12.process();

    QString e = QStringLiteral("<span class=\"code-string\">&quot;hello&quot;</span>");
    QString esa = QStringLiteral("<span class=\"code-string\">&quot;hell</span><span class=\"code-literal\">\\a</span><span class=\"code-string\">&quot;</span>");
    QString esb = QStringLiteral("<span class=\"code-string\">&quot;hell</span><span class=\"code-literal\">\\b</span><span class=\"code-string\">&quot;</span>");
    QString ese = QStringLiteral("<span class=\"code-string\">&quot;hell</span><span class=\"code-literal\">\\e</span><span class=\"code-string\">&quot;</span>");
    QString esf = QStringLiteral("<span class=\"code-string\">&quot;hell</span><span class=\"code-literal\">\\f</span><span class=\"code-string\">&quot;</span>");
    QString esn = QStringLiteral("<span class=\"code-string\">&quot;hell</span><span class=\"code-literal\">\\n</span><span class=\"code-string\">&quot;</span>");
    QString esr = QStringLiteral("<span class=\"code-string\">&quot;hell</span><span class=\"code-literal\">\\r</span><span class=\"code-string\">&quot;</span>");
    QString est = QStringLiteral("<span class=\"code-string\">&quot;hell</span><span class=\"code-literal\">\\t</span><span class=\"code-string\">&quot;</span>");
    QString esv = QStringLiteral("<span class=\"code-string\">&quot;hell</span><span class=\"code-literal\">\\v</span><span class=\"code-string\">&quot;</span>");
    QString esslash = QStringLiteral("<span class=\"code-string\">&quot; hell</span><span class=\"code-literal\">\\\\</span><span class=\"code-string\"> &quot;</span>");
    QString esq = QStringLiteral("<span class=\"code-string\">&quot;hell</span><span class=\"code-literal\">\\?</span><span class=\"code-string\">&quot;</span>");
    QString esquote = QStringLiteral("<span class=\"code-string\">&quot; hell </span><span class=\"code-literal\">\\&quot;</span><span class=\"code-string\"> &quot;</span>");

    QVERIFY(sR == e);
    QVERIFY(saR == esa);
    QVERIFY(sbR == esb);
    QVERIFY(seR == ese);
    QVERIFY(sfR == esf);
    QVERIFY(snR == esn);
    QVERIFY(srR == esr);
    QVERIFY(stR == est);
    QVERIFY(svR == esv);
    QVERIFY(sslashR == esslash);
    QVERIFY(sqR == esq);
    QVERIFY(squoteR == esquote);
}

void TestNotes::testOctal()
{
    QString o1 = QStringLiteral("\"Waqar\\256\"");
    QString ofail1 = QStringLiteral("\"Waqar\\183\"");
    QString ofail2 = QStringLiteral("\"Waqar\\1234\"");

    QString eOctal = QStringLiteral("<span class=\"code-string\">&quot;Waqar</span><span class=\"code-literal\">\\256</span><span class=\"code-string\">&quot;</span>");
    QString eOctalFail1 = QStringLiteral("<span class=\"code-string\">&quot;Waqar\\183&quot;</span>");
    QString eOctalFail2 = QStringLiteral("<span class=\"code-string\">&quot;Waqar</span><span class=\"code-literal\">\\123</span><span class=\"code-string\">4&quot;</span>");

    CodeToHtmlConverter c13(o1.midRef(0, o1.length()), QStringLiteral("c"));
    CodeToHtmlConverter c14(ofail1.midRef(0, ofail1.length()), QStringLiteral("c"));
    CodeToHtmlConverter c15(ofail2.midRef(0, ofail2.length()), QStringLiteral("c"));

    QString octal = c13.process();
    QString octalFail1 = c14.process();
    QString octalFail2 = c15.process();

    QVERIFY(octal == eOctal);
    QVERIFY(octalFail1 == eOctalFail1);
    QVERIFY(octalFail2 == eOctalFail2);
}

void TestNotes::testHex()
{
    QString h1 = QStringLiteral("\"Waqar\\x1b\"");
    QString hfail = QStringLiteral("\"Waqar\\x123\"");

    CodeToHtmlConverter c16(h1.midRef(0, h1.length()), QStringLiteral("c"));
    CodeToHtmlConverter c17(hfail.midRef(0, hfail.length()), QStringLiteral("c"));

    QString hR = c16.process();
    QString hfailR = c17.process();

    QString eh = QStringLiteral("<span class=\"code-string\">&quot;Waqar</span><span class=\"code-literal\">\\x1b</span><span class=\"code-string\">&quot;</span>");
    QString ehFail = QStringLiteral("<span class=\"code-string\">&quot;Waqar</span><span class=\"code-literal\">\\x12</span><span class=\"code-string\">3&quot;</span>");

    QVERIFY(hR == eh);
    QVERIFY(hfailR == ehFail);
}

void TestNotes::testHTMLescape()
{
    QString str = "/ & < >";
    CodeToHtmlConverter c1(str.midRef(0, str.length()), "");
    QString result = c1.process();
    QString expected = "&#47; &amp; &lt; &gt;";
    QVERIFY(result == expected);
}

//QTEST_MAIN(TestNotes)
