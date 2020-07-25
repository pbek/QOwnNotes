#include "test_network.h"

#include <QUrl>
#include <QtTest>

#include "utils/misc.h"

void TestNetwork::initTestCase() {}

void TestNetwork::testSSL() {
    // TLS checks fail on GitHub Actions for Linux Qt <= 5.7
#if !defined(Q_OS_LINUX) || (QT_VERSION > QT_VERSION_CHECK(5, 7, 0))
    auto data = Utils::Misc::downloadUrl(QUrl("https://www.github.com"));

    QVERIFY(data.size() > 0);
#endif
}
