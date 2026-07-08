#include "test_qmarkdowntextedit.h"

#include <QApplication>
#include <QTextBlock>
#include <QtTest>

#include "libraries/qmarkdowntextedit/qmarkdowntextedit.h"

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
