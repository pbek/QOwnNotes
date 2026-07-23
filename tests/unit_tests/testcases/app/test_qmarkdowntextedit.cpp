#include "test_qmarkdowntextedit.h"

#include <QApplication>
#include <QTextBlock>
#include <QTextLayout>
#include <QtTest>

#include "libraries/qmarkdowntextedit/qmarkdowntextedit.h"

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
