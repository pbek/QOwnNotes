#ifndef TESTNOTES_H
#define TESTNOTES_H

// #include "basetestcase.h"
// #include "models/key-models/keyfactory.h"
#include <QObject>
#include <QUrl>
#include <QVariant>

#include "entities/note.h"
#include "entities/notesubfolder.h"

class TestNotes : public QObject {
    Q_OBJECT
   private:
    QString notesPath;
    QString noteFile;
    QString noteName;
    QString noteFileName;
    QVariant wikiLinkSupportSetting;

    QString uniqueTestName(const QString &baseName) const;
    Note createTestNote(const QString &name, int noteSubFolderId = 0,
                        const QString &text = QString()) const;
    NoteSubFolder createTestNoteSubFolder(const QString &name, int parentId = 0) const;

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
    void testXmlHighlighterEscaping();

    /* Code block angle-bracket protection tests (issue #3084) */
    void testAngleBracketsInCodeBlocksNotConvertedToLinks();

    /* Nested fence tests (issue #2671) */
    void testBacktickBlockInsideTildeFenceNotDoubleHighlighted();
    void testBacktickBlockInsideIndentedCodeNotHighlighted();

    /* File URL handling tests (issue #3483) */
    void testPercentEncodedFileUrlUsesDecodedLocalPath();

    /* Wiki-link tests (issue #3512) */
    void testWikiLinkSupportDisabledLeavesPlainText();
    void testResolveWikiLinkPrefersCurrentSubfolderAndFindsNestedNotes();
    void testWikiLinkHtmlRenderingMarksResolvedAndBrokenLinks();
    void testQualifiedWikiLinksAreUpdatedOnSubfolderRename();

    /* Homepage suggestion API helper tests */
    void testBookmarkSuggestionsPrefixSubstringAndExact();
    void testBookmarkSuggestionsDeduplication();
    void testBookmarkSuggestionsMultiTokenAndOrderIndependent();
    void testBookmarkSuggestionsKeepNameUrlPairing();
    void testBookmarkSuggestionsIncludeMarkdownMetadata();
    void testBookmarkSuggestionsEmptyQuery();
    void testBookmarkSuggestionsLimitHandling();
    void testBookmarkSuggestionsResponseShape();
};

#endif    // TESTNOTES_H
