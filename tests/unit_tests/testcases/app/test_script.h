#ifndef TESTSCRIPT_H
#define TESTSCRIPT_H

#include <QObject>

class TestScript : public QObject {
    Q_OBJECT

   private Q_SLOTS:
    void testChangelogForVersionRange();
    void testChangelogVersionOnlyHeadings();
    void testChangelogForVersionRangeIgnoresNonVersionHeadings();
    void testChangelogHtmlForVersionRange();
    void testRepositoryUrlsUseMainBranch();
};

#endif    // TESTSCRIPT_H
