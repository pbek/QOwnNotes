#pragma once

#include <QJsonObject>

#include "masterdialog.h"

namespace Ui {
class ServerBookmarksImportDialog;
}

class QJSValue;

class ServerBookmarksImportDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit ServerBookmarksImportDialog(const QJSValue &bookmarks, QWidget *parent = 0);
    ~ServerBookmarksImportDialog();

   private slots:
    void on_importButton_clicked();

   private:
    Ui::ServerBookmarksImportDialog *ui;
    QJsonObject jsonObject;
};
