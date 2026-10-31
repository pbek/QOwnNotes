#pragma once

#include <QObject>

class TestOpenAiService : public QObject {
    Q_OBJECT

   private slots:
    void init();
    void cleanup();
    void testBackendLookupPreservesSelection();
    void testIsolatedCompletion_data();
    void testIsolatedCompletion();
};
