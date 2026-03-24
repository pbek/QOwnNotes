#include "test_notes.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSet>
#include <QString>
#include <QTextStream>
#include <QUuid>
#include <QtTest>

#include "entities/bookmark.h"
#include "entities/notesubfolder.h"
#include "services/settingsservice.h"
#include "utils/urlhandler.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#endif

#include "helpers/codetohtmlconverter.h"
#include "services/databaseservice.h"

// TestNotes::TestNotes()
//{
//}

QString TestNotes::uniqueTestName(const QString &baseName) const {
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    return baseName + QStringLiteral("-") + QUuid::createUuid().toString(QUuid::WithoutBraces);
#else
    // QUuid::WithoutBraces was introduced in Qt 5.11; strip the surrounding braces manually
    return baseName + QStringLiteral("-") + QUuid::createUuid().toString().mid(1, 36);
#endif
}

NoteSubFolder TestNotes::createTestNoteSubFolder(const QString &name, int parentId) const {
    NoteSubFolder subFolder;
    subFolder.setName(name);
    subFolder.setParentId(parentId);
    if (!subFolder.store()) {
        qFatal("Failed to store note subfolder in test setup");
    }

    QDir dir(notesPath);
    if (!dir.mkpath(subFolder.relativePath())) {
        qFatal("Failed to create note subfolder on disk");
    }

    return subFolder;
}

Note TestNotes::createTestNote(const QString &name, int noteSubFolderId,
                               const QString &text) const {
    const QString filePath = Note::getFullFilePathForFile(
        noteSubFolderId > 0 ? NoteSubFolder::fetch(noteSubFolderId).relativePath() +
                                  QStringLiteral("/") + name + QStringLiteral(".md")
                            : name + QStringLiteral(".md"));

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qFatal("Failed to create test note file");
    }
    QTextStream stream(&file);
    stream << (text.isEmpty() ? QStringLiteral("# ") + name + QStringLiteral("\n") : text);
    file.close();

    QFile createdFile(filePath);
    Note note;
    note.createFromFile(createdFile, noteSubFolderId);
    if (!note.isFetched()) {
        qFatal("Failed to create test note from file");
    }

    return note;
}

void TestNotes::initTestCase() {
    DatabaseService::createConnection();
    DatabaseService::setupTables();

#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
    const quint32 number = qrand();
#else
    const quint32 number = QRandomGenerator::global()->generate();
#endif

    // generate a notes path
    notesPath = QDir::tempPath() + QDir::separator() + QStringLiteral("qownnotes_test_") +
                QString::number(number);
    // qDebug() << "generated notesPath:" << notesPath;

    // create temporary notes directory
    QDir dir;
    dir.mkpath(notesPath);

    // store notes path for notes
    SettingsService settings;
    settings.setValue(QStringLiteral("notesPath"), notesPath);
    wikiLinkSupportSetting = settings.value(QStringLiteral("Editor/wikiLinkSupport"));
    settings.setValue(QStringLiteral("Editor/wikiLinkSupport"), false);

    // create a note file
    noteName = QStringLiteral("MyTestNote");
    noteFile = noteName + QStringLiteral(".md");
    noteFileName = notesPath + QDir::separator() + noteFile;

    QFile file(noteFileName);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        stream << noteName << Qt::endl
               << "============" << Qt::endl
               << Qt::endl
#else
        stream << noteName << endl
               << "============" << endl
               << endl
#endif
               << "Some text";
        file.close();
    }
}

void TestNotes::cleanupTestCase() {
    SettingsService settings;
    if (wikiLinkSupportSetting.isValid()) {
        settings.setValue(QStringLiteral("Editor/wikiLinkSupport"), wikiLinkSupportSetting);
    } else {
        settings.remove(QStringLiteral("Editor/wikiLinkSupport"));
    }

    QDir dir(notesPath);

    // remove temporary notes directory
    if (notesPath.startsWith(QDir::tempPath()) && dir.exists(notesPath)) {
        dir.removeRecursively();
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
            QStringLiteral("MyTestNote\n============\n\n<!-- BEGIN ENCRYPTED TEXT "
                           "--\nVTVdShbeNi63fYLB7B56pg==\n-- END ENCRYPTED TEXT -->"));
}

void TestNotes::testNoteDecryption() {
    auto note = Note::fetch(2);
    note.setCryptoPassword("test");

    QVERIFY(note.getId() == 2);
    QVERIFY(note.fetchDecryptedNoteText() ==
            QStringLiteral("MyTestNote\n============\n\nSome text"));
}

void TestNotes::testNoteDecryptionFail() {
    auto note = Note::fetch(2);
    note.setCryptoPassword("test2");

    QVERIFY(note.getId() == 2);
    QVERIFY(note.fetchDecryptedNoteText() !=
            QStringLiteral("MyTestNote\n============\n\nSome text"));
}

void TestNotes::testNoteToMarkdownHtml() {
    QFile file(noteFileName);
    Note note;
    note.createFromFile(file);

    QString html = note.toMarkdownHtml("", 980, true);
    QString expectedBody = QStringLiteral("<h1>MyTestNote</h1>\n<p>Some text</p>");

    QVERIFY(html.contains(expectedBody));
}

void TestNotes::testMarkdownTildeCodeFenceToHtml() {
    QString code = QStringLiteral("# Tilde Code Fence\n");
    code += QStringLiteral("~~~cpp\n");
    code += QStringLiteral("int main() {\n");
    code += QStringLiteral("return 0;\n");
    code += QStringLiteral("}\n");
    code += QStringLiteral("~~~\n");

    Note note;
    note.setNoteText(code);
    QString result = note.toMarkdownHtml("", 980, true);
    QString expected = QStringLiteral(
        "<h1>Tilde Code Fence</h1>\n<pre><code class=\"language-cpp\"><span "
        "class=\"code-type\">int</span> <span "
        "class=\"code-builtin\">main</span>&#40;&#41; {\n<span "
        "class=\"code-keyword\">return</span> <span "
        "class=\"code-literal\">0</span>;\n}</code></pre>");

    QVERIFY(result.contains(expected));
}

void TestNotes::testMarkdownBacktickCodeFenceToHtml() {
    QString code = QStringLiteral("# Backtick Code Fence\n");
    code += QStringLiteral("```cpp\n");
    code += QStringLiteral("int main() {\n");
    code += QStringLiteral("return 0;\n");
    code += QStringLiteral("}\n");
    code += QStringLiteral("```\n");

    Note note;
    note.setNoteText(code);
    QString result = note.toMarkdownHtml("", 980, true);
    QString expected = QStringLiteral(
        "<h1>Backtick Code Fence</h1>\n<pre><code class=\"language-cpp\"><span "
        "class=\"code-type\">int</span> <span "
        "class=\"code-builtin\">main</span>&#40;&#41; {\n<span "
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
    QString outputPython = c.process(pythonCode.mid(0, pythonCode.length()));

    QString expectedOutputPython = QStringLiteral(
        "<span class=\"code-other\">import</span> hello\n"
        "<span class=\"code-keyword\">if</span> this&#40;&#41;\n"
        "\tthat\n"
        "<span class=\"code-comment\"># this is a comment</span>\n");

    QVERIFY(outputPython == expectedOutputPython);
}

void TestNotes::testCodeToHtmlConversionHashComment() {
    QString hashStyleComment = QStringLiteral("#hello my qownnotes blah blah\n");
    CodeToHtmlConverter c2(QStringLiteral("bash"));
    QString outputHashStyleComment = c2.process(hashStyleComment.mid(0, hashStyleComment.length()));
    QString expectedOutputHashStyleComment =
        QStringLiteral("<span class=\"code-comment\">#hello my qownnotes blah blah</span>\n");

    QVERIFY(outputHashStyleComment == expectedOutputHashStyleComment);
}

void TestNotes::testCodeToHtmlConversionSingleLineComment() {
    QString comment = QStringLiteral("//hello my qownnotes blah blah single line\n");
    QString commentpy = QStringLiteral("#hello my qownnotes blah blah single line\n");

    CodeToHtmlConverter cpp(QStringLiteral("cpp"));
    CodeToHtmlConverter py(QStringLiteral("py"));

    QString outputSingleLineComment = cpp.process(comment.mid(0, comment.length()));
    QString outputCommentPy = py.process(commentpy.mid(0, commentpy.length()));

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
    QString comment = QStringLiteral("/*hello my qownnotes blah blah single line*/\n");
    QString commentTrueMultiLine =
        QStringLiteral("/*hello my \nqownnotes blah \nblah single line*/\n");
    QString commentTrueMultiLineNoEnd =
        QStringLiteral("/*hello my \nqownnotes blah \nblah single line\n");
    QString notAComment = QStringLiteral("isnot/ a /comment");

    CodeToHtmlConverter c(QStringLiteral("cpp"));
    CodeToHtmlConverter css("css");

    QString outputMultiLineComment = c.process(comment.mid(0, comment.length()));
    QString outputTrueMultiLine =
        c.process(commentTrueMultiLine.mid(0, commentTrueMultiLine.length()));
    QString outputTrueMultiLineNoEnd =
        c.process(commentTrueMultiLineNoEnd.mid(0, commentTrueMultiLineNoEnd.length()));
    QString outputNotAComment = c.process(notAComment.mid(0, notAComment.length()));
    QString outputCSSNotAComment = css.process(notAComment.mid(0, notAComment.length()));

    QString expectedMultiLineComment = QStringLiteral(
        "<span class=\"code-comment\">&#47;*hello my qownnotes blah blah "
        "single line*&#47;</span>\n");
    QString expectedTrueMultiLine = QStringLiteral(
        "<span class=\"code-comment\">&#47;*hello my \nqownnotes blah \nblah "
        "single line*&#47;</span>\n");
    QString expectedTrueMultiLineNoEnd = QStringLiteral(
        "<span class=\"code-comment\">&#47;*hello my \nqownnotes blah \nblah "
        "single line</span>\n");
    QString expectedNotAComment = QStringLiteral("isnot&#47; a &#47;comment");
    QString expectedCSSNotAComment = QStringLiteral("isnot&#47; a &#47;comment");

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
    QString a7 = "12em;";

    CodeToHtmlConverter c(QStringLiteral("c"));
    CodeToHtmlConverter css(QStringLiteral("css"));
    QString ar = c.process(a.mid(0, a.length()));
    QString a1r = c.process(a1.mid(0, a1.length()));
    QString a2r = c.process(a2.mid(0, a2.length()));
    QString a3r = c.process(a3.mid(0, a3.length()));
    QString badResult = c.process(bad.mid(0, bad.length()));
    QString badResult1 = c.process(bad1.mid(0, bad1.length()));
    QString badResult2 = c.process(bad2.mid(0, bad2.length()));
    QString badResult3 = c.process(bad3.mid(0, bad3.length()));
    QString badResult4 = c.process(bad4.mid(0, bad4.length()));
    QString a5r = c.process(a5.mid(0, a5.length()));
    QString a7r = css.process(a7.mid(0, a7.length()));

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

    QString e7 = "<span class=\"code-literal\">12</span>em;";

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
    QString a1r = c.process(a1.mid(0, a1.length()));
    QString a2r = c.process(a2.mid(0, a2.length()));
    QString a3r = c.process(a3.mid(0, a3.length()));
    QString a4r = c.process(a4.mid(0, a4.length()));
    QString a5r = c.process(a5.mid(0, a5.length()));
    QString a6r = c.process(a6.mid(0, a6.length()));
    QString a7r = c.process(a7.mid(0, a7.length()));
    QString a8r = c.process(a8.mid(0, a8.length()));
    QString a9r = c.process(a9.mid(0, a9.length()));

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
    QString sR = c.process(s.mid(0, s.length()));
    QString saR = c.process(sa.mid(0, sa.length()));
    QString sbR = c.process(sb.mid(0, sb.length()));
    QString seR = c.process(se.mid(0, se.length()));
    QString sfR = c.process(sf.mid(0, sf.length()));
    QString snR = c.process(sn.mid(0, sn.length()));
    QString srR = c.process(sr.mid(0, sr.length()));
    QString stR = c.process(st.mid(0, st.length()));
    QString svR = c.process(sv.mid(0, sv.length()));
    QString sslashR = c.process(sslash.mid(0, sslash.length()));
    QString sqR = c.process(sq.mid(0, sq.length()));
    QString squoteR = c.process(squote.mid(0, squote.length()));
    QString charBefR = c.process(charBef.mid(0, charBef.length()));

    // e = expected
    QString e = QStringLiteral("<span class=\"code-string\">&quot;hello&quot;</span>");
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
    QString eCharBef = QStringLiteral("G<span class=\"code-string\">&quot;hello&quot;</span>");

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
    QString eOctalFail1 =
        QStringLiteral("<span class=\"code-string\">&quot;Waqar\\183&quot;</span>");
    QString eOctalFail2 = QStringLiteral(
        "<span class=\"code-string\">&quot;Waqar</span><span "
        "class=\"code-literal\">\\123</span><span "
        "class=\"code-string\">4&quot;</span>");

    CodeToHtmlConverter c(QStringLiteral("c"));

    QString octal = c.process(o1.mid(0, o1.length()));
    QString octalFail1 = c.process(ofail1.mid(0, ofail1.length()));
    QString octalFail2 = c.process(ofail2.mid(0, ofail2.length()));

    QVERIFY(octal == eOctal);
    QVERIFY(octalFail1 == eOctalFail1);
    QVERIFY(octalFail2 == eOctalFail2);
}

void TestNotes::testHex() {
    QString h1 = QStringLiteral("\"Waqar\\x1b\"");
    QString hfail = QStringLiteral("\"Waqar\\x123\"");

    CodeToHtmlConverter c(QStringLiteral("c"));

    QString hR = c.process(h1.mid(0, h1.length()));
    QString hfailR = c.process(hfail.mid(0, hfail.length()));

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
    QString result = c1.process(str.mid(0, str.length()));
    QString expected = "&#47; &amp; &lt; &gt;";
    QVERIFY(result == expected);
}

/**
 * Test that the XML highlighter properly escapes angle brackets so that
 * XML tags in code blocks are not stripped by the QLiteHTML preview.
 * See: https://github.com/pbek/QOwnNotes/issues/3521
 */
void TestNotes::testXmlHighlighterEscaping() {
    CodeToHtmlConverter c("xml");

    // Simple XML tag: both '<' and '>' must be escaped
    {
        QString input = QStringLiteral("<tag>content</tag>");
        QString result = c.process(input);
        QVERIFY2(result.contains(QStringLiteral("&lt;")), "Opening '<' must be escaped as &lt;");
        QVERIFY2(result.contains(QStringLiteral("&gt;")), "Closing '>' must be escaped as &gt;");
        QVERIFY2(!result.contains(QRegularExpression(QStringLiteral("(?<!&[a-z]+)>(?!</span>)"))),
                 "No unescaped '>' outside of span tags");
    }

    // XML processing instruction: '?' must be preserved
    {
        QString input = QStringLiteral("<?xml version=\"1.0\"?>");
        QString result = c.process(input);
        QVERIFY2(result.contains(QStringLiteral("?")),
                 "'?' in processing instruction must not be dropped");
    }

    // Nested XML: inner tags must have escaped angle brackets
    {
        QString input = QStringLiteral("<root><child>text</child></root>");
        QString result = c.process(input);
        // Count occurrences of &lt; - should be 4 (root open, child open,
        // child close, root close)
        QCOMPARE(result.count(QStringLiteral("&lt;")), 4);
        // Count occurrences of &gt; - should be 4
        QCOMPARE(result.count(QStringLiteral("&gt;")), 4);
    }

    // Multi-line XML matching the user's bug report (issue #3521):
    // Tags like <iconReference> and <description> must be escaped
    {
        QString input = QStringLiteral(
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<searchConnectorDescription "
            "xmlns=\"http://schemas.microsoft.com/windows/2009/searchConnector\">\n"
            "    <iconReference>\\\\user\\test</iconReference>\n"
            "    <description>Some description here.</description>\n"
            "</searchConnectorDescription>");
        QString result = c.process(input);
        qDebug() << "Multi-line XML highlighter output:" << result;
        // Every '<' must be escaped as &lt; (7 tags total:
        // <?xml, <searchConnector, <iconRef, </iconRef,
        // <description, </description, </searchConnector)
        QCOMPARE(result.count(QStringLiteral("&lt;")), 7);
        // Every '>' must be escaped as &gt; (7 closing brackets)
        QCOMPARE(result.count(QStringLiteral("&gt;")), 7);
        // None of the XML tag names should appear as unescaped HTML tags
        QVERIFY2(!result.contains(QStringLiteral("<iconReference>")),
                 "Raw <iconReference> tag must not appear in output");
        QVERIFY2(!result.contains(QStringLiteral("<description>")),
                 "Raw <description> tag must not appear in output");
    }

    // Full pipeline test: XML code block through note → HTML
    {
        QString md = QStringLiteral(
            "# Test\n```xml\n"
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<searchConnectorDescription "
            "xmlns=\"http://schemas.microsoft.com/windows/2009/searchConnector\">\n"
            "    <iconReference>\\\\user\\test</iconReference>\n"
            "    <description>Some description here.</description>\n"
            "</searchConnectorDescription>\n"
            "```\n");
        Note note;
        note.setNoteText(md);
        QString html = note.toMarkdownHtml("", 980, true);
        qDebug() << "Full pipeline XML HTML:" << html;
        // The output must NOT contain raw <child> or </child> tags
        QVERIFY2(!html.contains(QStringLiteral("<iconReference>")),
                 "Raw <iconReference> tag must not appear in final HTML");
        QVERIFY2(!html.contains(QStringLiteral("</iconReference>")),
                 "Raw </iconReference> tag must not appear in final HTML");
        QVERIFY2(!html.contains(QStringLiteral("<description>")),
                 "Raw <description> tag must not appear in final HTML");
    }
}

/**
 * Test that angle-bracket content like <stdio.h> inside various code block
 * types is NOT converted to a file link in the preview HTML.
 * See: https://github.com/pbek/QOwnNotes/issues/3084
 */
void TestNotes::testAngleBracketsInCodeBlocksNotConvertedToLinks() {
    // Test backtick-fenced code block
    {
        QString md = QStringLiteral("# Test\n```c\n#include <stdio.h>\n```\n");
        Note note;
        note.setNoteText(md);
        QString html = note.toMarkdownHtml("", 980, true);
        // Should NOT contain a file link for stdio.h
        QVERIFY(!html.contains(QStringLiteral("](file://")));
        QVERIFY(!html.contains(QStringLiteral("[stdio.h]")));
        // Should contain stdio.h as text (HTML-escaped angle brackets)
        QVERIFY(html.contains(QStringLiteral("stdio.h")));
    }

    // Test tilde-fenced code block
    {
        QString md = QStringLiteral("# Test\n~~~c\n#include <stdio.h>\n~~~\n");
        Note note;
        note.setNoteText(md);
        QString html = note.toMarkdownHtml("", 980, true);
        QVERIFY(!html.contains(QStringLiteral("](file://")));
        QVERIFY(!html.contains(QStringLiteral("[stdio.h]")));
        QVERIFY(html.contains(QStringLiteral("stdio.h")));
    }

    // Test inline code
    {
        QString md = QStringLiteral("# Test\nUse `#include <stdio.h>` in C.\n");
        Note note;
        note.setNoteText(md);
        QString html = note.toMarkdownHtml("", 980, true);
        QVERIFY(!html.contains(QStringLiteral("](file://")));
        QVERIFY(!html.contains(QStringLiteral("[stdio.h]")));
        QVERIFY(html.contains(QStringLiteral("stdio.h")));
    }

    // Test 4-space indented code block
    {
        QString md = QStringLiteral("# Test\n\n    #include <stdio.h>\n\nSome text.\n");
        Note note;
        note.setNoteText(md);
        QString html = note.toMarkdownHtml("", 980, true);
        QVERIFY(!html.contains(QStringLiteral("](file://")));
        QVERIFY(!html.contains(QStringLiteral("[stdio.h]")));
        QVERIFY(html.contains(QStringLiteral("stdio.h")));
    }

    // Test tab-indented code block
    {
        QString md = QStringLiteral("# Test\n\n\t#include <stdlib.h>\n\nSome text.\n");
        Note note;
        note.setNoteText(md);
        QString html = note.toMarkdownHtml("", 980, true);
        QVERIFY(!html.contains(QStringLiteral("](file://")));
        QVERIFY(!html.contains(QStringLiteral("[stdlib.h]")));
        QVERIFY(html.contains(QStringLiteral("stdlib.h")));
    }

    // Test that angle brackets OUTSIDE code blocks still get converted
    {
        QString md = QStringLiteral("# Test\nSee <test.md> for details.\n");
        Note note;
        note.setNoteText(md);
        QString html = note.toMarkdownHtml("", 980, true);
        // Outside code blocks, <test.md> should become a link
        QVERIFY(html.contains(QStringLiteral("test.md")));
    }
}

/**
 * Test that a backtick-fenced code block nested inside a tilde-fenced code
 * block does not result in raw HTML span tags being visible in the preview.
 * See: https://github.com/pbek/QOwnNotes/issues/2671
 */
void TestNotes::testBacktickBlockInsideTildeFenceNotDoubleHighlighted() {
    // A tilde fence containing a backtick-fenced ini block
    QString md = QStringLiteral("~~~\n```ini\ntest1=test2\n```\n~~~\n");

    Note note;
    note.setNoteText(md);
    QString html = note.toMarkdownHtml("", 980, true);
    // Raw HTML span tags must NOT appear as visible text in the output.
    // Before the fix, the backtick block was highlighted first (injecting spans),
    // then the outer tilde block processor escaped those spans, causing
    // "&lt;span class=\"code-keyword\"&gt;test1&lt;/span&gt;=test2" to show up literally.
    QVERIFY(!html.contains(QStringLiteral("&lt;span")));
    QVERIFY(!html.contains(QStringLiteral("&gt;test1&lt;")));

    // The raw content should still be present (as plain text, not wrapped in spans)
    QVERIFY(html.contains(QStringLiteral("test1")));
    QVERIFY(html.contains(QStringLiteral("test2")));
}

/**
 * Test that a backtick-fenced code block nested inside a 4-space/tab indented
 * code block does not result in raw HTML span tags being visible in the preview.
 * See: https://github.com/pbek/QOwnNotes/issues/2671
 */
void TestNotes::testBacktickBlockInsideIndentedCodeNotHighlighted() {
    // A tab-indented code block containing a backtick-fenced ini block
    QString md = QStringLiteral("\t```ini\n\ttest1=test2\n\t```\n");

    Note note;
    note.setNoteText(md);
    QString html = note.toMarkdownHtml("", 980, true);

    // The entire block is a 4-space/tab indented code block, so MD4C renders
    // it as a <pre><code> block. No highlighting should be injected — the
    // backtick fences are literal content, not Markdown code fences.
    // Before the fix, highlightCode would see the ```ini, run the INI
    // highlighter, and inject <span class="code-keyword"> into the output.
    QVERIFY2(!html.contains(QStringLiteral("<span class=\"code-keyword\">")),
             "Indented code block content must not contain injected highlight spans");

    // The raw content should be present
    QVERIFY(html.contains(QStringLiteral("test1")));
    QVERIFY(html.contains(QStringLiteral("test2")));
}

void TestNotes::testPercentEncodedFileUrlUsesDecodedLocalPath() {
    const QString filePath = notesPath + QStringLiteral("/attachments/Test File.pdf");
    const QString encodedFileUrl = QUrl::fromLocalFile(filePath).toString();
    const QUrl fileUrl = UrlHandler::localFileUrlForDesktopOpen(encodedFileUrl);
    QCOMPARE(fileUrl.toLocalFile(), filePath);
}

void TestNotes::testWikiLinkSupportDisabledLeavesPlainText() {
    SettingsService().setValue(QStringLiteral("Editor/wikiLinkSupport"), false);

    Note note;
    note.setNoteText(QStringLiteral("[[Disabled Link]]"));
    const QString html = note.toMarkdownHtml(QString(), 980, true);

    QVERIFY(!html.contains(QStringLiteral("wikilink:")));
    QVERIFY(!html.contains(QStringLiteral("class=\"wikilink\"")));
    QVERIFY(html.contains(QStringLiteral("[[Disabled Link]]")));
}

void TestNotes::testResolveWikiLinkPrefersCurrentSubfolderAndFindsNestedNotes() {
    SettingsService().setValue(QStringLiteral("Editor/wikiLinkSupport"), true);

    const QString sharedName = uniqueTestName(QStringLiteral("wiki-shared"));
    const QString nestedName = uniqueTestName(QStringLiteral("wiki-nested"));

    const NoteSubFolder currentFolder =
        createTestNoteSubFolder(uniqueTestName(QStringLiteral("current")));
    const NoteSubFolder nestedFolder =
        createTestNoteSubFolder(uniqueTestName(QStringLiteral("nested")), currentFolder.getId());

    const Note rootNote = createTestNote(sharedName);
    const Note currentNote = createTestNote(sharedName, currentFolder.getId());
    const Note nestedNote = createTestNote(nestedName, nestedFolder.getId());

    const Note resolvedShared = Note::resolveWikiLink(sharedName, currentFolder.getId());
    QVERIFY(resolvedShared.isFetched());
    QCOMPARE(resolvedShared.getId(), currentNote.getId());

    const Note resolvedNested = Note::resolveWikiLink(nestedName, currentFolder.getId());
    QVERIFY(resolvedNested.isFetched());
    QCOMPARE(resolvedNested.getId(), nestedNote.getId());

    const Note resolvedQualified = Note::resolveWikiLink(
        nestedFolder.getName() + QStringLiteral("/") + nestedName, currentFolder.getId());
    QVERIFY(resolvedQualified.isFetched());
    QCOMPARE(resolvedQualified.getId(), nestedNote.getId());

    QVERIFY(rootNote.isFetched());
}

void TestNotes::testWikiLinkHtmlRenderingMarksResolvedAndBrokenLinks() {
    SettingsService().setValue(QStringLiteral("Editor/wikiLinkSupport"), true);

    const QString resolvedName = uniqueTestName(QStringLiteral("wiki-html-resolved"));
    createTestNote(resolvedName);

    Note note;
    note.setNoteText(QStringLiteral("[[%1]] and [[Missing %2]]")
                         .arg(resolvedName, uniqueTestName(QStringLiteral("wiki-html-broken"))));

    const QString html = note.toMarkdownHtml(QString(), 980, true);

    QVERIFY(html.contains(QStringLiteral("<a class=\"wikilink\"")));
    QVERIFY(html.contains(QStringLiteral("href=\"wikilink:")));
    QVERIFY(html.contains(resolvedName));
    QVERIFY(html.contains(QStringLiteral("class=\"wikilink\"")));
    QVERIFY(html.contains(QStringLiteral("class=\"wikilink broken\"")));
}

void TestNotes::testQualifiedWikiLinksAreUpdatedOnSubfolderRename() {
    SettingsService().setValue(QStringLiteral("Editor/wikiLinkSupport"), true);

    const NoteSubFolder parentFolder =
        createTestNoteSubFolder(uniqueTestName(QStringLiteral("issue-3512-parent")));
    NoteSubFolder childFolder = createTestNoteSubFolder(
        uniqueTestName(QStringLiteral("issue-3512-child")), parentFolder.getId());
    const QString oldChildName = childFolder.getName();

    const QString targetName = uniqueTestName(QStringLiteral("issue-3512-target"));
    createTestNote(targetName, childFolder.getId());

    const QString sourceName = uniqueTestName(QStringLiteral("issue-3512-source"));
    const QString originalText =
        QStringLiteral("See [[%1/%2/%3]]").arg(parentFolder.getName(), oldChildName, targetName);
    const Note sourceNote = createTestNote(sourceName, 0, originalText);

    const QString newChildName = uniqueTestName(QStringLiteral("issue-3512-renamed"));
    QVERIFY(childFolder.rename(newChildName));

    const Note updatedSource = Note::fetch(sourceNote.getId());
    QVERIFY(updatedSource.isFetched());
    QVERIFY(updatedSource.getNoteText().contains(
        QStringLiteral("[[%1/%2/%3]]").arg(parentFolder.getName(), newChildName, targetName)));
    QVERIFY(!updatedSource.getNoteText().contains(
        QStringLiteral("[[%1/%2/%3]]").arg(parentFolder.getName(), oldChildName, targetName)));
}

void TestNotes::testBookmarkSuggestionsPrefixSubstringAndExact() {
    QVector<Bookmark> bookmarks = {
        Bookmark(QStringLiteral("https://example.com/home"), QStringLiteral("Homepage")),
        Bookmark(QStringLiteral("https://example.com/about-home"), QStringLiteral("About")),
        Bookmark(QStringLiteral("https://foo.bar"), QStringLiteral("Some Home Link"))};

    const QStringList suggestions =
        Bookmark::suggestionStrings(bookmarks, QStringLiteral("home"), 10);
    QVERIFY(!suggestions.isEmpty());
    QCOMPARE(suggestions.at(0), QStringLiteral("Homepage"));
    QVERIFY(suggestions.contains(QStringLiteral("Some Home Link")) ||
            suggestions.contains(QStringLiteral("https://example.com/home")));
}

void TestNotes::testBookmarkSuggestionsDeduplication() {
    QVector<Bookmark> bookmarks = {
        Bookmark(QStringLiteral("https://example.com/home"), QStringLiteral("Homepage")),
        Bookmark(QStringLiteral("https://example.com/home"), QStringLiteral("Homepage")),
        Bookmark(QStringLiteral("https://EXAMPLE.com/home"), QStringLiteral("homepage"))};

    const QStringList suggestions =
        Bookmark::suggestionStrings(bookmarks, QStringLiteral("home"), 10);
    int homepageCount = 0;
    int urlCount = 0;

    for (const QString &value : suggestions) {
        if (value.compare(QStringLiteral("Homepage"), Qt::CaseInsensitive) == 0) {
            homepageCount++;
        }
        if (value.compare(QStringLiteral("https://example.com/home"), Qt::CaseInsensitive) == 0) {
            urlCount++;
        }
    }

    QVERIFY(homepageCount <= 1);
    QVERIFY(urlCount <= 1);
}

void TestNotes::testBookmarkSuggestionsMultiTokenAndOrderIndependent() {
    QVector<Bookmark> bookmarks = {Bookmark(QStringLiteral("https://example.com/qownnotes-launch"),
                                            QStringLiteral("QOwnNotes launch guide")),
                                   Bookmark(QStringLiteral("https://example.com/launch-only"),
                                            QStringLiteral("Launch helper")),
                                   Bookmark(QStringLiteral("https://example.com/qownnotes-only"),
                                            QStringLiteral("QOwnNotes docs"))};

    const QStringList suggestionsForward =
        Bookmark::suggestionStrings(bookmarks, QStringLiteral("qow launch"), 10);
    QVERIFY(suggestionsForward.contains(QStringLiteral("QOwnNotes launch guide")) ||
            suggestionsForward.contains(QStringLiteral("https://example.com/qownnotes-launch")));

    const QStringList suggestionsReverse =
        Bookmark::suggestionStrings(bookmarks, QStringLiteral("launch qow"), 10);
    QVERIFY(suggestionsReverse.contains(QStringLiteral("QOwnNotes launch guide")) ||
            suggestionsReverse.contains(QStringLiteral("https://example.com/qownnotes-launch")));

    QVERIFY(!suggestionsForward.contains(QStringLiteral("Launch helper")));
    QVERIFY(!suggestionsForward.contains(QStringLiteral("QOwnNotes docs")));
}

void TestNotes::testBookmarkSuggestionsKeepNameUrlPairing() {
    QVector<Bookmark> bookmarks = {
        Bookmark(QStringLiteral("https://github.com/digital-blueprint/cabinet-app"),
                 QStringLiteral("Cabinet Frontend UI Wireframe")),
        Bookmark(QStringLiteral("https://github.com/digital-blueprint/relay-cabinet-bundle"),
                 QStringLiteral("System Architecture Diagram for the Cabinet Bundle"))};

    const QStringList suggestions =
        Bookmark::suggestionStrings(bookmarks, QStringLiteral("cabi"), 20);

    const int cabinetNameIndex =
        suggestions.indexOf(QStringLiteral("Cabinet Frontend UI Wireframe"));
    const int cabinetUrlIndex =
        suggestions.indexOf(QStringLiteral("https://github.com/digital-blueprint/cabinet-app"));
    QVERIFY(cabinetNameIndex >= 0);
    QVERIFY(cabinetUrlIndex > cabinetNameIndex);

    const int bundleNameIndex =
        suggestions.indexOf(QStringLiteral("System Architecture Diagram for the Cabinet Bundle"));
    const int bundleUrlIndex = suggestions.indexOf(
        QStringLiteral("https://github.com/digital-blueprint/relay-cabinet-bundle"));
    QVERIFY(bundleNameIndex >= 0);
    QVERIFY(bundleUrlIndex > bundleNameIndex);
}

void TestNotes::testBookmarkSuggestionsIncludeMarkdownMetadata() {
    const QString bookmarkText = QStringLiteral(
        "* [TU Zeiterfassung Stundenaufzeichnung / Urlaub / Krankenstanderfassung / "
        "Homeoffice / Gehalt / Lohnzettel](https://sap.tugraz.at/) #tu time "
        "(IIIB, Gehaltsstufe: 01) Bestellservice");
    const QVector<Bookmark> bookmarks = Bookmark::parseBookmarks(bookmarkText, true);
    QVERIFY(!bookmarks.isEmpty());

    const QStringList urlaubSuggestions =
        Bookmark::suggestionStrings(bookmarks, QStringLiteral("Urlaub"), 10);
    QVERIFY(urlaubSuggestions.contains(
        QStringLiteral("TU Zeiterfassung Stundenaufzeichnung / Urlaub / Krankenstanderfassung / "
                       "Homeoffice / Gehalt / Lohnzettel")));

    const QStringList gehaltsstufeSuggestions =
        Bookmark::suggestionStrings(bookmarks, QStringLiteral("Gehaltsstufe"), 10);
    QVERIFY(gehaltsstufeSuggestions.contains(
        QStringLiteral("TU Zeiterfassung Stundenaufzeichnung / Urlaub / Krankenstanderfassung / "
                       "Homeoffice / Gehalt / Lohnzettel")));
}

void TestNotes::testBookmarkSuggestionsEmptyQuery() {
    QVector<Bookmark> bookmarks = {
        Bookmark(QStringLiteral("https://example.com/home"), QStringLiteral("Homepage"))};

    QVERIFY(Bookmark::suggestionStrings(bookmarks, QStringLiteral("   "), 10).isEmpty());
}

void TestNotes::testBookmarkSuggestionsLimitHandling() {
    QVector<Bookmark> bookmarks;
    for (int i = 0; i < 40; ++i) {
        bookmarks.append(Bookmark(QStringLiteral("https://example.com/home-%1").arg(i),
                                  QStringLiteral("home-%1").arg(i)));
    }

    QCOMPARE(Bookmark::suggestionStrings(bookmarks, QStringLiteral("home"), 3).count(), 3);
    QCOMPARE(Bookmark::suggestionStrings(bookmarks, QStringLiteral("home"), 100).count(), 50);
}

void TestNotes::testBookmarkSuggestionsResponseShape() {
    const QString query = QStringLiteral("home");
    const QStringList suggestions = {QStringLiteral("suggestion 1"),
                                     QStringLiteral("suggestion 2")};
    const QJsonDocument doc = Bookmark::homepageSuggestionResponseJson(query, suggestions);
    QVERIFY(doc.isArray());

    const QJsonArray root = doc.array();
    QCOMPARE(root.count(), 2);
    QCOMPARE(root.at(0).toString(), query);
    QVERIFY(root.at(1).isArray());

    const QJsonArray suggestionArray = root.at(1).toArray();
    QCOMPARE(suggestionArray.count(), 2);
    QCOMPARE(suggestionArray.at(0).toString(), QStringLiteral("suggestion 1"));
    QCOMPARE(suggestionArray.at(1).toString(), QStringLiteral("suggestion 2"));
}

// QTEST_MAIN(TestNotes)
