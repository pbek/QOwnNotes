#include "test_searchwidgets.h"

#include <QComboBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QTextBlock>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QtTest>

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
