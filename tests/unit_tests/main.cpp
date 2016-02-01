#include <QTest>
#include <QApplication>

//tests
#include <iostream>
#include "version.h"
#include "release.h"
#include "testcases/app/test_notes.h"
#include "testcases/app/test_htmlentities.h"
#include "testcases/app/test_metricsservice.h"

int main(int argc, char *argv[])
{
	QApplication app( argc, argv );   

    QCoreApplication::setOrganizationDomain("PBE");
    QCoreApplication::setOrganizationName("PBE");
    QCoreApplication::setApplicationName("QOwnNotesTests");
    QCoreApplication::setApplicationVersion(
            QString(VERSION) + " " + QString(RELEASE));

    int allTestsResult = 0
        + QTest::qExec(new TestNotes(), argc, argv)
        + QTest::qExec(new TestHTMLEntities(), argc, argv)
        + QTest::qExec(new TestMetricsService(), argc, argv)
        ;

    if (allTestsResult == 0)
        qDebug() << "[Tests PASS]";
    else
        qDebug() << "[Tests FAIL]";

    return (allTestsResult != 0 )? 1 : 0;
}
