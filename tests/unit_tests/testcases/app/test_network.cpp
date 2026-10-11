#include "test_network.h"

#include <QUrl>
#include <QtTest>

#include "services/cloudservice.h"
#include "utils/misc.h"

void TestNetwork::initTestCase() {}

void TestNetwork::testQOwnNotesApiDiscovery() {
    QCOMPARE(
        CloudService::apiBaseFromCapabilities(
            R"({"ocs":{"data":{"capabilities":{"qownnotes":{"api_base":"/index.php/apps/qownnotes/api/v1/"}}}}})"),
        QStringLiteral("/index.php/apps/qownnotes/api/v1/"));
    for (
        const QByteArray &data :
        {QByteArray("{}"), QByteArray("<ocs/>"),
         QByteArray(
             R"({"ocs":{"data":{"capabilities":{"qownnotes":{"api_base":"https://evil.test/"}}}}})"),
         QByteArray(
             R"({"ocs":{"data":{"capabilities":{"qownnotes":{"api_base":"//evil.test/"}}}}})")}) {
        QCOMPARE(CloudService::apiBaseFromCapabilities(data),
                 QStringLiteral("/index.php/apps/qownnotesapi/api/v1/"));
    }
}

// Disabled on Linux because of https://github.com/pbek/QOwnNotes/issues/3212
#ifndef Q_OS_LINUX
void TestNetwork::testSSL() {
    auto data = Utils::Misc::downloadUrl(QUrl("https://www.github.com"));

    QVERIFY(data.size() > 0);
}
#endif
