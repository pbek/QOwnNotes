#include "test_metricsservice.h"

#include <QtTest>

#include "services/metricsservice.h"

void TestMetricsService::initTestCase() { MetricsService::createInstance(this); }

void TestMetricsService::testSendEvent() {
    MetricsService::instance()->sendEventIfEnabled("test/event", "test", "event");
    QVERIFY(true);
}

void TestMetricsService::testSendVisit() {
    MetricsService::instance()->sendVisitIfEnabled("test/visit", "TestScreen");
    QVERIFY(true);
}
