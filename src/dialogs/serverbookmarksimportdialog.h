#pragma once

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

private slots:
    void on_importButton_clicked();

private:
    Ui::ServerBookmarksImportDialog *ui;
    QJsonObject jsonObject;
};
