#pragma once

#include <QObject>

class TestNextcloudDeckService : public QObject {
    Q_OBJECT

   private slots:
    void testMoveCard_data();
    void testMoveCard();
    void testInvalidMove();
};
