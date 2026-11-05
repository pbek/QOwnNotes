#include "test_nextclouddeckdialog.h"

#include <QComboBox>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSignalSpy>
#include <QSplitter>
#include <QToolButton>
#include <QTreeWidget>
#include <QtTest>

#include "dialogs/nextclouddeckdialog.h"
#include "widgets/nextclouddecklabelswidget.h"

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
    QVERIFY(picker.findChild<QPushButton *>(QStringLiteral("createAndInsertLinkButton"))
                ->shortcut()
                .isEmpty());
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

void TestNextcloudDeckDialog::testNewCardButtons() {
    NextcloudDeckDialog dialog;
    populatePicker(dialog);
    static_cast<QWidget &>(dialog).show();
    auto *save = dialog.findChild<QPushButton *>(QStringLiteral("saveButton"));
    auto *createAndInsert =
        dialog.findChild<QPushButton *>(QStringLiteral("createAndInsertLinkButton"));
    QVERIFY(createAndInsert);

    // Start a new card from the search field
    auto *search = dialog.findChild<QLineEdit *>(QStringLiteral("newItemEdit"));
    search->setText(QStringLiteral("New card title"));
    QTest::keyClick(search, Qt::Key_Return);

    QCOMPARE(dialog.findChild<QLineEdit *>(QStringLiteral("titleLineEdit"))->text(),
             QStringLiteral("New card title"));
    QVERIFY(save->isVisible());
    QVERIFY(createAndInsert->isVisible());
    QVERIFY(save->isEnabled());
    QVERIFY(createAndInsert->isEnabled());

    // Creating and inserting a link keeps the established Ctrl+S shortcut
    QCOMPARE(createAndInsert->shortcut(), QKeySequence(QStringLiteral("Ctrl+S")));
    QCOMPARE(save->shortcut(), QKeySequence(QStringLiteral("Ctrl+Shift+S")));

    // Both buttons need a title
    dialog.findChild<QLineEdit *>(QStringLiteral("titleLineEdit"))->clear();
    QVERIFY(!save->isEnabled());
    QVERIFY(!createAndInsert->isEnabled());
}

namespace {
QList<NextcloudDeckService::Label> testLabels() {
    return {{3, QStringLiteral("Later"), QStringLiteral("F1DB50")},
            {1, QStringLiteral("Finished"), QStringLiteral("31CC7C")},
            {2, QStringLiteral("Action needed"), QStringLiteral("FF7A66")}};
}

QStringList chipTitles(const NextcloudDeckLabelsWidget &widget) {
    QStringList titles;
    for (auto *chip : widget.findChildren<QFrame *>(QStringLiteral("deckLabelChip"))) {
        titles.append(chip->findChild<QLabel *>()->text());
    }
    titles.sort();
    return titles;
}
}    // namespace

void TestNextcloudDeckDialog::testLabelsWidget() {
    NextcloudDeckLabelsWidget widget;
    QSignalSpy changed(&widget, &NextcloudDeckLabelsWidget::selectedLabelsChanged);
    widget.setAvailableLabels(testLabels());

    // Tags are offered alphabetically
    QCOMPARE(widget.availableLabels().first().title, QStringLiteral("Action needed"));

    widget.setSelectedLabelIds({3, 1, 42});
    QCOMPARE(changed.count(), 1);
    // Unknown tags are ignored and the selection follows the available order
    QCOMPARE(widget.selectedLabelIds(), QList<int>({3, 1}));
    QCOMPARE(widget.selectedLabels().first().title, QStringLiteral("Finished"));
    QVERIFY(widget.hasSameSelection({1, 3}));
    QVERIFY(!widget.hasSameSelection({1}));
    QCOMPARE(chipTitles(widget),
             QStringList({QStringLiteral("Finished"), QStringLiteral("Later")}));

    // Removing a tag via its chip
    widget.show();
    auto *chip = widget.findChildren<QFrame *>(QStringLiteral("deckLabelChip")).first();
    const int removedId = chip->property("labelId").toInt();
    chip->findChild<QToolButton *>(QStringLiteral("removeLabelButton"))->click();
    QCOMPARE(changed.count(), 2);
    QVERIFY(!widget.selectedLabelIds().contains(removedId));
    QTRY_COMPARE(widget.findChildren<QFrame *>(QStringLiteral("deckLabelChip")).count(), 1);

    // Tags that aren't available in the board anymore are dropped from the selection
    widget.setSelectedLabelIds({1, 3});
    widget.setAvailableLabels({{1, QStringLiteral("Finished"), QStringLiteral("31CC7C")}});
    QCOMPARE(widget.selectedLabelIds(), QList<int>({1}));

    // Readable text colors on light and dark tags
    QCOMPARE(NextcloudDeckLabelsWidget::textColorForBackground(QColor(0xF1, 0xDB, 0x50)),
             QColor(0x22, 0x22, 0x22));
    QCOMPARE(NextcloudDeckLabelsWidget::textColorForBackground(QColor(0x3A, 0x3B, 0x3D)),
             QColor(Qt::white));
}

void TestNextcloudDeckDialog::testLabelPopup() {
    NextcloudDeckLabelsWidget widget;
    widget.setAvailableLabels(testLabels());
    widget.show();
    QSignalSpy create(&widget, &NextcloudDeckLabelsWidget::createLabelRequested);

    widget.showLabelPopup();
    QVERIFY(widget.isLabelPopupVisible());
    auto *popup = widget.findChild<QFrame *>(QStringLiteral("deckLabelPopup"));
    auto *search = popup->findChild<QLineEdit *>(QStringLiteral("labelSearchEdit"));
    auto *list = popup->findChild<QListWidget *>(QStringLiteral("labelListWidget"));
    QCOMPARE(list->count(), 3);

    // An exact match toggles the tag instead of creating a new one
    search->setText(QStringLiteral("later"));
    QTest::keyClick(search, Qt::Key_Return);
    QCOMPARE(widget.selectedLabelIds(), QList<int>({3}));
    QVERIFY(search->text().isEmpty());
    QCOMPARE(list->item(2)->checkState(), Qt::Checked);

    // Space toggles the current tag of the list
    list->setCurrentRow(2);
    QTest::keyClick(list, Qt::Key_Space);
    QVERIFY(widget.selectedLabelIds().isEmpty());

    // Partial matches can be created as new tags
    search->setText(QStringLiteral("Fin"));
    QCOMPARE(list->count(), 2);
    QCOMPARE(list->item(1)->data(Qt::UserRole).toInt(), -1);
    list->setCurrentRow(1);
    QTest::keyClick(search, Qt::Key_Return);
    QCOMPARE(create.count(), 1);
    QCOMPARE(create.first().at(0).toString(), QStringLiteral("Fin"));
    // The first unused default color is suggested
    QCOMPARE(create.first().at(1).toString(), QStringLiteral("317CCC"));

    // A created tag is selected and the search is reset
    widget.addAvailableLabel({7, QStringLiteral("Fin"), QStringLiteral("317CCC")});
    QCOMPARE(widget.selectedLabelIds(), QList<int>({7}));
    QVERIFY(search->text().isEmpty());
    QCOMPARE(list->count(), 4);

    QTest::keyClick(search, Qt::Key_Escape);
    QTRY_VERIFY(!widget.isLabelPopupVisible());
}

void TestNextcloudDeckDialog::testSearchCardsByTag() {
    NextcloudDeckDialog dialog;
    auto *tree = populatePicker(dialog);
    tree->topLevelItem(1)->setText(2, QStringLiteral("Finished, Later"));
    auto *search = dialog.findChild<QLineEdit *>(QStringLiteral("newItemEdit"));

    search->setText(QStringLiteral("later"));
    QVERIFY(tree->topLevelItem(0)->isHidden());
    QVERIFY(!tree->topLevelItem(1)->isHidden());
    QVERIFY(tree->topLevelItem(2)->isHidden());

    // Every word has to match either the title or the tags
    search->setText(QStringLiteral("card later"));
    QVERIFY(!tree->topLevelItem(1)->isHidden());
    search->setText(QStringLiteral("card 1 later"));
    QVERIFY(tree->topLevelItem(1)->isHidden());

    search->clear();
    QVERIFY(!tree->topLevelItem(0)->isHidden());
    QVERIFY(!tree->topLevelItem(2)->isHidden());

    QCOMPARE(tree->columnCount(), 3);
    QVERIFY(dynamic_cast<NextcloudDeckLabelDelegate *>(tree->itemDelegateForColumn(2)) != nullptr);
}
