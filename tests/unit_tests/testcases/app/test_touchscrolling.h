#ifndef TESTTOUCHSCROLLING_H
#define TESTTOUCHSCROLLING_H

#include <QObject>

class TestTouchScrolling : public QObject {
    Q_OBJECT

   private Q_SLOTS:
    void testPlainTextScrollCoordinates_data();
    void testPlainTextScrollCoordinates();
    void testPreviewScrollCoordinates();
    void testTouchDrag();
    void testMouseWheel();
    void testMouseSelection();
};

#endif    // TESTTOUCHSCROLLING_H
