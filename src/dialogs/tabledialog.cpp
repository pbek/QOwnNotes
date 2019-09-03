#include "tabledialog.h"
#include "ui_tabledialog.h"
#include <QDebug>
#include <mainwindow.h>
#include "libraries/qtcsv/src/include/reader.h"
#include "filedialog.h"


TableDialog::TableDialog(QWidget *parent) :
        MasterDialog(parent),
    ui(new Ui::TableDialog) {
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(Tab::CreateTab);
    ui->createTableWidget->setColumnCount(50);
    ui->createTableWidget->setRowCount(100);
    ui->csvFileTextEdit->setVisible(false);
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
        case Tab::ImportTab:
            // import the CSV file
            importCSV();
            break;

        default:
        case Tab::CreateTab:
            // create the markdown table
            createMarkdownTable();
            break;
    }
}

/**
 * Imports the CSV file
 */
void TableDialog::importCSV() {
    QString filePath = ui->fileLineEdit->text();
    if (filePath.isEmpty()) {
        return;
    }

    // start with two newlines to make sure that a proper table is inserted
    QString text = "\n\n";

    // read data from file
    QList<QStringList> readData = QtCSV::Reader::readToList(
            filePath, ui->separatorComboBox->currentText(),
            ui->textDelimiterComboBox->currentText());

    // loop through all rows
    for (int row = 0; row < readData.size(); ++row) {
        QStringList data = readData.at(row);

        // add a table row
        text += "| " + data.join(" | ") + " |\n";

        // add the headline separator if needed
        if ((row == 0) && ui->firstLineHeadlineCheckBox->isChecked()) {
            for (int col = 0; col < data.count(); col++) {
                text += "| --- ";
            }

            text += "|\n";
        }
    }

    // insert the table into the text edit
    MainWindow::instance()->activeNoteTextEdit()->insertPlainText(text);
}

/**
 * Creates the markdown table
 */
void TableDialog::createMarkdownTable() {
    if ((ui->rowSpinBox->value() == 0) ||
        (ui->columnSpinBox->value() == 0)) {
        return;
    }

    // start with two newlines to make sure that a proper table is inserted
    QString text = "\n\n";
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

/**
 * Selects a CSV file to import
 */
void TableDialog::on_fileButton_clicked() {
    ui->csvFileTextEdit->clear();
    QStringList filters = QStringList() << tr("CSV files") + " (*.csv)"
                                        << tr("All files") + " (*)";
    FileDialog dialog("CSVTableImport");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilters(filters);
    dialog.setWindowTitle(tr("Select CSV file to import"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            ui->fileLineEdit->setText(fileName);

            QFile file(fileName);

            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qCritical() << file.errorString();
                return;
            }

            // load the file into the file text edit
            ui->csvFileTextEdit->show();
            ui->csvFileTextEdit->setPlainText(file.readAll());
        }
    }
}
