#pragma once

#include <QObject>

class TestNetwork : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testSSL();
};
