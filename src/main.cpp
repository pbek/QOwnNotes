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

    QString appPath = QCoreApplication::applicationDirPath();

    QTranslator translator1;
    translator1.load(appPath + "/../src/languages/QOwnNotes_" + locale);
    app.installTranslator(&translator1);

    QTranslator translator2;
    translator2.load(appPath + "/../languages/QOwnNotes_" + locale);
    app.installTranslator(&translator2);

    QTranslator translator3;
    translator3.load(appPath + "/languages/QOwnNotes_" + locale);
    app.installTranslator(&translator3);

    QTranslator translator4;
    translator4.load(appPath + "/QOwnNotes_" + locale);
    app.installTranslator(&translator4);

    QTranslator translator5;
    translator5.load("../src/languages/QOwnNotes_" + locale);
    app.installTranslator(&translator5);

    QTranslator translatorLocal;
    translatorLocal.load("QOwnNotes_" + locale);
    app.installTranslator(&translatorLocal);

#ifdef Q_OS_MAC
    QTranslator translatorOSX;
    translatorOSX.load(appPath + "/../Resources/QOwnNotes_" + locale);
    app.installTranslator(&translatorOSX);

    QTranslator translatorOSX2;
    translatorOSX2.load("../Resources/QOwnNotes_" + locale);
    app.installTranslator(&translatorOSX2);
#endif

    MainWindow w;
    w.show();
    return app.exec();
}
