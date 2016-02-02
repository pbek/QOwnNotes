#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
#include <QTranslator>


int main(int argc, char *argv[])
{
    // fixing some troubles in Windows 8.1
#ifdef Q_OS_WIN32
    QCoreApplication::addLibraryPath("./");
#endif

    QApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QString locale = QLocale::system().name().section('_', 0, 0);
//    locale = "de";
    qDebug() << __func__ << " - 'locale': " << locale;

#ifndef QT_DEBUG
    QTranslator translatorRelease;
    translatorRelease.load(
        "/usr/share/QOwnNotes/languages/QOwnNotes_" + locale);
    app.installTranslator(&translatorRelease);
#endif

    QTranslator translator1;
    translator1.load(QCoreApplication::applicationDirPath() +
                             "/../src/languages/QOwnNotes_" + locale);

    QTranslator translator2;
    translator2.load(QCoreApplication::applicationDirPath() +
                             "/../languages/QOwnNotes_" + locale);
    app.installTranslator(&translator2);

    QTranslator translator3;
    translator3.load(QCoreApplication::applicationDirPath() +
                             "/languages/QOwnNotes_" + locale);
    app.installTranslator(&translator3);

    QTranslator translator4;
    translator4.load(QCoreApplication::applicationDirPath() +
                             "/QOwnNotes_" + locale);
    app.installTranslator(&translator4);

    QTranslator translator5;
    translator5.load("../src/languages/QOwnNotes_" + locale);
    app.installTranslator(&translator5);

    QTranslator translatorLocal;
    translatorLocal.load("QOwnNotes_" + locale);
    app.installTranslator(&translatorLocal);

    MainWindow w;
    w.show();
    return app.exec();
}
