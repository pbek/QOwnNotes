#include "test_network.h"

#include <QUrl>
#include <QtTest>

#include "utils/misc.h"

void TestNetwork::initTestCase() {}

// Disabled on Linux because of https://github.com/pbek/QOwnNotes/issues/3212
#ifndef Q_OS_LINUX
void TestNetwork::testSSL() {
    auto data = Utils::Misc::downloadUrl(QUrl("https://www.github.com"));

    QVERIFY(data.size() > 0);
}
#endif
