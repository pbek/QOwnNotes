#pragma once

#include <QDialog>
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

private:
    enum Tab {
        CreateTab,
        ImportTab
    };

    Ui::TableDialog *ui;

    void createMarkdownTable();

    void importCSV();
};
