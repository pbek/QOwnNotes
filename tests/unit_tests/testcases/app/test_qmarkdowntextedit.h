#ifndef TESTQMARKDOWNTEXTEDIT_H
#define TESTQMARKDOWNTEXTEDIT_H

#include <QObject>

class TestQMarkdownTextEdit : public QObject {
    Q_OBJECT

   private Q_SLOTS:
    void testUpFromStartOfSecondLineMovesToFirstLine();
    void testToPlainTextPreservesNoBreakSpaces();
    void testRCodeBlockHighlighting();
};

#endif    // TESTQMARKDOWNTEXTEDIT_H
