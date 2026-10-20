#ifndef TESTTODOSETTINGSWIDGET_H
#define TESTTODOSETTINGSWIDGET_H

#include <QObject>

class TestTodoSettingsWidget : public QObject {
    Q_OBJECT

   private Q_SLOTS:
    void init();
    void cleanup();
    void testCachedSelectionsSurviveReadAndStore_data();
    void testCachedSelectionsSurviveReadAndStore();
    void testInvalidRefreshPreservesSelections_data();
    void testInvalidRefreshPreservesSelections();
    void testRefreshPreservesCheckedState();
    void testRefreshPreservesAbsoluteUrls();
    void testValidEmptyRefreshClearsCalendars();
    void testDisabledSupportDoesNotRequestReload();
    void testEmptyCacheRequestsReload();
    void testBackendReloadIsDeferred();
};

#endif    // TESTTODOSETTINGSWIDGET_H
