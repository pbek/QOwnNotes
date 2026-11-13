#include "test_searchwidgets.h"

#include <QCheckBox>
#include <QComboBox>
#include <QJSEngine>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QTextBlock>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QtTest>

#include "dialogs/trashdialog.h"
#include "libraries/qmarkdowntextedit/qmarkdowntextedit.h"
#include "libraries/qmarkdowntextedit/qplaintexteditsearchwidget.h"
#include "widgets/qtexteditsearchwidget.h"

namespace {
template <typename Editor, typename SearchWidget>
void verifyFailedSearch(int mode, bool searchDown, bool selection) {
    Editor editor;
    editor.resize(800, 240);
    editor.setLineWrapMode(Editor::NoWrap);
    QString text;
    for (int i = 0; i < 200; ++i) {
        text +=
            QStringLiteral("Line %1: ").arg(i) + QString(160, QLatin1Char('x')) + QLatin1Char('\n');
    }
    editor.setPlainText(text);

    auto *layout = new QVBoxLayout(&editor);
    SearchWidget search(&editor);
    layout->addWidget(&search);
    editor.show();
    search.activate();
    QApplication::processEvents();

    auto *modeComboBox = search.template findChild<QComboBox *>(QStringLiteral("modeComboBox"));
    auto *searchLineEdit = search.template findChild<QLineEdit *>(QStringLiteral("searchLineEdit"));
    QVERIFY(modeComboBox);
    QVERIFY(searchLineEdit);
    modeComboBox->setCurrentIndex(mode);

    QTextCursor cursor(editor.document()->findBlockByNumber(100));
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 30);
    if (selection) {
        cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 10);
    }
    editor.setTextCursor(cursor);
    // Keep the viewport away from the cursor to test independent scroll restoration.
    editor.verticalScrollBar()->setValue(editor.verticalScrollBar()->maximum() * 3 / 4);
    editor.horizontalScrollBar()->setValue(editor.horizontalScrollBar()->maximum() / 2);
    const int verticalScroll = editor.verticalScrollBar()->value();
    const int horizontalScroll = editor.horizontalScrollBar()->value();
    QVERIFY(verticalScroll > 0);
    QVERIFY(horizontalScroll > 0);

    // Setting the field also exercises the automatic search used when pasting a query.
    searchLineEdit->setText(QStringLiteral("missing-search-term"));
    QTRY_VERIFY(!searchLineEdit->styleSheet().isEmpty());
    QCOMPARE(editor.textCursor().position(), cursor.position());
    QCOMPARE(editor.textCursor().anchor(), cursor.anchor());
    QCOMPARE(editor.verticalScrollBar()->value(), verticalScroll);
    QCOMPARE(editor.horizontalScrollBar()->value(), horizontalScroll);

    QVERIFY(!search.doSearch(searchDown));
    QCOMPARE(editor.textCursor().position(), cursor.position());
    QCOMPARE(editor.textCursor().anchor(), cursor.anchor());
    QCOMPARE(editor.verticalScrollBar()->value(), verticalScroll);
    QCOMPARE(editor.horizontalScrollBar()->value(), horizontalScroll);

    QVERIFY(!search.doSearch(searchDown, false));
    QCOMPARE(editor.textCursor().position(), cursor.position());
    QCOMPARE(editor.textCursor().anchor(), cursor.anchor());
    QCOMPARE(editor.verticalScrollBar()->value(), verticalScroll);
    QCOMPARE(editor.horizontalScrollBar()->value(), horizontalScroll);
}

template <typename Editor, typename SearchWidget>
void verifySuccessfulWrap(bool searchDown) {
    Editor editor;
    editor.setPlainText(QStringLiteral("needle\nmiddle\nneedle"));
    auto *layout = new QVBoxLayout(&editor);
    SearchWidget search(&editor);
    layout->addWidget(&search);
    auto *searchLineEdit = search.template findChild<QLineEdit *>(QStringLiteral("searchLineEdit"));
    QVERIFY(searchLineEdit);
    const QSignalBlocker blocker(searchLineEdit);
    searchLineEdit->setText(QStringLiteral("needle"));

    editor.moveCursor(searchDown ? QTextCursor::End : QTextCursor::Start);
    QVERIFY(search.doSearch(searchDown));
    QCOMPARE(editor.textCursor().selectedText(), QStringLiteral("needle"));
    QCOMPARE(editor.textCursor().selectionStart(), searchDown ? 0 : 14);
}
}    // namespace

void TestSearchWidgets::testRegexReplacement_data() {
    QTest::addColumn<bool>("preview");
    QTest::addColumn<bool>("replaceAll");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("pattern");
    QTest::addColumn<QString>("replacement");
    QTest::addColumn<QString>("expected");
    QTest::addColumn<bool>("caseSensitive");

    for (bool preview : {false, true}) {
        for (bool replaceAll : {false, true}) {
            const QString prefix = QStringLiteral("%1-%2-")
                                       .arg(preview ? "preview" : "editor")
                                       .arg(replaceAll ? "all" : "single");
            auto row = [&](const char *name, const char *text, const char *pattern,
                           const char *replacement, const char *expected,
                           bool caseSensitive = true) {
                QTest::newRow(qPrintable(prefix + QLatin1String(name)))
                    << preview << replaceAll << QString::fromUtf8(text)
                    << QString::fromUtf8(pattern) << QString::fromUtf8(replacement)
                    << QString::fromUtf8(expected) << caseSensitive;
            };
            row("lookbehind", "ab", "(?<=a)b", "X", "aX");
            row("lookahead", "ab", "a(?=b)", "X", "Xb");
            row("negative-lookbehind", "ab", "(?<!c)b", "X", "aX");
            row("negative-lookahead", "ab", "a(?!c)", "X", "Xb");
            row("non-boundary", "ab", "\\Bb", "X", "aX");
            row("lookaround-captures", "abc", "(?<=(a))(b)(?=(c))", "\\3\\2\\1", "acbac");
            row("captures", "ab", "(a)(b)", "\\2-\\1-\\2", "b-a-b");
            row("optional-capture", "b", "(a)?(b)", "\\1X\\2", "Xb");
            row("literal-replacement", "ab", "(a)b", "\\0-\\9-$1", "\\0-\\9-$1");
            row("capture-not-reexpanded", "\\2b", "(\\\\2)(b)", "\\1", "\\2");
            row("two-digit-capture", "abcdefghij", "(a)(b)(c)(d)(e)(f)(g)(h)(i)(j)", "\\10-\\1",
                "j-a");
            row("digit-after-capture", "ab", "(a)b", "\\12", "a2");
            row("case-insensitive", "Ab", "(?<=a)(B)", "\\1X", "AbX", false);
            row("case-sensitive", "Ab ab", "(?<=a)b", "X", "Ab aX");
            row("block-anchors", "first\nab\nlast", "^ab$", "X", "first\nX\nlast");
            row("non-breaking-space", "a\u00a0b", "(a b)", "\\1X", "a\u00a0bX");
            row("word-boundary", "ab abc", "\\bab\\b", "X", "X abc");
            row("alternatives", "ab", "ab|b", "X", "X");
            row("empty-replacement", "ab", "(?<=a)b", "", "a");
            row("newline-replacement", "ab", "(?<=a)b", "X\nY", "aX\nY");
            row("unicode-offset", "\U0001f600ab", "(?<=a)(b)", "\\1X", "\U0001f600abX");
            row("multiple", "ab ab\nab", "(?<=a)b", "X", replaceAll ? "aX aX\naX" : "aX ab\nab");
        }
    }
}

namespace {
template <typename Editor, typename SearchWidget>
void verifyReplacement(const QString &text, const QString &pattern, const QString &replacement,
                       const QString &expected, bool caseSensitive, bool replaceAll,
                       int searchMode = SearchWidget::RegularExpressionMode) {
    Editor editor;
    editor.setPlainText(text);
    auto *layout = new QVBoxLayout(&editor);
    SearchWidget search(&editor);
    layout->addWidget(&search);
    auto *mode = search.template findChild<QComboBox *>(QStringLiteral("modeComboBox"));
    auto *query = search.template findChild<QLineEdit *>(QStringLiteral("searchLineEdit"));
    auto *replace = search.template findChild<QLineEdit *>(QStringLiteral("replaceLineEdit"));
    auto *matchCase =
        search.template findChild<QPushButton *>(QStringLiteral("matchCaseSensitiveButton"));
    QVERIFY(mode);
    QVERIFY(query);
    QVERIFY(replace);
    QVERIFY(matchCase);
    mode->setCurrentIndex(searchMode);
    const QSignalBlocker blocker(query);
    query->setText(pattern);
    replace->setText(replacement);
    matchCase->setChecked(caseSensitive);
    editor.moveCursor(QTextCursor::Start);
    if (replaceAll) {
        search.doReplaceAll();
    } else {
        QVERIFY(search.doSearch());
        QVERIFY(search.doReplace());
    }
    // toPlainText() normalizes non-breaking spaces; check the raw block as well.
    QCOMPARE(editor.document()->firstBlock().text(), expected.section(QLatin1Char('\n'), 0, 0));
    QString plainExpected = expected;
    plainExpected.replace(QChar::Nbsp, QLatin1Char(' '));
    QCOMPARE(editor.toPlainText(), plainExpected);
}
}    // namespace

void TestSearchWidgets::testRegexReplacement() {
    QFETCH(bool, preview);
    QFETCH(bool, replaceAll);
    QFETCH(QString, text);
    QFETCH(QString, pattern);
    QFETCH(QString, replacement);
    QFETCH(QString, expected);
    QFETCH(bool, caseSensitive);
    if (preview) {
        verifyReplacement<QTextEdit, QTextEditSearchWidget>(text, pattern, replacement, expected,
                                                            caseSensitive, replaceAll);
    } else {
        verifyReplacement<QPlainTextEdit, QPlainTextEditSearchWidget>(
            text, pattern, replacement, expected, caseSensitive, replaceAll);
    }
}

void TestSearchWidgets::testRegexReplacementRejectsStaleSelection() {
    // A selection that no longer matches must not be partially replaced.
    for (bool preview : {false, true}) {
        auto verify = [](auto &editor, auto &search) {
            search.template findChild<QComboBox *>(QStringLiteral("modeComboBox"))
                ->setCurrentIndex(2);
            auto *query = search.template findChild<QLineEdit *>(QStringLiteral("searchLineEdit"));
            const QSignalBlocker blocker(query);
            query->setText(QStringLiteral("ab"));
            search.template findChild<QLineEdit *>(QStringLiteral("replaceLineEdit"))
                ->setText(QStringLiteral("X"));
            editor.setPlainText(QStringLiteral("abc"));
            editor.selectAll();
            QVERIFY(!search.doReplace());
            QCOMPARE(editor.toPlainText(), QStringLiteral("abc"));
            QCOMPARE(editor.textCursor().selectedText(), QStringLiteral("abc"));
        };
        if (preview) {
            QTextEdit editor;
            QTextEditSearchWidget search(&editor);
            verify(editor, search);
        } else {
            QPlainTextEdit editor;
            QPlainTextEditSearchWidget search(&editor);
            verify(editor, search);
        }
    }
}

void TestSearchWidgets::testSelectionOnlyRegexReplacement() {
    QPlainTextEdit editor;
    editor.setPlainText(QStringLiteral("ab ab ab"));
    auto *layout = new QVBoxLayout(&editor);
    QPlainTextEditSearchWidget search(&editor);
    layout->addWidget(&search);
    QTextCursor cursor = editor.textCursor();
    cursor.setPosition(4);
    cursor.setPosition(5, QTextCursor::KeepAnchor);
    editor.setTextCursor(cursor);
    search.findChild<QPushButton *>(QStringLiteral("selectionOnlyButton"))->setChecked(true);
    search.findChild<QComboBox *>(QStringLiteral("modeComboBox"))->setCurrentIndex(2);
    auto *query = search.findChild<QLineEdit *>(QStringLiteral("searchLineEdit"));
    const QSignalBlocker blocker(query);
    query->setText(QStringLiteral("(?<=a)b"));
    search.findChild<QLineEdit *>(QStringLiteral("replaceLineEdit"))->setText(QStringLiteral("XX"));
    search.doReplaceAll();
    QCOMPARE(editor.toPlainText(), QStringLiteral("ab aXX ab"));
    // A second replacement must respect the adjusted selection scope.
    query->setText(QStringLiteral("XX"));
    search.findChild<QLineEdit *>(QStringLiteral("replaceLineEdit"))->setText(QStringLiteral("Y"));
    search.doReplaceAll();
    QCOMPARE(editor.toPlainText(), QStringLiteral("ab aY ab"));

    // Replacing a later match in a larger scope must not jump back to its first match.
    editor.setPlainText(QStringLiteral("ab ab ab ab"));
    search.findChild<QPushButton *>(QStringLiteral("selectionOnlyButton"))->setChecked(false);
    cursor = editor.textCursor();
    cursor.setPosition(3);
    cursor.setPosition(8, QTextCursor::KeepAnchor);
    editor.setTextCursor(cursor);
    search.findChild<QPushButton *>(QStringLiteral("selectionOnlyButton"))->setChecked(true);
    query->setText(QStringLiteral("(?<=a)b"));
    search.findChild<QLineEdit *>(QStringLiteral("replaceLineEdit"))->setText(QStringLiteral("X"));
    cursor.setPosition(7);
    cursor.setPosition(8, QTextCursor::KeepAnchor);
    editor.setTextCursor(cursor);
    QVERIFY(search.doReplace());
    QCOMPARE(editor.toPlainText(), QStringLiteral("ab ab aX ab"));
}

void TestSearchWidgets::testPlainAndWholeWordReplacement() {
    for (int mode : {0, 1}) {
        for (bool replaceAll : {false, true}) {
            const QString expected = !replaceAll ? QStringLiteral("\\1 ab abc")
                                     : mode == 0 ? QStringLiteral("\\1 \\1 \\1c")
                                                 : QStringLiteral("\\1 \\1 abc");
            verifyReplacement<QTextEdit, QTextEditSearchWidget>(
                QStringLiteral("Ab ab abc"), QStringLiteral("ab"), QStringLiteral("\\1"), expected,
                false, replaceAll, mode);
            verifyReplacement<QPlainTextEdit, QPlainTextEditSearchWidget>(
                QStringLiteral("Ab ab abc"), QStringLiteral("ab"), QStringLiteral("\\1"), expected,
                false, replaceAll, mode);
        }
    }
}

void TestSearchWidgets::testCloudTrashTextSearch() {
    QJSEngine engine;
    const QJSValue notes = engine.evaluate(QStringLiteral(
        "[{noteName: 'First', fileName: 'first.md', data: 'Unrelated text', timestamp: 1},"
        " {noteName: 'Second', fileName: 'second.md', data: 'Body-only NEEDLE', timestamp: 2},"
        " {noteName: 'Third', fileName: 'third.md', data: 'Another needle', timestamp: 3}]"));
    TrashDialog dialog(notes);
    auto *search = dialog.findChild<QWidget *>(QStringLiteral("listFrame"))
                       ->findChild<QLineEdit *>(QStringLiteral("searchLineEdit"));
    auto *list = dialog.findChild<QListWidget *>(QStringLiteral("trashListWidget"));
    auto *preview = dialog.findChild<QMarkdownTextEdit *>(QStringLiteral("noteBrowser"));
    QVERIFY(search);
    QVERIFY(list);
    QVERIFY(preview);

    // JavaScript property iteration order is not guaranteed
    list->sortItems();
    QCOMPARE(list->count(), 3);
    static_cast<QWidget &>(dialog).show();
    search->setFocus();
    QApplication::processEvents();
    search->setText(QStringLiteral("needle"));
    QVERIFY(search->hasFocus());
    QVERIFY(list->item(0)->isHidden());
    QVERIFY(!list->item(1)->isHidden());
    QVERIFY(!list->item(2)->isHidden());
    QCOMPARE(list->currentRow(), 1);
    QCOMPARE(preview->toPlainText(), QStringLiteral("Body-only NEEDLE"));
    auto *inNoteSearch = preview->searchWidget();
    QVERIFY(!inNoteSearch->isHidden());
    QCOMPARE(inNoteSearch->findChild<QLineEdit *>(QStringLiteral("searchLineEdit"))->text(),
             search->text());
    QCOMPARE(preview->textCursor().selectedText(), QStringLiteral("NEEDLE"));

    // Removing an earlier entry must not shift text or restoration metadata
    delete list->takeItem(0);
    list->setCurrentRow(1);
    QCOMPARE(preview->toPlainText(), QStringLiteral("Another needle"));
    QCOMPARE(preview->textCursor().selectedText(), QStringLiteral("needle"));
    QCOMPARE(list->currentItem()->data(Qt::UserRole).toString(), QStringLiteral("third.md"));
    QCOMPARE(list->currentItem()->data(Qt::UserRole + 2).toInt(), 3);

    search->setText(QStringLiteral("missing-trash-search-term"));
    QCOMPARE(list->currentRow(), -1);
    QVERIFY(list->selectedItems().isEmpty());
    QVERIFY(preview->toPlainText().isEmpty());

    search->setText(QStringLiteral("SECOND"));
    QCOMPARE(list->currentRow(), 0);
    QCOMPARE(preview->toPlainText(), QStringLiteral("Body-only NEEDLE"));
    search->clear();
    QVERIFY(search->hasFocus());
    QVERIFY(inNoteSearch->isHidden());
    QVERIFY(preview->extraSelections().isEmpty());
    for (int row = 0; row < list->count(); ++row) {
        QVERIFY(!list->item(row)->isHidden());
    }

    // Select all must only select the notes that match the search
    auto *selectAll = dialog.findChild<QCheckBox *>(QStringLiteral("selectAllCheckBox"));
    QVERIFY(selectAll);
    search->setText(QStringLiteral("another"));

    // The only matching note became the current one, so it is already selected
    QCOMPARE(selectAll->checkState(), Qt::Checked);
    selectAll->click();
    QVERIFY(list->selectedItems().isEmpty());
    QCOMPARE(selectAll->checkState(), Qt::Unchecked);
    selectAll->click();
    QCOMPARE(list->selectedItems().count(), 1);
    QCOMPARE(list->selectedItems().first()->data(Qt::UserRole).toString(),
             QStringLiteral("third.md"));
    QCOMPARE(selectAll->checkState(), Qt::Checked);

    search->clear();
    QCOMPARE(selectAll->checkState(), Qt::PartiallyChecked);
    selectAll->click();
    QCOMPARE(list->selectedItems().count(), list->count());
    QCOMPARE(selectAll->checkState(), Qt::Checked);
    selectAll->click();
    QVERIFY(list->selectedItems().isEmpty());
    QCOMPARE(selectAll->checkState(), Qt::Unchecked);
}

void TestSearchWidgets::testFailedSearchPreservesPosition_data() {
    QTest::addColumn<bool>("preview");
    QTest::addColumn<int>("mode");
    QTest::addColumn<bool>("searchDown");
    QTest::addColumn<bool>("selection");

    for (bool preview : {false, true}) {
        for (int mode = 0; mode < 3; ++mode) {
            for (bool searchDown : {false, true}) {
                for (bool selection : {false, true}) {
                    const QString name = QStringLiteral("%1-mode%2-%3-%4")
                                             .arg(preview ? "preview" : "editor")
                                             .arg(mode)
                                             .arg(searchDown ? "down" : "up")
                                             .arg(selection ? "selection" : "cursor");
                    QTest::newRow(qPrintable(name)) << preview << mode << searchDown << selection;
                }
            }
        }
    }
}

void TestSearchWidgets::testFailedSearchPreservesPosition() {
    QFETCH(bool, preview);
    QFETCH(int, mode);
    QFETCH(bool, searchDown);
    QFETCH(bool, selection);
    if (preview) {
        verifyFailedSearch<QTextEdit, QTextEditSearchWidget>(mode, searchDown, selection);
    } else {
        verifyFailedSearch<QPlainTextEdit, QPlainTextEditSearchWidget>(mode, searchDown, selection);
    }
}

void TestSearchWidgets::testSuccessfulSearchWraps_data() {
    QTest::addColumn<bool>("preview");
    QTest::addColumn<bool>("searchDown");
    QTest::newRow("editor-down") << false << true;
    QTest::newRow("editor-up") << false << false;
    QTest::newRow("preview-down") << true << true;
    QTest::newRow("preview-up") << true << false;
}

void TestSearchWidgets::testSuccessfulSearchWraps() {
    QFETCH(bool, preview);
    QFETCH(bool, searchDown);
    if (preview) {
        verifySuccessfulWrap<QTextEdit, QTextEditSearchWidget>(searchDown);
    } else {
        verifySuccessfulWrap<QPlainTextEdit, QPlainTextEditSearchWidget>(searchDown);
    }
}

void TestSearchWidgets::testFailedSelectionOnlySearchPreservesPosition() {
    QPlainTextEdit editor;
    editor.setPlainText(QStringLiteral("needle\nselected text\nneedle"));
    auto *layout = new QVBoxLayout(&editor);
    QPlainTextEditSearchWidget search(&editor);
    layout->addWidget(&search);

    QTextCursor cursor(editor.document()->findBlockByNumber(1));
    cursor.select(QTextCursor::BlockUnderCursor);
    editor.setTextCursor(cursor);
    auto *selectionOnlyButton =
        search.findChild<QPushButton *>(QStringLiteral("selectionOnlyButton"));
    auto *searchLineEdit = search.findChild<QLineEdit *>(QStringLiteral("searchLineEdit"));
    QVERIFY(selectionOnlyButton);
    QVERIFY(searchLineEdit);
    selectionOnlyButton->setChecked(true);
    searchLineEdit->setText(QStringLiteral("needle"));

    for (bool searchDown : {false, true}) {
        QVERIFY(!search.doSearch(searchDown));
        QCOMPARE(editor.textCursor().position(), cursor.position());
        QCOMPARE(editor.textCursor().anchor(), cursor.anchor());
    }
}
