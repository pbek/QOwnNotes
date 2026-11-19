#include "test_timepickerbutton.h"

#include <QDateTimeEdit>
#include <QLabel>
#include <QPushButton>
#include <QSignalSpy>
#include <QToolButton>
#include <QtTest>

#include "widgets/timepickerbutton.h"

void TestTimePickerButton::testDialValues() {
    ClockDialWidget dial;
    dial.resize(dial.sizeHint());

    // Every hour is found again at its own position on the two rings
    for (int hour = 0; hour < 24; ++hour) {
        QCOMPARE(dial.valueAt(dial.positionForValue(hour)), hour);
    }

    dial.setMode(ClockDialWidget::Mode::Minutes);
    for (int minute = 0; minute < 60; minute += 5) {
        QCOMPARE(dial.valueAt(dial.positionForValue(minute)), minute);
    }

    // Minutes next to a label snap to it, others can be picked exactly
    QCOMPARE(dial.valueAt(dial.positionForValue(14)), 15);
    QCOMPARE(dial.valueAt(dial.positionForValue(59)), 0);
    QCOMPARE(dial.valueAt(dial.positionForValue(17)), 17);

    // Seconds are dropped
    QSignalSpy changed(&dial, &ClockDialWidget::timeChanged);
    dial.setTime(QTime(8, 30, 42));
    QCOMPARE(dial.time(), QTime(8, 30));
    QCOMPARE(changed.count(), 1);
    dial.setTime(QTime(8, 30, 10));
    QCOMPARE(changed.count(), 1);
}

void TestTimePickerButton::testDialKeyboard() {
    ClockDialWidget dial;
    dial.setTime(QTime(23, 58));
    dial.show();
    QSignalSpy modeChanged(&dial, &ClockDialWidget::modeChanged);

    // Hours wrap around the day
    QTest::keyClick(&dial, Qt::Key_Up);
    QCOMPARE(dial.time(), QTime(0, 58));

    // Tab switches to the minutes, which wrap without changing the hour
    QTest::keyClick(&dial, Qt::Key_Tab);
    QCOMPARE(dial.mode(), ClockDialWidget::Mode::Minutes);
    QCOMPARE(modeChanged.count(), 1);
    QTest::keyClick(&dial, Qt::Key_Up);
    QTest::keyClick(&dial, Qt::Key_Up);
    QCOMPARE(dial.time(), QTime(0, 0));
    QTest::keyClick(&dial, Qt::Key_PageDown);
    QCOMPARE(dial.time(), QTime(0, 55));
}

void TestTimePickerButton::testPopupAppliesTime() {
    QWidget window;
    auto *dateEdit = new QDateEdit(QDate(2026, 11, 19), &window);
    auto *edit = new QTimeEdit(QTime(10, 20), &window);
    auto *button = new TimePickerButton(&window);
    button->setTimeEdit(edit);
    button->setDateEdit(dateEdit);
    window.show();

    button->click();
    QVERIFY(button->isPopupVisible());
    TimePickerPopup *popup = button->popup();
    ClockDialWidget *dial = popup->dial();
    QCOMPARE(dial->time(), QTime(10, 20));

    // The date of the date edit is shown as a reminder
    auto *dateLabel = popup->findChild<QLabel *>(QStringLiteral("timePickerDateLabel"));
    QVERIFY(dateLabel != nullptr);
    QCOMPARE(dateLabel->text(), QLocale().toString(QDate(2026, 11, 19), QLocale::LongFormat));

    // Picking an hour with the mouse continues with the minutes
    QTest::mouseClick(dial, Qt::LeftButton, Qt::NoModifier, dial->positionForValue(15).toPoint());
    QCOMPARE(edit->time(), QTime(15, 20));
    QCOMPARE(dial->mode(), ClockDialWidget::Mode::Minutes);

    QTest::mouseClick(dial, Qt::LeftButton, Qt::NoModifier, dial->positionForValue(45).toPoint());
    QCOMPARE(edit->time(), QTime(15, 45));
    QCOMPARE(dateEdit->date(), QDate(2026, 11, 19));

    // Presets set the time right away
    const auto presets = popup->findChildren<QToolButton *>(QStringLiteral("timePresetButton"));
    QVERIFY(!presets.isEmpty());
    presets.first()->click();
    QCOMPARE(edit->time(), presets.first()->property("presetTime").toTime());
    QVERIFY(presets.first()->isChecked());

    // The hour button in the header switches back to the hours
    popup->findChild<QToolButton *>(QStringLiteral("timePickerHourButton"))->click();
    QCOMPARE(dial->mode(), ClockDialWidget::Mode::Hours);

    QTest::keyClick(dial, Qt::Key_Return);
    QTRY_VERIFY(!button->isPopupVisible());
    QCOMPARE(edit->time(), presets.first()->property("presetTime").toTime());
}

void TestTimePickerButton::testPopupEscapeRestoresTime() {
    QWidget window;
    auto *edit = new QTimeEdit(QTime(10, 20), &window);
    auto *button = new TimePickerButton(&window);
    button->setTimeEdit(edit);
    window.show();

    button->showPopup();
    ClockDialWidget *dial = button->popup()->dial();

    // Without a date edit no date is shown
    QVERIFY(button->popup()->findChild<QLabel *>(QStringLiteral("timePickerDateLabel")) == nullptr);

    QTest::keyClick(dial, Qt::Key_Down);
    QCOMPARE(edit->time(), QTime(9, 20));

    QTest::keyClick(dial, Qt::Key_Escape);
    QTRY_VERIFY(!button->isPopupVisible());
    QCOMPARE(edit->time(), QTime(10, 20));
}

void TestTimePickerButton::testFollowsEnabledState() {
    QWidget window;
    auto *edit = new QTimeEdit(&window);
    auto *button = new TimePickerButton(&window);
    button->setTimeEdit(edit);
    window.show();
    QVERIFY(button->isEnabled());

    edit->setEnabled(false);
    QVERIFY(!button->isEnabled());
    button->showPopup();
    QVERIFY(!button->isPopupVisible());

    edit->setEnabled(true);
    QVERIFY(button->isEnabled());

    // A disabled parent disables both, enabling it again restores the button
    window.setEnabled(false);
    QVERIFY(!button->isEnabled());
    window.setEnabled(true);
    QVERIFY(button->isEnabled());
}

void TestTimePickerButton::testDarkModeStyleSheet() {
    // The dark mode sets its colors with an application style sheet, not with the palette
    const QString styleSheet = qApp->styleSheet();
    qApp->setStyleSheet(
        QStringLiteral("QWidget { color: silver; background-color: #302f2f;"
                       " selection-background-color: #3daee9; selection-color: black;"
                       " font-size: 14px; }"));
    auto restoreStyleSheet = qScopeGuard([styleSheet]() { qApp->setStyleSheet(styleSheet); });

    QWidget window;
    auto *edit = new QTimeEdit(QTime(10, 20), &window);
    auto *button = new TimePickerButton(&window);
    button->setTimeEdit(edit);
    window.show();
    button->showPopup();
    QCoreApplication::processEvents();

    TimePickerPopup *popup = button->popup();
    QVERIFY(popup->palette().color(QPalette::Window).lightness() < 100);
    QVERIFY(popup->styleSheet().contains(QStringLiteral("background-color: #302f2f")));

    // The clock face paints with the colors of the style sheet
    ClockDialWidget *dial = popup->dial();
    QVERIFY(dial->palette().color(QPalette::WindowText).lightness() > 150);
    QCOMPARE(dial->palette().color(QPalette::Highlight), QColor(QStringLiteral("#3daee9")));

    // The large time display isn't shrunk by the font size of the style sheet
    auto *hourButton = popup->findChild<QToolButton *>(QStringLiteral("timePickerHourButton"));
    QVERIFY(QFontInfo(hourButton->font()).pixelSize() > 28);
}
