#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include "test_htmlentities.h"

void TestHTMLEntities::initTestCase()
{
}

void TestHTMLEntities::testEntities()
{
    QString text1 = "Ich bin ein &amp;";
    QString text2 = htmlEntities.decodeHtmlEntities( text1 );
    QVERIFY( text2 == "Ich bin ein &" );
}
