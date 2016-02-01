#include <QtTest>
#include "services/metricsservice.h"
#include "test_metricsservice.h"


void TestMetricsService::initTestCase()
{
    MetricsService *metricsService = MetricsService::createInstance(this);
}

void TestMetricsService::testSendEvent()
{
    MetricsService::instance()->sendEvent("tests", "test event");
    QVERIFY(true);
}

void TestMetricsService::testSendAppView()
{
    MetricsService::instance()->sendAppView("TestScreen");
    QVERIFY(true);
}
