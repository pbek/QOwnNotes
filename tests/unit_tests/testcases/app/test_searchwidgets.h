#ifndef TESTSEARCHWIDGETS_H
#define TESTSEARCHWIDGETS_H

#include <QObject>

class TestSearchWidgets : public QObject {
    Q_OBJECT

   private Q_SLOTS:
    void testFailedSearchPreservesPosition_data();
    void testFailedSearchPreservesPosition();
    void testSuccessfulSearchWraps_data();
    void testSuccessfulSearchWraps();
    void testFailedSelectionOnlySearchPreservesPosition();
};

#endif    // TESTSEARCHWIDGETS_H
