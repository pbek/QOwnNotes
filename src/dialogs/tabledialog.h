#pragma once

#include <QDialog>
#include <QTableWidget>
#include "masterdialog.h"

namespace Ui {
class TableDialog;
}

class TableDialog : public MasterDialog
{
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

private:
    enum Tab {
        CreateTab,
        ImportTab
    };

    Ui::TableDialog *ui;
    int _maxColumns = 0;
    int _maxRows = 0;

    void createMarkdownTable();
    void importCSV();
    void updateMaxItems();
};
