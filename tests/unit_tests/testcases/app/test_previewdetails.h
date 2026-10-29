#pragma once

#include <QObject>

class TestPreviewDetails : public QObject {
    Q_OBJECT
   private Q_SLOTS:
    void toggleSections();
    void nestedSections();
    void tokenization_data();
    void tokenization();
    void preserveState();
    void exportAllContent();
    void markdownExample();
    void previewLinks();
    void revealAnchor();
    void notesWithoutDetailsAreUnchanged();
    void previewAnchorScrolling();
    void largeDocumentPerformance();
#ifdef USE_QLITEHTML
    void qliteHtmlPreviewLinks();
#endif
};
