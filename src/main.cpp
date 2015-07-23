#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
#include <QTextEdit>
#include <QSplitter>
#include <QVBoxLayout>
#include <QListView>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
