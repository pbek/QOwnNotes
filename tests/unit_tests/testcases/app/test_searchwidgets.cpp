#include "test_searchwidgets.h"

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
