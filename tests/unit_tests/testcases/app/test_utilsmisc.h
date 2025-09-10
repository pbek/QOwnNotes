#ifndef TESTUTILSMISC_H
#define TESTUTILSMISC_H

#include <QObject>

class TestUtilsMisc : public QObject {
    Q_OBJECT
   private Q_SLOTS:
    void initTestCase();
    void testRemoveIfStartsWith();
    void testRemoveIfEndsWith();
    void testPrependIfDoesNotStartWith();
    void testAppendIfDoesNotEndWith();
    void testStartDetachedProcess();
    void testShorten();
    void testCycleTextCase();
    void testHtmlToMarkdown();
    void testParseTaskList();
    void testUnescapeHtml();
    void testHtmlSpecialChars();
    void testToHumanReadableByteSize();
    void testTransformEvernoteImportCodeBlock();
    void testTransformEvernoteImportCodeBlock2();
    void testTransformEvernoteImportRussian();
    void testTransformEvernoteImportHyperlinks();
    void testGetBaseUrlFromUrlString();
    void testCreateAbsolutePathsInHtml();
    void testDetectFileFormatTxt();
    void testDetectFileFormatJson();
    void testDetectFileFormatXml();
    void testDetectFileFormatXmlWithoutDeclaration();
    void testDetectFileFormatHtml();
    void testDetectFileFormatCsv();
    void testDetectFileFormatIni();
    void testDetectFileFormatNix();
    void testDetectFileFormatYaml();
    void testDetectFileFormatSql();
    void testDetectFileFormatJavaScript();
    void testDetectFileFormatEdgeCases();
};

#endif    // TESTUTILSMISC_H
