#include "test_nextclouddeckdialog.h"

#include <QComboBox>
#include <QFrame>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSignalSpy>
#include <QSplitter>
#include <QTreeWidget>
#include <QtTest>

#include "dialogs/nextclouddeckdialog.h"

namespace {
QTreeWidget *populatePicker(NextcloudDeckDialog &dialog) {
    auto *boards = dialog.findChild<QComboBox *>(QStringLiteral("boardStackComboBox"));
    // Supply offline fixtures without triggering a server reload.
    const QSignalBlocker blocker(boards);
    boards->addItem(QStringLiteral("Board / Stack"), 12);
    boards->setItemData(0, 34, Qt::UserRole + 1);
    boards->setCurrentIndex(0);

    auto *tree = dialog.findChild<QTreeWidget *>(QStringLiteral("cardItemTreeWidget"));
    tree->setEnabled(true);
    tree->setSortingEnabled(false);
    for (int i = 0; i < 3; ++i) {
        auto *item = new QTreeWidgetItem(tree, {QStringLiteral("Card %1").arg(i + 1)});
        item->setData(0, Qt::UserRole, 56 + i);
    }
    dialog.findChild<QLineEdit *>(QStringLiteral("newItemEdit"))->setEnabled(true);
    return tree;
}
}    // namespace

void TestNextcloudDeckDialog::testModes() {
    NextcloudDeckDialog manage;
    QVERIFY(!manage.findChild<QFrame *>(QStringLiteral("editFrame"))->isHidden());
    QVERIFY(!manage.findChild<QPushButton *>(QStringLiteral("insertLinksButton")));
    QCOMPARE(manage.objectName(), QStringLiteral("NextcloudDeckDialog"));

    NextcloudDeckDialog picker(nullptr, NextcloudDeckDialog::Mode::InsertLinks);
    QVERIFY(picker.findChild<QFrame *>(QStringLiteral("editFrame"))->isHidden());
    QVERIFY(picker.findChild<QPushButton *>(QStringLiteral("saveButton"))->shortcut().isEmpty());
    auto *insert = picker.findChild<QPushButton *>(QStringLiteral("insertLinksButton"));
    QVERIFY(insert);
    QVERIFY(!insert->isEnabled());
    QCOMPARE(picker.objectName(), QStringLiteral("NextcloudDeckLinkDialog"));

    // The picker buttons must be laid out below the card list instead of overlapping it
    picker.resize(600, 400);
    static_cast<QWidget &>(picker).show();
    QApplication::processEvents();
    auto *splitter = picker.findChild<QSplitter *>();
    QVERIFY(splitter);
    const QRect splitterRect(splitter->mapTo(&picker, QPoint(0, 0)), splitter->size());
    const QRect insertRect(insert->mapTo(&picker, QPoint(0, 0)), insert->size());
    QVERIFY(!splitterRect.intersects(insertRect));
    QVERIFY(insertRect.top() >= splitterRect.bottom());
    QCOMPARE(
        picker.findChild<QTreeWidget *>(QStringLiteral("cardItemTreeWidget"))->contextMenuPolicy(),
        Qt::NoContextMenu);
}

void TestNextcloudDeckDialog::testInsertLinks_data() {
    QTest::addColumn<QString>("activation");
    QTest::newRow("button") << QStringLiteral("button");
    QTest::newRow("enter-list") << QStringLiteral("list");
    QTest::newRow("enter-search") << QStringLiteral("search");
    QTest::newRow("double-click") << QStringLiteral("double-click");
}

void TestNextcloudDeckDialog::testInsertLinks() {
    QFETCH(QString, activation);
    NextcloudDeckDialog picker(nullptr, NextcloudDeckDialog::Mode::InsertLinks);
    auto *tree = populatePicker(picker);
    auto *search = picker.findChild<QLineEdit *>(QStringLiteral("newItemEdit"));
    auto *insert = picker.findChild<QPushButton *>(QStringLiteral("insertLinksButton"));
    QSignalSpy links(&picker, &NextcloudDeckDialog::cardLinksSelected);
    static_cast<QWidget &>(picker).show();
    // Select in reverse order; insertion must still follow list order.
    tree->topLevelItem(2)->setSelected(true);
    tree->topLevelItem(0)->setSelected(true);
    tree->topLevelItem(1)->setSelected(true);
    search->setText(QStringLiteral("Card 1"));
    QVERIFY(insert->isEnabled());
    search->setText(QStringLiteral("missing"));
    QVERIFY(!insert->isEnabled());
    QTest::keyClick(search, Qt::Key_Return);
    QCOMPARE(links.count(), 0);
    search->clear();
    tree->topLevelItem(1)->setHidden(true);

    if (activation == QStringLiteral("button")) {
        insert->click();
    } else if (activation == QStringLiteral("list")) {
        QTest::keyClick(tree, Qt::Key_Return);
    } else if (activation == QStringLiteral("search")) {
        QTest::keyClick(search, Qt::Key_Return);
    } else {
        QVERIFY(QMetaObject::invokeMethod(tree, "itemDoubleClicked",
                                          Q_ARG(QTreeWidgetItem *, tree->topLevelItem(0)),
                                          Q_ARG(int, 0)));
    }

    QCOMPARE(links.count(), 1);
    const QStringList inserted = links.first().first().toString().split(QLatin1Char('\n'));
    QCOMPARE(inserted.size(), 2);
    QVERIFY(inserted.at(0).startsWith(QStringLiteral("[Card 1](")));
    QVERIFY(inserted.at(0).endsWith(QStringLiteral("/board/12/card/56)")));
    QVERIFY(inserted.at(1).startsWith(QStringLiteral("[Card 3](")));
    QVERIFY(inserted.at(1).endsWith(QStringLiteral("/board/12/card/58)")));
    QCOMPARE(picker.result(), int(QDialog::Accepted));
}

void TestNextcloudDeckDialog::testPickerDoesNotCreateOrDeleteCards() {
    NextcloudDeckDialog picker(nullptr, NextcloudDeckDialog::Mode::InsertLinks);
    auto *tree = populatePicker(picker);
    static_cast<QWidget &>(picker).show();
    tree->topLevelItem(0)->setSelected(true);
    QTest::keyClick(tree, Qt::Key_Delete);
    QTest::keyClick(tree, Qt::Key_Backspace);
    QCOMPARE(tree->topLevelItemCount(), 3);
    tree->clearSelection();
    auto *search = picker.findChild<QLineEdit *>(QStringLiteral("newItemEdit"));
    search->setText(QStringLiteral("New card title"));
    QTest::keyClick(search, Qt::Key_Return);
    QVERIFY(picker.findChild<QFrame *>(QStringLiteral("editFrame"))->isHidden());
    QVERIFY(picker.findChild<QLineEdit *>(QStringLiteral("titleLineEdit"))->text().isEmpty());
    QCOMPARE(search->text(), QStringLiteral("New card title"));
    QCOMPARE(tree->topLevelItemCount(), 3);
    QTest::keyClick(&picker, Qt::Key_Escape);
    QCOMPARE(picker.result(), int(QDialog::Rejected));
}
