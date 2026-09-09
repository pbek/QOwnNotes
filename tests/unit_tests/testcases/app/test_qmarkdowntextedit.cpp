#include "test_qmarkdowntextedit.h"

#include <QApplication>
#include <QTextBlock>
#include <QTextLayout>
#include <QtTest>

#include "libraries/qmarkdowntextedit/qmarkdowntextedit.h"
#include "utils/gui.h"

namespace {
QTextCharFormat formatAt(const QTextBlock &block, int position) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    const auto formats = block.layout()->additionalFormats();
#else
    const auto formats = block.layout()->formats();
#endif
    for (const QTextLayout::FormatRange &range : formats) {
        if (position >= range.start && position < range.start + range.length) {
            return range.format;
        }
    }

    return {};
}
}    // namespace

void TestQMarkdownTextEdit::testUpFromStartOfSecondLineMovesToFirstLine() {
    QMarkdownTextEdit editor;
    editor.resize(400, 200);
    editor.setPlainText(QStringLiteral("first line\nsecond line"));
    editor.show();
    editor.setFocus();
    QApplication::processEvents();

    QTextCursor cursor = editor.textCursor();
    cursor.setPosition(editor.document()->findBlockByNumber(1).position());
    editor.setTextCursor(cursor);

    QTest::keyClick(&editor, Qt::Key_Up);

    QCOMPARE(editor.textCursor().position(), 0);
}

void TestQMarkdownTextEdit::testToPlainTextPreservesNoBreakSpaces() {
    QMarkdownTextEdit editor;
    const QString text = QStringLiteral("U+00A0") + QChar(0x00A0) + QStringLiteral("NBSP\n") +
                         QStringLiteral("U+202F") + QChar(0x202F) + QStringLiteral("NNBSP");

    editor.setPlainText(text);

    QCOMPARE(editor.toPlainText(), text);
}

void TestQMarkdownTextEdit::testRCodeBlockHighlighting() {
    QMarkdownTextEdit editor;
    editor.setPlainText(QStringLiteral("```r\nif (is.numeric(value)) print(TRUE) # result\n```"));

    const QTextBlock codeBlock = editor.document()->findBlockByNumber(1);
    QCOMPARE(editor.document()->firstBlock().userState(), int(MarkdownHighlighter::CodeR));
    QCOMPARE(codeBlock.userState(), int(MarkdownHighlighter::CodeR));

    const QTextCharFormat defaultFormat = formatAt(codeBlock, codeBlock.text().indexOf("value"));
    const QTextCharFormat keywordFormat = formatAt(codeBlock, codeBlock.text().indexOf("if"));
    const QTextCharFormat builtinFormat =
        formatAt(codeBlock, codeBlock.text().indexOf("is.numeric"));
    const QTextCharFormat literalFormat = formatAt(codeBlock, codeBlock.text().indexOf("TRUE"));
    const int commentPosition = codeBlock.text().indexOf('#');
    const QTextCharFormat commentFormat = formatAt(codeBlock, commentPosition);

    QVERIFY(keywordFormat.foreground() != defaultFormat.foreground());
    QVERIFY(builtinFormat.foreground() != defaultFormat.foreground());
    QVERIFY(literalFormat.foreground() != defaultFormat.foreground());
    QVERIFY(commentFormat.foreground() != defaultFormat.foreground());
    QCOMPARE(formatAt(codeBlock, commentPosition + 2).foreground(), commentFormat.foreground());
}

void TestQMarkdownTextEdit::testCodeBlockLanguageAliases_data() {
    QTest::addColumn<QString>("language");
    QTest::addColumn<int>("state");

    const auto addAliases = [](MarkdownHighlighter::HighlighterState state,
                               const QStringList &aliases) {
        for (const QString &alias : aliases) {
            QTest::newRow(qPrintable(alias)) << alias << int(state);
        }
    };

    addAliases(MarkdownHighlighter::CodeBash, {QStringLiteral("bash"), QStringLiteral("sh"),
                                               QStringLiteral("shell"), QStringLiteral("zsh")});
    addAliases(MarkdownHighlighter::CodeC, {QStringLiteral("c"), QStringLiteral("h")});
    addAliases(MarkdownHighlighter::CodeCpp,
               {QStringLiteral("c++"), QStringLiteral("cc"), QStringLiteral("cpp"),
                QStringLiteral("cxx"), QStringLiteral("hpp"), QStringLiteral("hxx")});
    addAliases(MarkdownHighlighter::CodeCSharp,
               {QStringLiteral("c#"), QStringLiteral("cs"), QStringLiteral("csharp")});
    addAliases(
        MarkdownHighlighter::CodeConsole,
        {QStringLiteral("console"), QStringLiteral("shell-session"), QStringLiteral("terminal")});
    addAliases(MarkdownHighlighter::CodeGo, {QStringLiteral("go"), QStringLiteral("golang")});
    addAliases(MarkdownHighlighter::CodeXML, {QStringLiteral("html"), QStringLiteral("svg"),
                                              QStringLiteral("xhtml"), QStringLiteral("xml")});
    addAliases(MarkdownHighlighter::CodeINI, {QStringLiteral("cfg"), QStringLiteral("ini")});
    addAliases(MarkdownHighlighter::CodeJs, {QStringLiteral("javascript"), QStringLiteral("js"),
                                             QStringLiteral("jsx"), QStringLiteral("node")});
    addAliases(MarkdownHighlighter::CodeJSON, {QStringLiteral("json"), QStringLiteral("jsonc")});
    addAliases(MarkdownHighlighter::CodeMake, {QStringLiteral("make"), QStringLiteral("makefile")});
    addAliases(MarkdownHighlighter::CodePython,
               {QStringLiteral("py"), QStringLiteral("python"), QStringLiteral("python3")});
    addAliases(MarkdownHighlighter::CodeRust, {QStringLiteral("rs"), QStringLiteral("rust")});
    addAliases(MarkdownHighlighter::CodeSystemVerilog,
               {QStringLiteral("sv"), QStringLiteral("systemverilog")});
    addAliases(MarkdownHighlighter::CodeTypeScript,
               {QStringLiteral("ts"), QStringLiteral("tsx"), QStringLiteral("typescript")});
}

void TestQMarkdownTextEdit::testCodeBlockLanguageAliases() {
    QFETCH(QString, language);
    QFETCH(int, state);

    QMarkdownTextEdit editor;
    editor.setPlainText(QStringLiteral("```") + language + QStringLiteral("\nvalue\n```"));

    QCOMPARE(editor.document()->firstBlock().userState(), state);
    QCOMPARE(editor.document()->findBlockByNumber(1).userState(), state);
}

void TestQMarkdownTextEdit::testCodeBlockInfoStrings() {
    QMarkdownTextEdit editor;
    editor.setPlainText(QStringLiteral("```JavaScript title=app.js\nconst value = true;\n```"));
    QCOMPARE(editor.document()->firstBlock().userState(), int(MarkdownHighlighter::CodeJs));

    editor.setPlainText(QStringLiteral("```{.python #example}\nprint(True)\n```"));
    QCOMPARE(editor.document()->firstBlock().userState(), int(MarkdownHighlighter::CodePython));
}

void TestQMarkdownTextEdit::testTildeSystemVerilogHighlighting() {
    QMarkdownTextEdit editor;
    editor.setPlainText(QStringLiteral("~~~sv\nmodule counter;\n~~~"));

    const QTextBlock codeBlock = editor.document()->findBlockByNumber(1);
    QCOMPARE(codeBlock.userState(), int(MarkdownHighlighter::CodeSystemVerilog) + 300);
    QVERIFY(formatAt(codeBlock, 0).foreground() !=
            formatAt(codeBlock, codeBlock.text().indexOf("counter")).foreground());
}

void TestQMarkdownTextEdit::testSqlCodeBlockHighlighting() {
    QMarkdownTextEdit editor;
    editor.setPlainText(QStringLiteral(
        "```sql\nSELECT total - discount AS value;\nSELECT '-- text'; -- comment\n```"));

    const QTextBlock expressionBlock = editor.document()->findBlockByNumber(1);
    const QTextCharFormat defaultFormat =
        formatAt(expressionBlock, expressionBlock.text().indexOf("discount"));
    QCOMPARE(formatAt(expressionBlock, expressionBlock.text().indexOf('-')).foreground(),
             defaultFormat.foreground());

    const QTextBlock stringBlock = editor.document()->findBlockByNumber(2);
    const int stringDash = stringBlock.text().indexOf('-');
    const int commentDash = stringBlock.text().lastIndexOf(QLatin1String("--"));
    QVERIFY(formatAt(stringBlock, stringDash).foreground() !=
            formatAt(stringBlock, commentDash).foreground());
    QCOMPARE(formatAt(stringBlock, commentDash).foreground(),
             formatAt(stringBlock, commentDash + 3).foreground());
}

void TestQMarkdownTextEdit::testForthCommentHighlighting() {
    QMarkdownTextEdit editor;
    editor.setPlainText(QStringLiteral("```forth\n1 2 + \\ comment\n( stack effect ) DUP\n```"));

    const QTextBlock lineComment = editor.document()->findBlockByNumber(1);
    const int slash = lineComment.text().indexOf(QLatin1Char('\\'));
    QVERIFY(formatAt(lineComment, slash).foreground() != formatAt(lineComment, 0).foreground());
    QCOMPARE(formatAt(lineComment, slash).foreground(),
             formatAt(lineComment, lineComment.text().length() - 1).foreground());

    const QTextBlock parenComment = editor.document()->findBlockByNumber(2);
    const int closingParen = parenComment.text().indexOf(QLatin1Char(')'));
    QCOMPARE(formatAt(parenComment, 0).foreground(),
             formatAt(parenComment, closingParen).foreground());
    QVERIFY(formatAt(parenComment, closingParen).foreground() !=
            formatAt(parenComment, parenComment.text().indexOf("DUP")).foreground());
}

void TestQMarkdownTextEdit::testMultilineInlineHighlighting() {
    QMarkdownTextEdit editor;

    editor.setPlainText(QStringLiteral("~~first line\nsecond line\nthird line~~"));
    editor.highlighter()->rehighlight();
    QVERIFY(formatAt(editor.document()->findBlockByNumber(0), 2).fontStrikeOut());
    QVERIFY(formatAt(editor.document()->findBlockByNumber(1), 0).fontStrikeOut());
    QVERIFY(formatAt(editor.document()->findBlockByNumber(2), 0).fontStrikeOut());

    editor.setPlainText(QStringLiteral("**first line\nsecond line**"));
    editor.highlighter()->rehighlight();
    QCOMPARE(formatAt(editor.document()->findBlockByNumber(0), 2).fontWeight(), int(QFont::Bold));
    QCOMPARE(formatAt(editor.document()->findBlockByNumber(1), 0).fontWeight(), int(QFont::Bold));

    editor.setPlainText(QStringLiteral("*first line\nsecond line\nthird line*"));
    editor.highlighter()->rehighlight();
    QVERIFY(formatAt(editor.document()->findBlockByNumber(0), 1).fontItalic());
    QVERIFY(formatAt(editor.document()->findBlockByNumber(1), 0).fontItalic());
    QVERIFY(formatAt(editor.document()->findBlockByNumber(2), 0).fontItalic());

    editor.highlighter()->setHighlightingOptions(MarkdownHighlighter::Underline);
    editor.setPlainText(QStringLiteral("__first line\nsecond line__"));
    editor.highlighter()->rehighlight();
    QVERIFY(formatAt(editor.document()->findBlockByNumber(0), 2).fontUnderline());
    QVERIFY(formatAt(editor.document()->findBlockByNumber(1), 0).fontUnderline());

    editor.setPlainText(QStringLiteral("#1 | ABC [*1]\n#2 | XYZ [*2]\n#3 | XXX [*3]"));
    editor.highlighter()->rehighlight();
    QVERIFY(!formatAt(editor.document()->findBlockByNumber(0), 11).fontItalic());
    QVERIFY(!formatAt(editor.document()->findBlockByNumber(1), 0).fontItalic());
    QVERIFY(!formatAt(editor.document()->findBlockByNumber(2), 0).fontItalic());

    editor.setPlainText(
        QStringLiteral("!!_1) Text\ncontinued\n\n__Test__Broken - because of a missing space after "
                       "the closing \"__\" !!!\ncontinued"));
    editor.highlighter()->rehighlight();
    QVERIFY(!formatAt(editor.document()->findBlockByNumber(0), 3).fontItalic());
    QVERIFY(!formatAt(editor.document()->findBlockByNumber(1), 0).fontItalic());
    QVERIFY(!formatAt(editor.document()->findBlockByNumber(3), 2).fontUnderline());
    QVERIFY(!formatAt(editor.document()->findBlockByNumber(3), 8).fontUnderline());
    QVERIFY(formatAt(editor.document()->findBlockByNumber(4), 0).fontWeight() != int(QFont::Bold));

    editor.setPlainText(QStringLiteral("__foo_bar__"));
    editor.highlighter()->rehighlight();
    QVERIFY(formatAt(editor.document()->firstBlock(), 2).fontUnderline());
    QVERIFY(formatAt(editor.document()->firstBlock(), 6).fontUnderline());
}

void TestQMarkdownTextEdit::testMultilineInlineHighlightingDisabled() {
    QMarkdownTextEdit editor;
    editor.highlighter()->setMultilineInlineHighlightingEnabled(false);

    editor.setPlainText(QStringLiteral("~~first line\nsecond line\nthird line~~"));
    editor.highlighter()->rehighlight();
    QVERIFY(!formatAt(editor.document()->findBlockByNumber(1), 0).fontStrikeOut());
    QVERIFY(!formatAt(editor.document()->findBlockByNumber(2), 0).fontStrikeOut());

    editor.setPlainText(QStringLiteral("**first line\nsecond line**"));
    editor.highlighter()->rehighlight();
    QVERIFY(formatAt(editor.document()->findBlockByNumber(1), 0).fontWeight() != int(QFont::Bold));

    editor.setPlainText(QStringLiteral("*first line\nsecond line\nthird line*"));
    editor.highlighter()->rehighlight();
    QVERIFY(!formatAt(editor.document()->findBlockByNumber(1), 0).fontItalic());
    QVERIFY(!formatAt(editor.document()->findBlockByNumber(2), 0).fontItalic());

    // Turning it back on re-enables multi-line highlighting
    editor.highlighter()->setMultilineInlineHighlightingEnabled(true);
    QVERIFY(formatAt(editor.document()->findBlockByNumber(1), 0).fontItalic());
    QVERIFY(formatAt(editor.document()->findBlockByNumber(2), 0).fontItalic());
}

void TestQMarkdownTextEdit::testWhitespaceMarkerHighlighting() {
    QCOMPARE(int(MarkdownHighlighter::LinkInternal), 34);
    QCOMPARE(int(MarkdownHighlighter::Whitespace), 35);

    QMarkdownTextEdit editor;
    const QColor markerColor(QStringLiteral("#789abc"));
    editor.setPlainText(QStringLiteral("# heading\ttext\u00a0\nplain  "));
    editor.highlighter()->rehighlight();

    const QTextBlock block = editor.document()->firstBlock();
    const int firstSpace = block.text().indexOf(QLatin1Char(' '));
    const int tab = block.text().indexOf(QLatin1Char('\t'));
    const int noBreakSpace = block.text().indexOf(QChar(0x00a0));
    const QTextBlock trailingBlock = block.next();
    const int trailingSpace = trailingBlock.text().lastIndexOf(QStringLiteral("  "));
    const QTextCharFormat headingSpaceFormat = formatAt(block, firstSpace);
    const QTextCharFormat trailingSpaceFormat = formatAt(trailingBlock, trailingSpace);

    editor.highlighter()->setWhitespaceMarkerHighlighting(true, markerColor);

    QCOMPARE(formatAt(block, firstSpace).foreground().color(), markerColor);
    QCOMPARE(formatAt(block, tab).foreground().color(), markerColor);
    QCOMPARE(formatAt(trailingBlock, trailingSpace).foreground().color(), markerColor);
    QCOMPARE(formatAt(trailingBlock, trailingSpace).background(), trailingSpaceFormat.background());
    QCOMPARE(formatAt(block, firstSpace).fontWeight(), headingSpaceFormat.fontWeight());
    QVERIFY(formatAt(block, noBreakSpace).foreground().color() != markerColor);

    editor.setHighlightingEnabled(false);
    QCOMPARE(formatAt(block, firstSpace).foreground().color(), markerColor);
    QVERIFY(formatAt(block, firstSpace).fontWeight() != int(QFont::Bold));

    editor.highlighter()->setWhitespaceMarkerHighlighting(false, markerColor);
    QVERIFY(formatAt(block, firstSpace).foreground().color() != markerColor);
}

void TestQMarkdownTextEdit::testLinkedCheckBoxDetectionInReadOnlyEditor() {
    QMarkdownTextEdit editor;
    editor.setPlainText(
        QStringLiteral("- [ ] [Update contact information](https://example.com/tasks/123)"));
    editor.setReadOnly(true);

    QTextCursor cursor = editor.textCursor();
    cursor.setPosition(3);
    editor.setTextCursor(cursor);

    QVERIFY(Utils::Gui::isCheckBoxAtCursor(&editor));
    QVERIFY(editor.toPlainText().startsWith(QStringLiteral("- [ ]")));

    editor.setReadOnly(false);
    QVERIFY(Utils::Gui::toggleCheckBoxAtCursor(&editor));
    QVERIFY(editor.toPlainText().startsWith(QStringLiteral("- [x]")));
}

void TestQMarkdownTextEdit::testPercentEncodedAttachmentLinkDetection() {
    QMarkdownTextEdit editor;
    const QString label = QStringLiteral("Task 74776: Bulletin authorization plan.md");
    const QString destination =
        QStringLiteral("attachments/Task%2074776%3A%20Bulletin%20authorization%20plan.md");
    const QString markdown =
        QStringLiteral("[") + label + QStringLiteral("](") + destination + QStringLiteral(")");

    QCOMPARE(editor.getMarkdownUrlAtPosition(markdown, 1), destination);
    QCOMPARE(editor.getMarkdownUrlAtPosition(markdown, markdown.indexOf(destination)), destination);
}

void TestQMarkdownTextEdit::testFootnoteNavigationAndHighlighting() {
    QMarkdownTextEdit editor;
    editor.setPlainText(QStringLiteral("Text[^source]\n\n[^source]: Explanation"));
    editor.highlighter()->rehighlight();

    const QTextBlock referenceBlock = editor.document()->firstBlock();
    QVERIFY(formatAt(referenceBlock, referenceBlock.text().indexOf(QStringLiteral("[^source]")))
                .fontUnderline());

    QTextCursor cursor = editor.textCursor();
    cursor.setPosition(referenceBlock.position() +
                       referenceBlock.text().indexOf(QStringLiteral("source")));
    editor.setTextCursor(cursor);
    QVERIFY(editor.openLinkAtCursorPosition());
    QCOMPARE(editor.textCursor().blockNumber(), 2);

    cursor = editor.textCursor();
    cursor.setPosition(cursor.block().position() + 2);
    editor.setTextCursor(cursor);
    QVERIFY(editor.openLinkAtCursorPosition());
    QCOMPARE(editor.textCursor().blockNumber(), 0);
}

void TestQMarkdownTextEdit::testAutoFormatTableAtCursor() {
    QMarkdownTextEdit editor;
    editor.setPlainText(QStringLiteral("| Name|Value|\n|-|-|\n|one|123|\n|longer|4|"));

    QTextCursor cursor = editor.textCursor();
    cursor.setPosition(editor.document()->findBlockByNumber(2).position());
    editor.setTextCursor(cursor);

    QVERIFY(Utils::Gui::autoFormatTableAtCursor(&editor));
    QCOMPARE(editor.toPlainText(), QStringLiteral("| Name   | Value |\n"
                                                  "| ------ | ----- |\n"
                                                  "| one    | 123   |\n"
                                                  "| longer | 4     |"));
    QVERIFY(!Utils::Gui::autoFormatTableAtCursor(&editor));
}
