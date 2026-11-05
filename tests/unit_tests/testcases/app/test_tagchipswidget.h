#pragma once

#include <QObject>

class TestTagChipsWidget : public QObject {
    Q_OBJECT

   private slots:
    void testSelection();
    void testRemoveChip();
    void testPopup();
    void testNestedAndUncoloredTags();
    void testPartialSelection();
    void testChipClick();
    void testPopupFitsWrappedText();
};
