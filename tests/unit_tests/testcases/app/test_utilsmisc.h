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
};

#endif    // TESTUTILSMISC_H
