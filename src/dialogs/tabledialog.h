#pragma once

#include <QTemporaryFile>

#include "masterdialog.h"

namespace Ui {
class TableDialog;
}

class QTableWidgetItem;

class TableDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit TableDialog(QWidget *parent = 0);
    ~TableDialog();

   private slots:
    void on_createTableWidget_itemSelectionChanged();

    void on_buttonBox_accepted();

    void on_headlineCheckBox_toggled(bool checked);

    void on_fileButton_clicked();

    void on_createTableWidget_itemChanged(QTableWidgetItem *item);

    void on_clipboardButton_clicked();

   private:
    enum Tab { CreateTab, ImportTab };

    Ui::TableDialog *ui;
    int _maxColumns = 0;
    int _maxRows = 0;
    QTemporaryFile *_tempFile;

    void createMarkdownTable();
    void importCSV();
    void updateMaxItems();
    void updateSeparator(const QString &text) const;
};
