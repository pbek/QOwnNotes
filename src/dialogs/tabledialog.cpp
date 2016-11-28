#include "tabledialog.h"
#include "ui_tabledialog.h"
#include <QDebug>
#include <mainwindow.h>

TableDialog::TableDialog(QWidget *parent) :
        MasterDialog(parent),
    ui(new Ui::TableDialog) {
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(Tab::CreateTab);
    ui->createTableWidget->setColumnCount(50);
    ui->createTableWidget->setRowCount(100);

    // Todo: remove and implement
    ui->tabWidget->removeTab(Tab::ImportTab);
}

TableDialog::~TableDialog() {
    delete ui;
}

/**
 * Updates the row and column spin boxes with the selection
 */
void TableDialog::on_createTableWidget_itemSelectionChanged() {
    // clear the spin boxes and return if nothing was selected
    if (ui->createTableWidget->selectedRanges().count() == 0) {
        ui->rowSpinBox->clear();
        ui->columnSpinBox->clear();
        return;
    }

    QTableWidgetSelectionRange range = ui->createTableWidget->
            selectedRanges().first();
    ui->rowSpinBox->setValue(range.rowCount());
    ui->columnSpinBox->setValue(range.columnCount());
}

/**
 * Creates the table markdown code
 */
void TableDialog::on_buttonBox_accepted() {
    switch (ui->tabWidget->currentIndex()) {
        default:
        case Tab::CreateTab:
            if ((ui->rowSpinBox->value() == 0) ||
                    (ui->columnSpinBox->value() == 0)) {
                return;
            }

            QString text;
            QString space = QString(" ").repeated(
                    ui->columnWidthSpinBox->value());
            QString headline = QString("-").repeated(
                    ui->separatorColumnWidthSpinBox->value());

            for (int row = 0; row < ui->rowSpinBox->value(); row++) {
                // add all columns of the row
                for (int col = 0; col < ui->columnSpinBox->value(); col++) {
                    text += "|" + space;
                }

                text += "|\n";

                // add the headline separator row
                if ((row == 0) && ui->headlineCheckBox->isChecked()) {
                    for (int col = 0; col < ui->columnSpinBox->value(); col++) {
                        text += "|" + headline;
                    }

                    text += "|\n";
                }
            }

            // insert the table into the text edit
            MainWindow::instance()->activeNoteTextEdit()->insertPlainText(text);
            break;
    }
}

/**
 * Shows or hides the separator column width ui elements
 *
 * @param checked
 */
void TableDialog::on_headlineCheckBox_toggled(bool checked) {
    ui->separatorColumnWidthSpinBox->setVisible(checked);
    ui->separatorColumnWidthLabel->setVisible(checked);
}
