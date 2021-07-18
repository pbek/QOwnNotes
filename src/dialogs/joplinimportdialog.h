#pragma once

#include "masterdialog.h"

namespace Ui {
class JoplinImportDialog;
}

class Note;
class QXmlQuery;
class QTreeWidgetItem;

class JoplinImportDialog : public MasterDialog {
    Q_OBJECT

    struct MediaFileData {
        QString data;
        QString suffix;
        QString fileName;
    };

   public:
    explicit JoplinImportDialog(QWidget *parent = 0);
    ~JoplinImportDialog() override;

    int getImportCount() const;

   private slots:
    void on_directoryButton_clicked();

    void on_importButton_clicked();

   private:
    Ui::JoplinImportDialog *ui;
    int _importCount;

    static bool importNote(const QString& text, const QString& dirPath);
};
