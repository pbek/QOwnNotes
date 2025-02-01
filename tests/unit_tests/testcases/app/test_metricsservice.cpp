#include "test_metricsservice.h"

#include <QtTest>

#include "services/metricsservice.h"

void TestMetricsService::initTestCase() { MetricsService::createInstance(this); }

// Disabled on Linux because of https://github.com/pbek/QOwnNotes/issues/3212
#ifndef Q_OS_LINUX
void TestMetricsService::testSendEvent() {
    MetricsService::instance()->sendEventIfEnabled("test/event", "test", "event");
    QVERIFY(true);
}
#endif

void TestMetricsService::testSendVisit() {
    MetricsService::instance()->sendVisitIfEnabled("test/visit", "TestScreen");
    QVERIFY(true);
}
