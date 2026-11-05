#include "test_tagchipswidget.h"

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QSignalSpy>
#include <QToolButton>
#include <QtTest>

#include "widgets/tagchipswidget.h"

namespace {
TagChipsWidget::Item makeItem(int id, const QString &name, const QString &color = QString(),
                              const QStringList &parentNames = QStringList()) {
    TagChipsWidget::Item item;
    item.id = id;
    item.name = name;
    item.color = color.isEmpty() ? QColor() : QColor(QStringLiteral("#") + color);
    item.parentNames = parentNames;
    return item;
}

QList<TagChipsWidget::Item> testItems() {
    return {makeItem(3, QStringLiteral("Later"), QStringLiteral("F1DB50")),
            makeItem(1, QStringLiteral("Finished"), QStringLiteral("31CC7C")),
            makeItem(2, QStringLiteral("Action needed"), QStringLiteral("FF7A66"))};
}

QList<QFrame *> chips(const TagChipsWidget &widget) {
    // Removed chips lose their object name until they are deleted
    return widget.findChildren<QFrame *>(QStringLiteral("tagChip"));
}

QStringList chipNames(const TagChipsWidget &widget) {
    QStringList names;
    for (auto *chip : chips(widget)) {
        names.append(chip->findChild<QLabel *>()->text());
    }
    return names;
}

QListWidget *popupList(const TagChipsWidget &widget) {
    return widget.findChild<QFrame *>(QStringLiteral("tagChipsPopup"))
        ->findChild<QListWidget *>(QStringLiteral("tagListWidget"));
}

QLineEdit *popupSearch(const TagChipsWidget &widget) {
    return widget.findChild<QFrame *>(QStringLiteral("tagChipsPopup"))
        ->findChild<QLineEdit *>(QStringLiteral("tagSearchEdit"));
}
}    // namespace

void TestTagChipsWidget::testSelection() {
    TagChipsWidget widget;
    QSignalSpy changed(&widget, &TagChipsWidget::selectedIdsChanged);
    widget.setItems(testItems());

    // Tags are offered alphabetically
    QCOMPARE(widget.items().first().name, QStringLiteral("Action needed"));

    widget.setSelectedIds({3, 1, 42});
    QCOMPARE(changed.count(), 1);
    // Unknown tags are ignored and the chips follow the alphabetical order
    QCOMPARE(widget.selectedIds(), QList<int>({3, 1}));
    QVERIFY(widget.hasSameSelection({1, 3}));
    QVERIFY(!widget.hasSameSelection({1}));
    QCOMPARE(chipNames(widget), QStringList({QStringLiteral("Finished"), QStringLiteral("Later")}));

    // The add button only shows its text while there are no tags
    auto *addButton = widget.findChild<QToolButton *>(QStringLiteral("addTagButton"));
    QVERIFY(addButton->text().isEmpty());
    widget.setSelectedIds({});
    QCOMPARE(addButton->text(), QStringLiteral("Add tag"));

    // Tags that aren't available anymore are dropped from the selection
    widget.setSelectedIds({1, 3});
    widget.setItems({makeItem(1, QStringLiteral("Finished"), QStringLiteral("31CC7C"))});
    QCOMPARE(widget.selectedIds(), QList<int>({1}));

    QCOMPARE(TagChipsWidget::textColorForBackground(QColor(0xF1, 0xDB, 0x50)),
             QColor(0x22, 0x22, 0x22));
    QCOMPARE(TagChipsWidget::textColorForBackground(QColor(0x3A, 0x3B, 0x3D)), QColor(Qt::white));
    QCOMPARE(TagChipsWidget::nextUnusedColor({QStringLiteral("31CC7C"), QStringLiteral("317CCC")},
                                             {QColor(0x31, 0xCC, 0x7C)}),
             QStringLiteral("317CCC"));
    QVERIFY(TagChipsWidget::nextUnusedColor({}, {}).isEmpty());
}

void TestTagChipsWidget::testRemoveChip() {
    TagChipsWidget widget;
    widget.setItems(testItems());
    widget.setSelectedIds({1, 3});
    widget.show();
    QSignalSpy changed(&widget, &TagChipsWidget::selectedIdsChanged);
    QSignalSpy toggled(&widget, &TagChipsWidget::tagToggled);

    auto *chip = chips(widget).first();
    const int removedId = chip->property("tagId").toInt();
    chip->findChild<QToolButton *>(QStringLiteral("removeTagButton"))->click();
    QCOMPARE(changed.count(), 1);
    QCOMPARE(toggled.count(), 1);
    QCOMPARE(toggled.first().at(0).toInt(), removedId);
    QCOMPARE(toggled.first().at(1).toBool(), false);
    QVERIFY(!widget.selectedIds().contains(removedId));
    QCOMPARE(chips(widget).count(), 1);
}

void TestTagChipsWidget::testPopup() {
    TagChipsWidget widget;
    widget.setItems(testItems());
    widget.setNewTagColors({QStringLiteral("31CC7C"), QStringLiteral("317CCC")});
    widget.show();
    QSignalSpy create(&widget, &TagChipsWidget::createTagRequested);
    QSignalSpy aboutToShow(&widget, &TagChipsWidget::popupAboutToShow);

    widget.showPopup();
    QCOMPARE(aboutToShow.count(), 1);
    QVERIFY(widget.isPopupVisible());
    auto *search = popupSearch(widget);
    auto *list = popupList(widget);
    QCOMPARE(list->count(), 3);

    // An exact match toggles the tag instead of creating a new one
    search->setText(QStringLiteral("later"));
    QTest::keyClick(search, Qt::Key_Return);
    QCOMPARE(widget.selectedIds(), QList<int>({3}));
    QVERIFY(search->text().isEmpty());
    QCOMPARE(list->item(2)->checkState(), Qt::Checked);

    // Space toggles the current tag of the list
    list->setCurrentRow(2);
    QTest::keyClick(list, Qt::Key_Space);
    QVERIFY(widget.selectedIds().isEmpty());

    // Tab adds the best match of the search, like the former tag name completion
    search->setText(QStringLiteral("act"));
    QTest::keyClick(search, Qt::Key_Tab);
    QCOMPARE(widget.selectedIds(), QList<int>({2}));

    // Partial matches can be created as new tags
    search->setText(QStringLiteral("Fin"));
    QCOMPARE(list->count(), 2);
    QCOMPARE(list->item(1)->data(Qt::UserRole).toInt(), -1);
    list->setCurrentRow(1);
    QTest::keyClick(search, Qt::Key_Return);
    QCOMPARE(create.count(), 1);
    QCOMPARE(create.first().at(0).toString(), QStringLiteral("Fin"));
    // The first unused color is suggested
    QCOMPARE(create.first().at(1).toString(), QStringLiteral("317CCC"));

    // A created tag is selected and the search is reset
    widget.addItem(makeItem(7, QStringLiteral("Fin"), QStringLiteral("317CCC")));
    QCOMPARE(widget.selectedIds(), QList<int>({2, 7}));
    QVERIFY(search->text().isEmpty());
    QCOMPARE(list->count(), 4);

    QTest::keyClick(search, Qt::Key_Escape);
    QTRY_VERIFY(!widget.isPopupVisible());
}

void TestTagChipsWidget::testNestedAndUncoloredTags() {
    TagChipsWidget widget;
    widget.setItems({makeItem(1, QStringLiteral("Work")),
                     makeItem(2, QStringLiteral("Meetings"), QString(), {QStringLiteral("Work")}),
                     makeItem(3, QStringLiteral("Archive"))});
    widget.setSelectedIds({2});
    widget.show();
    QSignalSpy create(&widget, &TagChipsWidget::createTagRequested);

    // Chips show the name, the tooltip shows the full path
    QCOMPARE(chipNames(widget), QStringList({QStringLiteral("Meetings")}));
    QVERIFY(chips(widget).first()->toolTip().contains(QStringLiteral("Work › Meetings")));

    // The popup groups nested tags below their parents and searches the full path
    widget.showPopup();
    auto *list = popupList(widget);
    QCOMPARE(list->item(0)->text(), QStringLiteral("Archive"));
    QCOMPARE(list->item(1)->text(), QStringLiteral("Work"));
    QCOMPARE(list->item(2)->text(), QStringLiteral("Work › Meetings"));
    popupSearch(widget)->setText(QStringLiteral("work"));
    QCOMPARE(list->count(), 2);

    // Without tag colors no color can be chosen and an empty color is requested
    popupSearch(widget)->setText(QStringLiteral("Private"));
    QTest::keyClick(popupSearch(widget), Qt::Key_Return);
    QCOMPARE(create.count(), 1);
    QVERIFY(create.first().at(1).toString().isEmpty());
}

void TestTagChipsWidget::testPartialSelection() {
    TagChipsWidget widget;
    widget.setItems(testItems());
    widget.setSelectedIds({1, 3});
    widget.setPartiallySelectedIds({3, 2});
    widget.setPartialSelectionToolTip(QStringLiteral("Only on some notes"));
    widget.show();
    QSignalSpy toggled(&widget, &TagChipsWidget::tagToggled);

    // Only selected tags can be partially selected
    QCOMPARE(widget.partiallySelectedIds(), QList<int>({3}));
    QFrame *partialChip = nullptr;
    for (auto *chip : chips(widget)) {
        if (chip->property("tagId").toInt() == 3) {
            partialChip = chip;
        }
    }
    QVERIFY(partialChip);
    QVERIFY(partialChip->property("chipDashed").toBool());
    QVERIFY(partialChip->toolTip().contains(QStringLiteral("Only on some notes")));

    widget.showPopup();
    auto *list = popupList(widget);
    QCOMPARE(list->item(2)->checkState(), Qt::PartiallyChecked);

    // Toggling a partially selected tag selects it for all
    list->setCurrentRow(2);
    QTest::keyClick(list, Qt::Key_Return);
    QCOMPARE(toggled.count(), 1);
    QCOMPARE(toggled.first().at(0).toInt(), 3);
    QCOMPARE(toggled.first().at(1).toBool(), true);
    QVERIFY(widget.partiallySelectedIds().isEmpty());
    QCOMPARE(widget.selectedIds(), QList<int>({1, 3}));
    QCOMPARE(list->item(2)->checkState(), Qt::Checked);
}

void TestTagChipsWidget::testChipClick() {
    TagChipsWidget widget;
    widget.setItems(testItems());
    widget.setSelectedIds({1});
    widget.show();
    QSignalSpy clicked(&widget, &TagChipsWidget::tagClicked);

    // Chips are only clickable if enabled
    QTest::mouseClick(chips(widget).first(), Qt::LeftButton);
    QCOMPARE(clicked.count(), 0);

    widget.setChipsClickable(true);
    widget.setChipToolTipTemplates(QStringLiteral("Show notes tagged with \"%1\""),
                                   QStringLiteral("Remove \"%1\""));
    auto *chip = chips(widget).first();
    QVERIFY(chip->toolTip().contains(QStringLiteral("Show notes tagged with \"Finished\"")));
    QCOMPARE(chip->findChild<QToolButton *>()->toolTip(), QStringLiteral("Remove \"Finished\""));
    QTest::mouseClick(chip->findChild<QLabel *>(), Qt::LeftButton);
    QCOMPARE(clicked.count(), 1);
    QCOMPARE(clicked.first().at(0).toInt(), 1);

    // The remove button doesn't count as click on the chip
    chip->findChild<QToolButton *>()->click();
    QCOMPARE(clicked.count(), 1);
    QVERIFY(widget.selectedIds().isEmpty());
}

void TestTagChipsWidget::testPopupFitsWrappedText() {
    // Fonts of an application style sheet are only applied when the popup gets polished
    const QString styleSheet = qApp->styleSheet();
    qApp->setStyleSheet(QStringLiteral("QWidget { font-size: 18px; }"));
    auto restoreStyleSheet = qScopeGuard([styleSheet]() { qApp->setStyleSheet(styleSheet); });

    TagChipsWidget widget;
    widget.setPopupHintText(
        QStringLiteral("New tags are created right away, the tags of the card are stored when "
                       "the card is saved."));
    widget.show();

    const auto verifyLabelsFit = [&widget]() {
        auto *popup = widget.findChild<QFrame *>(QStringLiteral("tagChipsPopup"));
        QVERIFY(popup != nullptr);
        QCoreApplication::processEvents();

        // Every visible word-wrapped label has to be high enough for its text and inside the popup
        for (auto *label : popup->findChildren<QLabel *>()) {
            if (!label->isVisible() || !label->wordWrap()) {
                continue;
            }

            QVERIFY2(label->height() >= label->heightForWidth(label->width()),
                     qPrintable(label->text()));
            QVERIFY2(label->geometry().bottom() <= popup->contentsRect().bottom(),
                     qPrintable(label->text()));
        }
    };

    // Without tags, the empty label and the hint are both wrapped
    widget.showPopup();
    verifyLabelsFit();

    widget.setItems(testItems());
    verifyLabelsFit();

    popupSearch(widget)->setText(QStringLiteral("No such tag"));
    verifyLabelsFit();
}
