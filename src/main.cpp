#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
#include <QTextEdit>
#include <QSplitter>
#include <QVBoxLayout>
#include <QListView>
#include <QSystemTrayIcon>


int main(int argc, char *argv[])
{
    // fixing some troubles in Windows 8.1
#ifdef Q_OS_WIN32
    QCoreApplication::addLibraryPath("./");
#endif

    QApplication a(argc, argv);

    MainWindow w;
    w.show();
   // a.setQuitOnLastWindowClosed(false);
    return a.exec();
}
