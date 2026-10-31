#pragma once

#include <QObject>

class TestOpenAiService : public QObject {
    Q_OBJECT

   private slots:
    void init();
    void cleanup();
    void testBackendLookupPreservesSelection();
    void testOpenRouterBackend();
    void testOpenRouterSettings();
    void testIsolatedCompletion_data();
    void testIsolatedCompletion();
};
