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

    QApplication a(argc, argv);

    QString locale = QLocale::system().name().section('_', 0, 0);

    QTranslator translator;
    // TODO(pbek): this has to be adapted for build systems
    translator.load("../src/languages/QOwnNotes_" + locale);

    a.installTranslator(&translator);

    MainWindow w;
    w.show();
    return a.exec();
}
