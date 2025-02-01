#pragma once

#include <QObject>

class TestMetricsService : public QObject {
    Q_OBJECT

   private Q_SLOTS:
    void initTestCase();
#ifndef Q_OS_LINUX
    void testSendEvent();
#endif
    void testSendVisit();
};
