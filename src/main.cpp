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
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
   // a.setQuitOnLastWindowClosed(false);
    return a.exec();
}
