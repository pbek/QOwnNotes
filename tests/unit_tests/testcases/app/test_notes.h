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
    void testFootnotesToMarkdownHtml();
    void testMalformedUnderlineToMarkdownHtml();
    void testMarkdownImageDimensionsToHtml();
    void testSearchQueryStringListModes();
    void testSearchInNotesModes();

    /* Preview Syntax Highlighter Tests */
    void testMarkdownTildeCodeFenceToHtml();
    void testMarkdownBacktickCodeFenceToHtml();
    void testCodeToHtmlConversionPython();
    void testCodeToHtmlConversionR();
    void testCodeToHtmlConversionHashComment();
    void testCodeToHtmlConversionConsole();
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
    void testWikiLinksShowInNoteRelations();

    /* Wiki-link backlink tests (issue #3535) */
    void testWikiLinkBacklinksShowInBacklinkPanel();

    /* Markdown link refactoring tests (issue #705) */
    void testMarkdownLinkTitleUpdatedOnNoteRename();

    /* Homepage suggestion API helper tests */
    void testBookmarkSuggestionsPrefixSubstringAndExact();
    void testBookmarkSuggestionsDeduplication();
    void testBookmarkSuggestionsMultiTokenAndOrderIndependent();
    void testBookmarkSuggestionsKeepNameUrlPairing();
    void testBookmarkSuggestionsIncludeMarkdownMetadata();
    void testBookmarkSuggestionsEmptyQuery();
    void testBookmarkSuggestionsLimitHandling();
    void testBookmarkSuggestionsResponseShape();

    /* Command snippet parsing tests */
    void testCommandSnippetsKeepNearestHeadingForCodeBlocks();

    /* Duplicate-title-collision fix (found via Joplin-import validation, not
     * import-specific -- affects any note creation/rename) */
    void testDuplicateTitleInNonActiveSubfolderGetsSuffixedNotOverwritten();
    void testDuplicateTitleInActiveSubfolderStillGetsSuffixed();
    void testRenameNoteFileToExistingNameInNonActiveSubfolder();
    void testEditingExistingNoteTitleToMatchAnotherNoteDestroysItsContent();

    /* Follow-up fixes for pbek's review comments on this PR */
    void testFetchByFileNameExcludesGivenNoteIdAmongDuplicates();
    void testCanWriteToNoteFileSucceedsWithoutReadPermission();
};

#endif    // TESTNOTES_H
