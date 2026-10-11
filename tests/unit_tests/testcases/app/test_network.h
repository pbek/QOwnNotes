#pragma once

#include <QObject>

class TestNetwork : public QObject {
    Q_OBJECT

   private Q_SLOTS:
    void initTestCase();
    void testQOwnNotesApiDiscovery();
#ifndef Q_OS_LINUX
    void testSSL();
#endif
};
