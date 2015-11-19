#include <QTest>
#include <QApplication>

//tests
#include <iostream>
#include "testcases/app/test_notes.h"
#include "testcases/app/test_htmlentities.h"

int main(int argc, char *argv[])
{
	QApplication app( argc, argv );   

    int allTestsResult = 0
        + QTest::qExec(new TestNotes(), argc, argv)
        + QTest::qExec(new TestHTMLEntities(), argc, argv)
        ;

    if (allTestsResult == 0)
        qDebug() << "[Tests PASS]";
    else
        qDebug() << "[Tests FAIL]";

    return (allTestsResult != 0 )? 1 : 0;
}
