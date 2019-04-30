#ifndef SERVERBOOKMARKSIMPORTDIALOG_H
#define SERVERBOOKMARKSIMPORTDIALOG_H

#include <QDialog>
#include <QJSValue>
#include <mainwindow.h>
#include "masterdialog.h"

namespace Ui {
class ServerBookmarksImportDialog;
}

class ServerBookmarksImportDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit ServerBookmarksImportDialog(QJSValue bookmarks,
            MainWindow *mainWindow, QWidget *parent = 0);
    ~ServerBookmarksImportDialog();

private:
    Ui::ServerBookmarksImportDialog *ui;
};

#endif // SERVERBOOKMARKSIMPORTDIALOG_H
