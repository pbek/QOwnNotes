#include "test_touchscrolling.h"

#include <QPlainTextEdit>
#include <QScrollBar>
#include <QScrollEvent>
#include <QScrollPrepareEvent>
#include <QScroller>
#include <QScrollerProperties>
#include <QTextBrowser>
#include <QWheelEvent>
#include <QtTest>

#include "utils/gui.h"

namespace {
QString longText() {
    QString text;
    for (int i = 0; i < 200; ++i) {
        text +=
            QStringLiteral("Line %1: ").arg(i) + QString(160, QLatin1Char('x')) + QLatin1Char('\n');
    }
    return text;
}
}    // namespace

void TestTouchScrolling::testPlainTextScrollCoordinates_data() {
    QTest::addColumn<bool>("wrap");
    QTest::addColumn<int>("fontSize");
    QTest::newRow("unwrapped") << false << 12;
    QTest::newRow("wrapped") << true << 12;
    QTest::newRow("larger-font") << false << 24;
}

void TestTouchScrolling::testPlainTextScrollCoordinates() {
    QFETCH(bool, wrap);
    QFETCH(int, fontSize);
    QPlainTextEdit editor;
    editor.resize(800, 240);
    QFont font = editor.font();
    font.setPointSize(fontSize);
    editor.setFont(font);
    editor.setLineWrapMode(wrap ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
    editor.setPlainText(longText());
    Utils::Gui::enableTouchScrolling(&editor);
    editor.show();
    QApplication::processEvents();

    QVERIFY(QScroller::grabbedGesture(editor.viewport()) != Qt::GestureType(0));
    QVERIFY(editor.viewport()->testAttribute(Qt::WA_AcceptTouchEvents));
    auto *vertical = editor.verticalScrollBar();
    auto *horizontal = editor.horizontalScrollBar();
    vertical->setValue(20);
    horizontal->setValue(horizontal->maximum() / 2);
    // Laying out newly visible wrapped blocks can adjust the scrollbar position.
    const int verticalPosition = vertical->value();
    const int horizontalPosition = horizontal->value();
    const qreal lineHeight = editor.fontMetrics().lineSpacing();

    QScrollPrepareEvent prepare(QPointF(20, 20));
    prepare.ignore();
    QApplication::sendEvent(editor.viewport(), &prepare);
    QVERIFY(prepare.isAccepted());
    QCOMPARE(prepare.viewportSize(), QSizeF(editor.viewport()->size()));
    QCOMPARE(prepare.contentPos(), QPointF(horizontalPosition, verticalPosition * lineHeight));
    QCOMPARE(prepare.contentPosRange().height(), vertical->maximum() * lineHeight);
    QCOMPARE(prepare.contentPosRange().width(), qreal(horizontal->maximum()));

    const QTextCursor cursor = editor.textCursor();
    QScrollEvent scroll(QPointF(horizontalPosition, 25 * lineHeight), QPointF(),
                        QScrollEvent::ScrollUpdated);
    QApplication::sendEvent(editor.viewport(), &scroll);
    QCOMPARE(vertical->value(), 25);
    QCOMPARE(horizontal->value(), horizontalPosition);
    QCOMPARE(editor.textCursor().position(), cursor.position());
    QCOMPARE(editor.textCursor().anchor(), cursor.anchor());

    const auto properties = QScroller::scroller(editor.viewport())->scrollerProperties();
    QCOMPARE(properties.scrollMetric(QScrollerProperties::VerticalOvershootPolicy).toInt(),
             int(QScrollerProperties::OvershootAlwaysOff));
}

void TestTouchScrolling::testPreviewScrollCoordinates() {
    QTextBrowser preview;
    preview.resize(800, 240);
    preview.setPlainText(longText());
    Utils::Gui::enableTouchScrolling(&preview);
    preview.show();
    QApplication::processEvents();
    QVERIFY(QScroller::grabbedGesture(preview.viewport()) != Qt::GestureType(0));
    QVERIFY(preview.viewport()->testAttribute(Qt::WA_AcceptTouchEvents));
    preview.verticalScrollBar()->setValue(100);

    QScrollPrepareEvent prepare(QPointF(20, 20));
    prepare.ignore();
    QApplication::sendEvent(preview.viewport(), &prepare);
    QVERIFY(prepare.isAccepted());
    QCOMPARE(prepare.contentPos().y(), qreal(100));
    QCOMPARE(prepare.contentPosRange().height(), qreal(preview.verticalScrollBar()->maximum()));

    QScrollEvent scroll(QPointF(0, 200), QPointF(), QScrollEvent::ScrollUpdated);
    QApplication::sendEvent(preview.viewport(), &scroll);
    QCOMPARE(preview.verticalScrollBar()->value(), 200);
}

void TestTouchScrolling::testTouchDrag() {
    QPlainTextEdit editor;
    editor.resize(800, 240);
    editor.setPlainText(longText());
    Utils::Gui::enableTouchScrolling(&editor);
    editor.show();
    QApplication::processEvents();

    auto *device = QTest::createTouchDevice();
    auto *viewport = editor.viewport();
    QTest::touchEvent(viewport, device).press(0, QPoint(200, 180), viewport);
    QTest::touchEvent(viewport, device).move(0, QPoint(200, 120), viewport);
    QTest::touchEvent(viewport, device).move(0, QPoint(200, 60), viewport);
    QTest::touchEvent(viewport, device).release(0, QPoint(200, 60), viewport);
    QTRY_VERIFY(editor.verticalScrollBar()->value() > 0);
    QScroller::scroller(viewport)->stop();
}

void TestTouchScrolling::testMouseWheel() {
    auto verifyWheel = [](auto &area) {
        area.resize(800, 240);
        area.setPlainText(longText());
        area.show();
        QApplication::processEvents();
        auto sendWheel = [&area] {
            const QPoint position(20, 20);
            QWheelEvent event(QPointF(position), QPointF(area.viewport()->mapToGlobal(position)),
                              QPoint(), QPoint(0, -120), Qt::NoButton, Qt::NoModifier,
                              Qt::NoScrollPhase, false);
            QApplication::sendEvent(area.viewport(), &event);
        };

        sendWheel();
        const int expected = area.verticalScrollBar()->value();
        QVERIFY(expected > 0);
        area.verticalScrollBar()->setValue(0);
        Utils::Gui::enableTouchScrolling(&area);
        sendWheel();
        QCOMPARE(area.verticalScrollBar()->value(), expected);
        QCOMPARE(QScroller::scroller(area.viewport())->state(), QScroller::Inactive);
    };

    QPlainTextEdit editor;
    verifyWheel(editor);
    QTextBrowser preview;
    verifyWheel(preview);
}

void TestTouchScrolling::testMouseSelection() {
    QPlainTextEdit editor;
    editor.resize(800, 240);
    editor.setPlainText(QStringLiteral("Text that can still be selected with the mouse"));
    Utils::Gui::enableTouchScrolling(&editor);
    editor.show();
    QApplication::processEvents();

    QTextCursor cursor = editor.textCursor();
    cursor.setPosition(0);
    const QPoint start = editor.cursorRect(cursor).center();
    cursor.setPosition(4);
    const QPoint end = editor.cursorRect(cursor).center();
    QTest::mousePress(editor.viewport(), Qt::LeftButton, Qt::NoModifier, start);
    QTest::mouseMove(editor.viewport(), end);
    QTest::mouseRelease(editor.viewport(), Qt::LeftButton, Qt::NoModifier, end);
    QCOMPARE(editor.textCursor().selectedText(), QStringLiteral("Text"));
    QCOMPARE(QScroller::scroller(editor.viewport())->state(), QScroller::Inactive);
}
