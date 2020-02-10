#include "test_htmlentities.h"
#include <QCoreApplication>
#include <QString>
#include <QtTest>

void TestHTMLEntities::initTestCase() {}

void TestHTMLEntities::testEntities() {
    QString text1 = "Ich bin ein &amp;";
    QString text2 = htmlEntities.decodeHtmlEntities(text1);
    QVERIFY(text2 == "Ich bin ein &");
}
