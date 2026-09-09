#ifndef TESTQMARKDOWNTEXTEDIT_H
#define TESTQMARKDOWNTEXTEDIT_H

#include <QObject>

class TestQMarkdownTextEdit : public QObject {
    Q_OBJECT

   private Q_SLOTS:
    void testUpFromStartOfSecondLineMovesToFirstLine();
    void testToPlainTextPreservesNoBreakSpaces();
    void testRCodeBlockHighlighting();
    void testCodeBlockLanguageAliases_data();
    void testCodeBlockLanguageAliases();
    void testCodeBlockInfoStrings();
    void testTildeSystemVerilogHighlighting();
    void testSqlCodeBlockHighlighting();
    void testForthCommentHighlighting();
    void testMultilineInlineHighlighting();
    void testMultilineInlineHighlightingDisabled();
    void testWhitespaceMarkerHighlighting();
    void testLinkedCheckBoxDetectionInReadOnlyEditor();
    void testPercentEncodedAttachmentLinkDetection();
    void testFootnoteNavigationAndHighlighting();
    void testAutoFormatTableAtCursor();
};

#endif    // TESTQMARKDOWNTEXTEDIT_H
