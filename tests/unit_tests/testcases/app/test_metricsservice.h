#pragma once

#include <QObject>

class TestMetricsService : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testSendEvent();
    void testSendAppView();
};
