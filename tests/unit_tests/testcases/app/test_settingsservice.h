#pragma once

#include <QObject>

class TestSettingsService : public QObject {
    Q_OBJECT

   private Q_SLOTS:
    void initTestCase();
    void testClear();
    void testSimpleSetAndRead();
    void testGroupRemove();
    void testGroupRead();
    void testGroupSet();
    void testArraySetAndRead();
};
