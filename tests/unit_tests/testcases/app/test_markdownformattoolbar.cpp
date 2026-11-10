#include "test_markdownformattoolbar.h"

#include <QAction>
#include <QDialog>
#include <QInputDialog>
#include <QMenu>
#include <QPlainTextEdit>
#include <QShortcut>
#include <QTest>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "utils/markdownformatting.h"
#include "widgets/markdownformattoolbar.h"

namespace {
QAction *formatAction(MarkdownFormatToolbar &toolbar, const QString &name) {
    return toolbar.findChild<QAction *>(QStringLiteral("markdownFormat_") + name);
}

void selectText(QPlainTextEdit &editor, int start, int end) {
    QTextCursor cursor = editor.textCursor();
    cursor.setPosition(start);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    editor.setTextCursor(cursor);
}
}    // namespace

void TestMarkdownFormatToolbar::testInlineFormatting_data() {
    QTest::addColumn<QString>("actionName");
    QTest::addColumn<QString>("delimiter");
    QTest::newRow("bold") << QStringLiteral("bold") << QStringLiteral("**");
    QTest::newRow("italic") << QStringLiteral("italic") << QStringLiteral("*");
    QTest::newRow("strike") << QStringLiteral("strikeOut") << QStringLiteral("~~");
    QTest::newRow("code") << QStringLiteral("inlineCode") << QStringLiteral("`");
}

void TestMarkdownFormatToolbar::testInlineFormatting() {
    QFETCH(QString, actionName);
    QFETCH(QString, delimiter);
    QPlainTextEdit editor;
    MarkdownFormatToolbar toolbar(&editor);
    auto *action = formatAction(toolbar, actionName);
    QVERIFY(action);
    editor.setPlainText(QStringLiteral("hello"));
    editor.selectAll();
    action->trigger();
    QCOMPARE(editor.toPlainText(), delimiter + QStringLiteral("hello") + delimiter);
    QCOMPARE(editor.textCursor().selectedText(), QStringLiteral("hello"));
    action->trigger();
    QCOMPARE(editor.toPlainText(), QStringLiteral("hello"));
    editor.undo();
    QCOMPARE(editor.toPlainText(), delimiter + QStringLiteral("hello") + delimiter);
    editor.undo();
    QCOMPARE(editor.toPlainText(), QStringLiteral("hello"));

    editor.clear();
    action->trigger();
    QCOMPARE(editor.toPlainText(), delimiter.repeated(2));
    QCOMPARE(editor.textCursor().position(), delimiter.size());
    action->trigger();
    QCOMPARE(editor.toPlainText(), QString());
}

void TestMarkdownFormatToolbar::testWhitespaceAndMultiline() {
    QPlainTextEdit editor;
    editor.setPlainText(QStringLiteral(" hello \nnext\nlast"));
    selectText(editor, 0, 8);
    Utils::MarkdownFormatting::applyFormatter(&editor, QStringLiteral("**"));
    QCOMPARE(editor.toPlainText(), QStringLiteral(" **hello** \nnext\nlast"));
    editor.undo();
    QCOMPARE(editor.toPlainText(), QStringLiteral(" hello \nnext\nlast"));
    selectText(editor, 0, 12);
    Utils::MarkdownFormatting::applyFormatter(&editor, QStringLiteral("**"), true);
    QCOMPARE(editor.toPlainText(), QStringLiteral(" **hello \nnext**\nlast"));
    editor.undo();
    QCOMPARE(editor.toPlainText(), QStringLiteral(" hello \nnext\nlast"));
}

void TestMarkdownFormatToolbar::testLists_data() {
    QTest::addColumn<QString>("actionName");
    QTest::addColumn<QString>("expected");
    QTest::addColumn<QString>("emptyPrefix");
    QTest::newRow("bullet") << QStringLiteral("unorderedList")
                            << QStringLiteral("- first\n- second\nthird") << QStringLiteral("- ");
    QTest::newRow("numbered") << QStringLiteral("orderedList")
                              << QStringLiteral("1. first\n2. second\nthird")
                              << QStringLiteral("1. ");
    QTest::newRow("checkbox") << QStringLiteral("checkboxList")
                              << QStringLiteral("- [ ] first\n- [ ] second\nthird")
                              << QStringLiteral("- [ ] ");
}

void TestMarkdownFormatToolbar::testLists() {
    QFETCH(QString, actionName);
    QFETCH(QString, expected);
    QFETCH(QString, emptyPrefix);
    QPlainTextEdit editor;
    MarkdownFormatToolbar toolbar(&editor);
    auto *action = formatAction(toolbar, actionName);
    QVERIFY(action);
    editor.setPlainText(QStringLiteral("first\nsecond\nthird"));
    // Include the second newline, but not the third line.
    selectText(editor, 2, 13);
    action->trigger();
    QCOMPARE(editor.toPlainText(), expected);
    editor.undo();
    QCOMPARE(editor.toPlainText(), QStringLiteral("first\nsecond\nthird"));
    editor.clear();
    action->trigger();
    QCOMPARE(editor.toPlainText(), emptyPrefix);
    QCOMPARE(editor.textCursor().position(), emptyPrefix.size());
    QVERIFY(!editor.textCursor().hasSelection());
}

void TestMarkdownFormatToolbar::testCodeAndLinks() {
    QPlainTextEdit editor;
    MarkdownFormatToolbar toolbar(&editor);
    formatAction(toolbar, QStringLiteral("codeBlock"))->trigger();
    QCOMPARE(editor.toPlainText(), QStringLiteral("```\n\n```"));
    QCOMPARE(editor.textCursor().position(), 4);
    editor.setPlainText(QStringLiteral("one\ntwo"));
    editor.selectAll();
    formatAction(toolbar, QStringLiteral("codeBlock"))->trigger();
    QCOMPARE(editor.toPlainText(), QStringLiteral("```\none\ntwo\n```"));
    editor.undo();
    QCOMPARE(editor.toPlainText(), QStringLiteral("one\ntwo"));
    editor.setPlainText(QStringLiteral("abc"));
    selectText(editor, 1, 2);
    formatAction(toolbar, QStringLiteral("codeBlock"))->trigger();
    QCOMPARE(editor.toPlainText(), QStringLiteral("a\n```\nb\n```\nc"));
    editor.undo();
    QCOMPARE(editor.toPlainText(), QStringLiteral("abc"));

    auto *quote = formatAction(toolbar, QStringLiteral("blockQuote"));
    QVERIFY(quote);
    editor.setPlainText(QStringLiteral("one\n\ntwo\nthree"));
    selectText(editor, 0, 8);
    quote->trigger();
    QCOMPARE(editor.toPlainText(), QStringLiteral("> one\n\n> two\nthree"));
    quote->trigger();
    QCOMPARE(editor.toPlainText(), QStringLiteral("one\n\ntwo\nthree"));
    editor.clear();
    quote->trigger();
    QCOMPARE(editor.toPlainText(), QStringLiteral("> "));
    QCOMPARE(editor.textCursor().position(), 2);

    editor.setPlainText(QStringLiteral("label"));
    editor.selectAll();
    Utils::MarkdownFormatting::insertLink(&editor, QStringLiteral("https://example.org/a(b) c"),
                                          QStringLiteral("label"));
    QCOMPARE(editor.toPlainText(), QStringLiteral("[label](https://example.org/a%28b%29%20c)"));
    QCOMPARE(editor.textCursor().selectedText(), QStringLiteral("label"));
    editor.undo();
    QCOMPARE(editor.toPlainText(), QStringLiteral("label"));
    editor.selectAll();
    Utils::MarkdownFormatting::insertLink(&editor, QString(), QStringLiteral("label"));
    QCOMPARE(editor.toPlainText(), QStringLiteral("label"));
}

void TestMarkdownFormatToolbar::testActionStateAndMenu() {
    QPlainTextEdit editor;
    MarkdownFormatToolbar toolbar(&editor);
    const auto actions = MarkdownFormatToolbar::formattingActions(&editor);
    QCOMPARE(actions.size(), 10);
    QMenu menu;
    MarkdownFormatToolbar::addFormattingMenu(&menu, &editor);
    auto *formatMenu = menu.findChild<QMenu *>(QStringLiteral("markdownFormatMenu"));
    QVERIFY(formatMenu);
    QList<QAction *> menuActions;
    int separators = 0;
    for (auto *action : formatMenu->actions()) {
        if (action->isSeparator()) {
            ++separators;
        } else {
            menuActions.append(action);
        }
    }
    QCOMPARE(menuActions, actions);
    QCOMPARE(separators, 3);
    auto *help = toolbar.findChild<QAction *>(QStringLiteral("markdownFormatHelp"));
    QVERIFY(help);
    QVERIFY(!actions.contains(help));
    for (auto *action : actions) {
        QCOMPARE(action->shortcutContext(), Qt::WidgetShortcut);
    }
    editor.setReadOnly(true);
    for (auto *action : actions) {
        QVERIFY(!action->isEnabled());
    }
    Utils::MarkdownFormatting::applyFormatter(&editor, QStringLiteral("**"));
    Utils::MarkdownFormatting::insertCodeBlock(&editor);
    Utils::MarkdownFormatting::insertLink(&editor, QStringLiteral("https://example.org"),
                                          QStringLiteral("link"));
    QCOMPARE(editor.toPlainText(), QString());
    editor.setReadOnly(false);
    QVERIFY(actions.constFirst()->isEnabled());
    editor.setEnabled(false);
    QVERIFY(!actions.constFirst()->isEnabled());
    editor.setEnabled(true);
    QVERIFY(actions.constFirst()->isEnabled());
}

void TestMarkdownFormatToolbar::testShortcutsAndFocus() {
    QDialog dialog;
    int insertCalls = 0;
    auto *insertShortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+I")), &dialog);
    connect(insertShortcut, &QShortcut::activated, &dialog, [&insertCalls]() { ++insertCalls; });
    auto *layout = new QVBoxLayout(&dialog);
    auto *otherEditor = new QPlainTextEdit(&dialog);
    auto *editor = new QPlainTextEdit(&dialog);
    auto *toolbar = new MarkdownFormatToolbar(editor, &dialog);
    layout->addWidget(otherEditor);
    layout->addWidget(toolbar);
    layout->addWidget(editor);
    otherEditor->setPlainText(QStringLiteral("untouched"));
    editor->setPlainText(QStringLiteral("description"));
    dialog.show();
    dialog.activateWindow();
    editor->setFocus();
    QTRY_VERIFY(editor->hasFocus());
    editor->selectAll();
    QTest::keyClick(editor, Qt::Key_B, Qt::ControlModifier);
    QCOMPARE(editor->toPlainText(), QStringLiteral("**description**"));
    QCOMPARE(otherEditor->toPlainText(), QStringLiteral("untouched"));
    QTest::keyClick(editor, Qt::Key_B, Qt::ControlModifier);
    QCOMPARE(editor->toPlainText(), QStringLiteral("description"));
    QTest::keyClick(editor, Qt::Key_I, Qt::ControlModifier);
    QCOMPARE(editor->toPlainText(), QStringLiteral("*description*"));
    QCOMPARE(insertCalls, 0);
    auto *button = qobject_cast<QToolButton *>(
        toolbar->widgetForAction(formatAction(*toolbar, QStringLiteral("bold"))));
    QVERIFY(button);
    QCOMPARE(button->focusPolicy(), Qt::NoFocus);
    QTest::mouseClick(button, Qt::LeftButton);
    QVERIFY(editor->hasFocus());
    QCOMPARE(editor->toPlainText(), QStringLiteral("***description***"));
    otherEditor->setFocus();
    QTRY_VERIFY(otherEditor->hasFocus());
    QTest::keyClick(otherEditor, Qt::Key_B, Qt::ControlModifier);
    QCOMPARE(editor->toPlainText(), QStringLiteral("***description***"));
    QTest::keyClick(otherEditor, Qt::Key_I, Qt::ControlModifier);
    QCOMPARE(insertCalls, 1);
    QCOMPARE(otherEditor->toPlainText(), QStringLiteral("untouched"));

    // The link prompt is modal, so test it last on headless platforms
    editor->setPlainText(QStringLiteral("label"));
    editor->selectAll();
    QTimer::singleShot(0, &dialog, []() {
        auto *input = qobject_cast<QInputDialog *>(QApplication::activeModalWidget());
        QVERIFY(input);
        input->setTextValue(QStringLiteral("https://example.org"));
        input->accept();
    });
    QTest::keyClick(editor, Qt::Key_L, Qt::ControlModifier);
    QCOMPARE(editor->toPlainText(), QStringLiteral("[label](https://example.org)"));
    // Headless platforms don't reactivate the window, so check its focus widget.
    QCOMPARE(dialog.focusWidget(), editor);
    QTimer::singleShot(0, &dialog, []() {
        auto *input = qobject_cast<QInputDialog *>(QApplication::activeModalWidget());
        QVERIFY(input);
        input->reject();
    });
    QTest::keyClick(editor, Qt::Key_L, Qt::ControlModifier);
    QCOMPARE(editor->toPlainText(), QStringLiteral("[label](https://example.org)"));

    // A selected URL is suggested and used as the link text
    editor->setPlainText(QStringLiteral("https://qownnotes.org"));
    editor->selectAll();
    QTimer::singleShot(0, &dialog, []() {
        auto *input = qobject_cast<QInputDialog *>(QApplication::activeModalWidget());
        QVERIFY(input);
        QCOMPARE(input->textValue(), QStringLiteral("https://qownnotes.org"));
        input->accept();
    });
    QTest::keyClick(editor, Qt::Key_L, Qt::ControlModifier);
    QCOMPARE(editor->toPlainText(),
             QStringLiteral("[https://qownnotes.org](https://qownnotes.org)"));
    editor->setPlainText(QStringLiteral("[label](https://example.org)"));
    editor->setReadOnly(true);
    QTest::keyClick(editor, Qt::Key_I, Qt::ControlModifier);
    QCOMPARE(insertCalls, 1);
    QCOMPARE(editor->toPlainText(), QStringLiteral("[label](https://example.org)"));
}
