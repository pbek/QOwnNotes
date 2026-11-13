#pragma once

#include <QObject>

class TestNoteFolderDetection : public QObject {
    Q_OBJECT

   private Q_SLOTS:
    void testKnownNotesFolderNames_data();
    void testKnownNotesFolderNames();
    void testFindsCandidates();
    void testSortsCandidates();
    void testCapsNoteCount();
    void testCountsOnlyDirectSubfolders();
    void testUsesNoteFileExtensions();
    void testExcludesPaths();
    void testDeduplicatesCandidates();
    void testIgnoresMissingRoots();
};
