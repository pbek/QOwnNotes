#include "tabledialog.h"

#include <mainwindow.h>

#include <QDebug>

#include "filedialog.h"
#include "libraries/qtcsv/src/include/reader.h"
#include "ui_tabledialog.h"
#include "widgets/qownnotesmarkdowntextedit.h"

TableDialog::TableDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::TableDialog) {
    ui->setupUi(this);

    // ignore the return key so we can better edit text in the table
    setIgnoreReturnKey(true);

    ui->tabWidget->setCurrentIndex(Tab::CreateTab);
    ui->createTableWidget->setColumnCount(50);
    ui->createTableWidget->setRowCount(100);
    ui->csvFileTextEdit->setVisible(false);
}

TableDialog::~TableDialog() { delete ui; }

/**
 * Updates the row and column spin boxes with the selection
 */
void TableDialog::on_createTableWidget_itemSelectionChanged() {
    /*
    // clear the spin boxes and return if nothing was selected
    if (ui->createTableWidget->selectedRanges().count() == 0) {
        ui->rowSpinBox->clear();
        ui->columnSpinBox->clear();
        return;
    }
    */

    updateMaxItems();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QTableWidgetSelectionRange range =
        ui->createTableWidget->selectedRanges().constFirst();
#else
    QTableWidgetSelectionRange range =
        ui->createTableWidget->selectedRanges().first();
#endif

    ui->rowSpinBox->setValue(std::max(_maxRows, range.rowCount()));
    ui->columnSpinBox->setValue(std::max(_maxColumns, range.columnCount()));
}

void TableDialog::updateMaxItems() {
    for (int row = 0; row < ui->createTableWidget->rowCount(); row++) {
        for (int col = 0; col < ui->createTableWidget->columnCount(); col++) {
            auto item = ui->createTableWidget->item(row, col);
            bool hasText = item != nullptr ? !item->text().isEmpty() : false;

            if (hasText) {
                _maxRows = std::max(_maxRows, row + 1);
                _maxColumns = std::max(_maxColumns, col + 1);
            }
        }
    }
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
    QString text = QStringLiteral("\n\n");
    QString separator = ui->separatorComboBox->currentText();
    separator.replace("\\t", "\t");

    // read data from file
    QList<QStringList> readData = QtCSV::Reader::readToList(
        filePath, separator,
        ui->textDelimiterComboBox->currentText());

    // loop through all rows
    for (int row = 0; row < readData.size(); ++row) {
        QStringList data = readData.at(row);

        // add a table row
        text += "| " + data.join(QStringLiteral(" | ")) + " |\n";

        // add the headline separator if needed
        if ((row == 0) && ui->firstLineHeadlineCheckBox->isChecked()) {
            for (int col = 0; col < data.count(); col++) {
                text += QLatin1String("| --- ");
            }

            text += QLatin1String("|\n");
        }
    }

    // insert the table into the text edit
    MainWindow::instance()->activeNoteTextEdit()->insertPlainText(text);
}

/**
 * Creates the markdown table
 */
void TableDialog::createMarkdownTable() {
    if ((ui->rowSpinBox->value() == 0) || (ui->columnSpinBox->value() == 0)) {
        return;
    }

    // start with two newlines to make sure that a proper table is inserted
    QString text = QStringLiteral("\n\n");
    int colWidth = ui->columnWidthSpinBox->value();
    QString space = QStringLiteral(" ").repeated(colWidth);
    QString headline =
        QStringLiteral("-").repeated(ui->separatorColumnWidthSpinBox->value());

    for (int row = 0; row < ui->rowSpinBox->value(); row++) {
        // add all columns of the row
        for (int col = 0; col < ui->columnSpinBox->value(); col++) {
            auto item = ui->createTableWidget->item(row, col);
            QString itemText = item != nullptr ? item->text() : QString();
            text +=
                QStringLiteral("|") +
                (itemText.isEmpty() ? space
                                    : itemText.leftJustified(colWidth, ' '));
        }

        text += QStringLiteral("|\n");

        // add the headline separator row
        if ((row == 0) && ui->headlineCheckBox->isChecked()) {
            for (int col = 0; col < ui->columnSpinBox->value(); col++) {
                text += QStringLiteral("|") + headline;
            }

            text += QStringLiteral("|\n");
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
    FileDialog dialog(QStringLiteral("CSVTableImport"));
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

void TableDialog::on_createTableWidget_itemChanged(QTableWidgetItem *item) {
    if (item == nullptr) {
        return;
    }

    int columns = item->column() + 1;
    if (columns > ui->columnSpinBox->value()) {
        ui->columnSpinBox->setValue(columns);
    }

    int rows = item->row() + 1;
    if (rows > ui->rowSpinBox->value()) {
        ui->rowSpinBox->setValue(rows);
    }

    int length = item->text().length();
    if (length > ui->columnWidthSpinBox->value()) {
        ui->columnWidthSpinBox->setValue(length);
        ui->separatorColumnWidthSpinBox->setValue(length);
    }
}
