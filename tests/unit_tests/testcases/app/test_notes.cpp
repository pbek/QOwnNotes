#include "test_notes.h"

#include <QCoreApplication>
#include <QString>
#include <QtTest>

#include "helpers/codetohtmlconverter.h"
#include "services/databaseservice.h"

// TestNotes::TestNotes()
//{
//}

void TestNotes::initTestCase() {
    DatabaseService::createConnection();
    DatabaseService::setupTables();

    // generate a notes path
    notesPath = QDir::tempPath() + QDir::separator() +
                QStringLiteral("qownnotes_test_") + QString::number(qrand());
    // qDebug() << "generated notesPath:" << notesPath;

    // create temporary notes directory
    QDir dir;
    dir.mkpath(notesPath);

    // store notes path for notes
    QSettings settings;
    settings.setValue(QStringLiteral("notesPath"), notesPath);

    // create a note file
    noteName = QStringLiteral("MyTestNote");
    noteFile = noteName + QStringLiteral(".md");
    noteFileName = notesPath + QDir::separator() + noteFile;

    QFile file(noteFileName);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << noteName << endl
               << "============" << endl
               << endl
               << "Some text";
        file.close();
    }
}

void TestNotes::cleanupTestCase() {
    QDir dir(notesPath);

    // remove temporary notes directory
    if (notesPath.startsWith(QDir::tempPath()) && dir.exists(notesPath)) {
        Q_FOREACH (QFileInfo info,
                   dir.entryInfoList(
                       QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files,
                       QDir::DirsFirst)) {
            if (info.isFile()) {
                QFile::remove(info.absoluteFilePath());
            }
        }

        dir.rmdir(notesPath);
    }
}

void TestNotes::testNoteCreating() {
    QFile file(noteFileName);
    Note note;
    note.createFromFile(file);

    QVERIFY(note.getId() == 1);
    QVERIFY(note.getName() == noteName);
    QVERIFY(note.getNoteText().startsWith(noteName));
}

void TestNotes::testNoteEncryption() {
    QFile file(noteFileName);
    Note note;
    note.createFromFile(file);

    note.setCryptoPassword(QStringLiteral("test"));
    note.encryptNoteText();

    QVERIFY(note.getId() == 2);
    QVERIFY(note.getName() == noteName);
    QVERIFY(note.getNoteText() ==
            QStringLiteral(
                "MyTestNote\n============\n\n<!-- BEGIN ENCRYPTED TEXT "
                "--\nVTVdShbeNi63fYLB7B56pg==\n-- END ENCRYPTED TEXT -->"));
}

void TestNotes::testNoteDecryption() {
    auto note = Note::fetch(2);
    note.setCryptoPassword("test");

    QVERIFY(note.getId() == 2);
    QVERIFY(note.getDecryptedNoteText() ==
            QStringLiteral("MyTestNote\n============\n\nSome text"));
}

void TestNotes::testNoteDecryptionFail() {
    auto note = Note::fetch(2);
    note.setCryptoPassword("test2");

    QVERIFY(note.getId() == 2);
    QVERIFY(note.getDecryptedNoteText() !=
            QStringLiteral("MyTestNote\n============\n\nSome text"));
}

void TestNotes::testNoteToMarkdownHtml() {
    QFile file(noteFileName);
    Note note;
    note.createFromFile(file);

    QString html = note.toMarkdownHtml("", 980, true);
    QString expectedBody =
        QStringLiteral("<h1>MyTestNote</h1>\n<p>Some text</p>");

    QVERIFY(html.contains(expectedBody));
}

void TestNotes::testMarkdownTildeCodeFenceToHtml() {
    QString code = QStringLiteral("# Tilde Code Fence\n");
    code += QStringLiteral("~~~cpp\n");
    code += QStringLiteral("#include<iostream>\n");
    code += QStringLiteral("using namespace std;\n");
    code += QStringLiteral("int main() {\n");
    code += QStringLiteral("std::cout<<\"hello\"\n");
    code += QStringLiteral("return 0;\n");
    code += QStringLiteral("}\n");
    code += QStringLiteral("~~~\n");

    Note note;
    note.setNoteText(code);
    QString result = note.toMarkdownHtml("", 980, true);
    QString expected = QStringLiteral(
        "<h1>Tilde Code Fence</h1>\n<pre><code class=\"language-cpp\">#<span "
        "class=\"code-other\">include</span>&lt;iostream&gt;\n<span "
        "class=\"code-keyword\">using</span> <span "
        "class=\"code-keyword\">namespace</span> <span "
        "class=\"code-builtin\">std</span>;\n<span "
        "class=\"code-type\">int</span> <span "
        "class=\"code-builtin\">main</span>&#40;&#41; {\n<span "
        "class=\"code-builtin\">std</span>::<span "
        "class=\"code-builtin\">cout</span>&lt;&lt;<span "
        "class=\"code-string\">&quot;hello&quot;</span>\n<span "
        "class=\"code-keyword\">return</span> <span "
        "class=\"code-literal\">0</span>;\n}</code></pre>");

    QVERIFY(result.contains(expected));
}

void TestNotes::testMarkdownBacktickCodeFenceToHtml() {
    QString code = QStringLiteral("# Backtick Code Fence\n");
    code += QStringLiteral("```cpp\n");
    code += QStringLiteral("#include<iostream>\n");
    code += QStringLiteral("using namespace std;\n");
    code += QStringLiteral("int main() {\n");
    code += QStringLiteral("std::cout<<\"hello\"\n");
    code += QStringLiteral("return 0;\n");
    code += QStringLiteral("}\n");
    code += QStringLiteral("```\n");

    Note note;
    note.setNoteText(code);
    QString result = note.toMarkdownHtml("", 980, true);
    QString expected = QStringLiteral(
        "<h1>Backtick Code Fence</h1>\n<pre><code "
        "class=\"language-cpp\">#<span "
        "class=\"code-other\">include</span>&lt;iostream&gt;\n<span "
        "class=\"code-keyword\">using</span> <span "
        "class=\"code-keyword\">namespace</span> <span "
        "class=\"code-builtin\">std</span>;\n<span "
        "class=\"code-type\">int</span> <span "
        "class=\"code-builtin\">main</span>&#40;&#41; {\n<span "
        "class=\"code-builtin\">std</span>::<span "
        "class=\"code-builtin\">cout</span>&lt;&lt;<span "
        "class=\"code-string\">&quot;hello&quot;</span>\n<span "
        "class=\"code-keyword\">return</span> <span "
        "class=\"code-literal\">0</span>;\n}</code></pre>");

    QVERIFY(result.contains(expected));
}

void TestNotes::testCodeToHtmlConversionPython() {
    QString pythonCode =
        "import hello\n"
        "if this()\n"
        "\tthat\n"
        "# this is a comment\n";

    CodeToHtmlConverter c("python");
    QString outputPython = c.process(pythonCode.midRef(0, pythonCode.length()));

    QString expectedOutputPython = QStringLiteral(
        "<span class=\"code-other\">import</span> hello\n"
        "<span class=\"code-keyword\">if</span> this&#40;&#41;\n"
        "\tthat\n"
        "<span class=\"code-comment\"># this is a comment</span>\n");

    QVERIFY(outputPython == expectedOutputPython);
}

void TestNotes::testCodeToHtmlConversionHashComment() {
    QString hashStyleComment =
        QStringLiteral("#hello my qownnotes blah blah\n");
    CodeToHtmlConverter c2(QStringLiteral("bash"));
    QString outputHashStyleComment =
        c2.process(hashStyleComment.midRef(0, hashStyleComment.length()));
    QString expectedOutputHashStyleComment = QStringLiteral(
        "<span class=\"code-comment\">#hello my qownnotes blah blah</span>\n");

    QVERIFY(outputHashStyleComment == expectedOutputHashStyleComment);
}

void TestNotes::testCodeToHtmlConversionSingleLineComment() {
    QString comment =
        QStringLiteral("//hello my qownnotes blah blah single line\n");
    QString commentpy =
        QStringLiteral("#hello my qownnotes blah blah single line\n");

    CodeToHtmlConverter cpp(QStringLiteral("cpp"));
    CodeToHtmlConverter py(QStringLiteral("py"));

    QString outputSingleLineComment =
        cpp.process(comment.midRef(0, comment.length()));
    QString outputCommentPy =
        py.process(commentpy.midRef(0, commentpy.length()));

    QString expectedOutputSingleLineComment = QStringLiteral(
        "<span class=\"code-comment\">&#47;&#47;hello my qownnotes blah blah "
        "single line</span>\n");
    QString expectedCommentPy = QStringLiteral(
        "<span class=\"code-comment\">#hello my qownnotes blah blah single "
        "line</span>\n");

    QVERIFY(outputSingleLineComment == expectedOutputSingleLineComment);
    QVERIFY(outputCommentPy == expectedCommentPy);
}

void TestNotes::testCodeToHtmlConversionMultiLineComment() {
    QString comment =
        QStringLiteral("/*hello my qownnotes blah blah single line*/\n");
    QString commentTrueMultiLine =
        QStringLiteral("/*hello my \nqownnotes blah \nblah single line*/\n");
    QString commentTrueMultiLineNoEnd =
        QStringLiteral("/*hello my \nqownnotes blah \nblah single line\n");
    QString notAComment = QStringLiteral("not/ a /comment");

    CodeToHtmlConverter c(QStringLiteral("cpp"));
    CodeToHtmlConverter css("css");

    QString outputMultiLineComment =
        c.process(comment.midRef(0, comment.length()));
    QString outputTrueMultiLine = c.process(
        commentTrueMultiLine.midRef(0, commentTrueMultiLine.length()));
    QString outputTrueMultiLineNoEnd =
        c.process(commentTrueMultiLineNoEnd.midRef(
            0, commentTrueMultiLineNoEnd.length()));
    QString outputNotAComment =
        c.process(notAComment.midRef(0, notAComment.length()));
    QString outputCSSNotAComment =
        css.process(notAComment.midRef(0, notAComment.length()));

    QString expectedMultiLineComment = QStringLiteral(
        "<span class=\"code-comment\">&#47;*hello my qownnotes blah blah "
        "single line*&#47;</span>\n");
    QString expectedTrueMultiLine = QStringLiteral(
        "<span class=\"code-comment\">&#47;*hello my \nqownnotes blah \nblah "
        "single line*&#47;</span>\n");
    QString expectedTrueMultiLineNoEnd = QStringLiteral(
        "<span class=\"code-comment\">&#47;*hello my \nqownnotes blah \nblah "
        "single line</span>\n");
    QString expectedNotAComment = QStringLiteral("not&#47; a &#47;comment");
    QString expectedCSSNotAComment = QStringLiteral("not&#47; a &#47;comment");

    QVERIFY(outputMultiLineComment == expectedMultiLineComment);
    QVERIFY(outputTrueMultiLine == expectedTrueMultiLine);
    QVERIFY(outputTrueMultiLineNoEnd == expectedTrueMultiLineNoEnd);
    QVERIFY(outputNotAComment == expectedNotAComment);
    QVERIFY(outputCSSNotAComment == expectedCSSNotAComment);
}

void TestNotes::testCodeToHtmlNumericLiterals() {
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

    CodeToHtmlConverter c(QStringLiteral("c"));
    CodeToHtmlConverter css(QStringLiteral("css"));
    QString ar = c.process(a.midRef(0, a.length()));
    QString a1r = c.process(a1.midRef(0, a1.length()));
    QString a2r = c.process(a2.midRef(0, a2.length()));
    QString a3r = c.process(a3.midRef(0, a3.length()));
    QString badResult = c.process(bad.midRef(0, bad.length()));
    QString badResult1 = c.process(bad1.midRef(0, bad1.length()));
    QString badResult2 = c.process(bad2.midRef(0, bad2.length()));
    QString badResult3 = c.process(bad3.midRef(0, bad3.length()));
    QString badResult4 = c.process(bad4.midRef(0, bad4.length()));
    QString a5r = c.process(a5.midRef(0, a5.length()));
    QString a6r = css.process(a6.midRef(0, a6.length()));
    QString a7r = css.process(a7.midRef(0, a7.length()));

    QString e = "<span class=\"code-literal\">123</span>";
    QString e1 =
        "<span class=\"code-literal\">123</span>+<span "
        "class=\"code-literal\">321</span>";
    QString e2 = "&#40;<span class=\"code-literal\">123</span>&#41;";
    QString e3 = "[<span class=\"code-literal\">123</span>]";
    QString expectedBad = "a123";
    QString expectedBad1 = "123a";
    QString expectedBad2 = "12a3";
    QString expectedBad3 = "a123b";
    QString expectedBad4 = "12pxa";
    QString e5 = "<span class=\"code-literal\">0x123</span>";
    QString e6 =
        "<span class=\"code-literal\">12</span><span "
        "class=\"code-keyword\">px</span>\n";
    QString e7 =
        "<span class=\"code-literal\">12</span><span "
        "class=\"code-keyword\">em</span>\n";

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

void TestNotes::testCodeToHtmlNumericEquations() {
    QString a1 = "123+321";
    QString a2 = "123-321";
    QString a3 = "123/321";
    QString a4 = "123*321";
    QString a5 = "123%321";
    QString a6 = "a<<1";
    QString a7 = "b>>2";
    QString a8 = "100>>200";
    QString a9 = "[(1+2-3)+4/5] array[5] list{3}";

    CodeToHtmlConverter c(QStringLiteral("c"));
    QString a1r = c.process(a1.midRef(0, a1.length()));
    QString a2r = c.process(a2.midRef(0, a2.length()));
    QString a3r = c.process(a3.midRef(0, a3.length()));
    QString a4r = c.process(a4.midRef(0, a4.length()));
    QString a5r = c.process(a5.midRef(0, a5.length()));
    QString a6r = c.process(a6.midRef(0, a6.length()));
    QString a7r = c.process(a7.midRef(0, a7.length()));
    QString a8r = c.process(a8.midRef(0, a8.length()));
    QString a9r = c.process(a9.midRef(0, a9.length()));

    QString e1 =
        "<span class=\"code-literal\">123</span>+<span "
        "class=\"code-literal\">321</span>";
    QString e2 =
        "<span class=\"code-literal\">123</span>-<span "
        "class=\"code-literal\">321</span>";
    QString e3 =
        "<span class=\"code-literal\">123</span>&#47;<span "
        "class=\"code-literal\">321</span>";
    QString e4 =
        "<span class=\"code-literal\">123</span>*<span "
        "class=\"code-literal\">321</span>";
    QString e5 =
        "<span class=\"code-literal\">123</span>%<span "
        "class=\"code-literal\">321</span>";
    QString e6 = "a&lt;&lt;<span class=\"code-literal\">1</span>";
    QString e7 = "b&gt;&gt;<span class=\"code-literal\">2</span>";
    QString e8 =
        "<span class=\"code-literal\">100</span>&gt;&gt;<span "
        "class=\"code-literal\">200</span>";
    QString e9 =
        "[&#40;<span class=\"code-literal\">1</span>+<span "
        "class=\"code-literal\">2</span>-<span "
        "class=\"code-literal\">3</span>&#41;+<span "
        "class=\"code-literal\">4</span>&#47;<span "
        "class=\"code-literal\">5</span>] <span "
        "class=\"code-builtin\">array</span>[<span "
        "class=\"code-literal\">5</span>] <span "
        "class=\"code-builtin\">list</span>{<span "
        "class=\"code-literal\">3</span>}";

    QVERIFY(a1r == e1);
    QVERIFY(a2r == e2);
    QVERIFY(a3r == e3);
    QVERIFY(a4r == e4);
    QVERIFY(a5r == e5);
    QVERIFY(a6r == e6);
    QVERIFY(a7r == e7);
    QVERIFY(a8r == e8);
    QVERIFY(a9r == e9);
}

void TestNotes::testCodeToHtmlStringLiterals() {
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

    QString charBef = "G\"hello\"";

    CodeToHtmlConverter c(QStringLiteral("c"));
    // R = result
    QString sR = c.process(s.midRef(0, s.length()));
    QString saR = c.process(sa.midRef(0, sa.length()));
    QString sbR = c.process(sb.midRef(0, sb.length()));
    QString seR = c.process(se.midRef(0, se.length()));
    QString sfR = c.process(sf.midRef(0, sf.length()));
    QString snR = c.process(sn.midRef(0, sn.length()));
    QString srR = c.process(sr.midRef(0, sr.length()));
    QString stR = c.process(st.midRef(0, st.length()));
    QString svR = c.process(sv.midRef(0, sv.length()));
    QString sslashR = c.process(sslash.midRef(0, sslash.length()));
    QString sqR = c.process(sq.midRef(0, sq.length()));
    QString squoteR = c.process(squote.midRef(0, squote.length()));
    QString charBefR = c.process(charBef.midRef(0, charBef.length()));

    // e = expected
    QString e =
        QStringLiteral("<span class=\"code-string\">&quot;hello&quot;</span>");
    QString esa = QStringLiteral(
        "<span class=\"code-string\">&quot;hell</span><span "
        "class=\"code-literal\">\\a</span><span "
        "class=\"code-string\">&quot;</span>");
    QString esb = QStringLiteral(
        "<span class=\"code-string\">&quot;hell</span><span "
        "class=\"code-literal\">\\b</span><span "
        "class=\"code-string\">&quot;</span>");
    QString ese = QStringLiteral(
        "<span class=\"code-string\">&quot;hell</span><span "
        "class=\"code-literal\">\\e</span><span "
        "class=\"code-string\">&quot;</span>");
    QString esf = QStringLiteral(
        "<span class=\"code-string\">&quot;hell</span><span "
        "class=\"code-literal\">\\f</span><span "
        "class=\"code-string\">&quot;</span>");
    QString esn = QStringLiteral(
        "<span class=\"code-string\">&quot;hell</span><span "
        "class=\"code-literal\">\\n</span><span "
        "class=\"code-string\">&quot;</span>");
    QString esr = QStringLiteral(
        "<span class=\"code-string\">&quot;hell</span><span "
        "class=\"code-literal\">\\r</span><span "
        "class=\"code-string\">&quot;</span>");
    QString est = QStringLiteral(
        "<span class=\"code-string\">&quot;hell</span><span "
        "class=\"code-literal\">\\t</span><span "
        "class=\"code-string\">&quot;</span>");
    QString esv = QStringLiteral(
        "<span class=\"code-string\">&quot;hell</span><span "
        "class=\"code-literal\">\\v</span><span "
        "class=\"code-string\">&quot;</span>");
    QString esslash = QStringLiteral(
        "<span class=\"code-string\">&quot; hell</span><span "
        "class=\"code-literal\">\\\\</span><span class=\"code-string\"> "
        "&quot;</span>");
    QString esq = QStringLiteral(
        "<span class=\"code-string\">&quot;hell</span><span "
        "class=\"code-literal\">\\?</span><span "
        "class=\"code-string\">&quot;</span>");
    QString esquote = QStringLiteral(
        "<span class=\"code-string\">&quot; hell </span><span "
        "class=\"code-literal\">\\&quot;</span><span class=\"code-string\"> "
        "&quot;</span>");
    QString eCharBef =
        QStringLiteral("G<span class=\"code-string\">&quot;hello&quot;</span>");

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
    QVERIFY(charBefR == eCharBef);
}

void TestNotes::testOctal() {
    QString o1 = QStringLiteral("\"Waqar\\256\"");
    QString ofail1 = QStringLiteral("\"Waqar\\183\"");
    QString ofail2 = QStringLiteral("\"Waqar\\1234\"");

    QString eOctal = QStringLiteral(
        "<span class=\"code-string\">&quot;Waqar</span><span "
        "class=\"code-literal\">\\256</span><span "
        "class=\"code-string\">&quot;</span>");
    QString eOctalFail1 = QStringLiteral(
        "<span class=\"code-string\">&quot;Waqar\\183&quot;</span>");
    QString eOctalFail2 = QStringLiteral(
        "<span class=\"code-string\">&quot;Waqar</span><span "
        "class=\"code-literal\">\\123</span><span "
        "class=\"code-string\">4&quot;</span>");

    CodeToHtmlConverter c(QStringLiteral("c"));

    QString octal = c.process(o1.midRef(0, o1.length()));
    QString octalFail1 = c.process(ofail1.midRef(0, ofail1.length()));
    QString octalFail2 = c.process(ofail2.midRef(0, ofail2.length()));

    QVERIFY(octal == eOctal);
    QVERIFY(octalFail1 == eOctalFail1);
    QVERIFY(octalFail2 == eOctalFail2);
}

void TestNotes::testHex() {
    QString h1 = QStringLiteral("\"Waqar\\x1b\"");
    QString hfail = QStringLiteral("\"Waqar\\x123\"");

    CodeToHtmlConverter c(QStringLiteral("c"));

    QString hR = c.process(h1.midRef(0, h1.length()));
    QString hfailR = c.process(hfail.midRef(0, hfail.length()));

    QString eh = QStringLiteral(
        "<span class=\"code-string\">&quot;Waqar</span><span "
        "class=\"code-literal\">\\x1b</span><span "
        "class=\"code-string\">&quot;</span>");
    QString ehFail = QStringLiteral(
        "<span class=\"code-string\">&quot;Waqar</span><span "
        "class=\"code-literal\">\\x12</span><span "
        "class=\"code-string\">3&quot;</span>");

    QVERIFY(hR == eh);
    QVERIFY(hfailR == ehFail);
}

void TestNotes::testHTMLescape() {
    QString str = "/ & < >";
    CodeToHtmlConverter c1("");
    QString result = c1.process(str.midRef(0, str.length()));
    QString expected = "&#47; &amp; &lt; &gt;";
    QVERIFY(result == expected);
}

// QTEST_MAIN(TestNotes)
