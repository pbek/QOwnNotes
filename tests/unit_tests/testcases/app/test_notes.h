#ifndef TESTNOTES_H
#define TESTNOTES_H

// #include "basetestcase.h"
// #include "models/key-models/keyfactory.h"
#include <QObject>
#include <QUrl>

#include "entities/note.h"

class TestNotes : public QObject {
    Q_OBJECT
   private:
    QString notesPath;
    QString noteFile;
    QString noteName;
    QString noteFileName;

   private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testNoteCreating();
    void testNoteEncryption();
    void testNoteDecryption();
    void testNoteDecryptionFail();
    void testNoteToMarkdownHtml();

    /* Preview Syntax Highlighter Tests */
    void testMarkdownTildeCodeFenceToHtml();
    void testMarkdownBacktickCodeFenceToHtml();
    void testCodeToHtmlConversionPython();
    void testCodeToHtmlConversionHashComment();
    void testCodeToHtmlConversionSingleLineComment();
    void testCodeToHtmlConversionMultiLineComment();
    void testCodeToHtmlNumericLiterals();
    void testCodeToHtmlNumericEquations();
    void testCodeToHtmlStringLiterals();
    void testOctal();
    void testHex();
    void testHTMLescape();

    /* Code block angle-bracket protection tests (issue #3084) */
    void testAngleBracketsInCodeBlocksNotConvertedToLinks();

    /* Nested fence tests (issue #2671) */
    void testBacktickBlockInsideTildeFenceNotDoubleHighlighted();
    void testBacktickBlockInsideIndentedCodeNotHighlighted();

    /* File URL handling tests (issue #3483) */
    void testPercentEncodedFileUrlUsesDecodedLocalPath();

    /* Homepage suggestion API helper tests */
    void testBookmarkSuggestionsPrefixSubstringAndExact();
    void testBookmarkSuggestionsDeduplication();
    void testBookmarkSuggestionsMultiTokenAndOrderIndependent();
    void testBookmarkSuggestionsKeepNameUrlPairing();
    void testBookmarkSuggestionsEmptyQuery();
    void testBookmarkSuggestionsLimitHandling();
    void testBookmarkSuggestionsResponseShape();
};

#endif    // TESTNOTES_H
