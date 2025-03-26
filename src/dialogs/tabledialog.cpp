#include "tabledialog.h"

#include <mainwindow.h>

#include <QClipboard>
#include <QDebug>

#include "filedialog.h"
#include "libraries/qtcsv/src/include/reader.h"
#include "ui_tabledialog.h"
#include "widgets/qownnotesmarkdowntextedit.h"

TableDialog::TableDialog(QWidget *parent) : MasterDialog(parent), ui(new Ui::TableDialog) {
    ui->setupUi(this);
    afterSetupUI();

    // ignore the return key so we can better edit text in the table
    setIgnoreReturnKey(true);

    ui->tabWidget->setCurrentIndex(Tab::CreateTab);
    ui->createTableWidget->setColumnCount(50);
    ui->createTableWidget->setRowCount(100);
    ui->csvFileTextEdit->setVisible(false);
    _tempFile = nullptr;
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
    const QList<QTableWidgetSelectionRange> &list = ui->createTableWidget->selectedRanges();

    if (list.isEmpty()) {
        return;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QTableWidgetSelectionRange range = list.constFirst();
#else
    QTableWidgetSelectionRange range = list.first();
#endif

    ui->rowSpinBox->setValue(std::max<int>(_maxRows, range.rowCount()));
    ui->columnSpinBox->setValue(std::max<int>(_maxColumns, range.columnCount()));
}

void TableDialog::updateMaxItems() {
    for (int row = 0; row < ui->createTableWidget->rowCount(); row++) {
        for (int col = 0; col < ui->createTableWidget->columnCount(); col++) {
            auto item = ui->createTableWidget->item(row, col);
            bool hasText = item != nullptr ? !item->text().isEmpty() : false;

            if (hasText) {
                _maxRows = std::max<int>(_maxRows, row + 1);
                _maxColumns = std::max<int>(_maxColumns, col + 1);
            }
        }
    }
}

/**
 * Creates the table Markdown code
 */
void TableDialog::on_buttonBox_accepted() {
    switch (ui->tabWidget->currentIndex()) {
        case Tab::ImportTab:
            // import the CSV file
            importCSV();
            break;

        default:
        case Tab::CreateTab:
            // create the Markdown table
            createMarkdownTable();
            break;
    }
}

/**
 * Imports the CSV file
 */
void TableDialog::importCSV() {
    QString filePath = _tempFile != nullptr ? _tempFile->fileName() : ui->fileLineEdit->text();

    if (filePath.isEmpty()) {
        return;
    }

    // start with two newlines to make sure that a proper table is inserted
    QString text = QStringLiteral("\n\n");
    QString separator = ui->separatorComboBox->currentText();
    separator.replace("\\t", "\t");

    // read data from file
    QList<QStringList> readData =
        QtCSV::Reader::readToList(filePath, separator, ui->textDelimiterComboBox->currentText());

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
 * Creates the Markdown table
 */
void TableDialog::createMarkdownTable() {
    if ((ui->rowSpinBox->value() == 0) || (ui->columnSpinBox->value() == 0)) {
        return;
    }

    // start with two newlines to make sure that a proper table is inserted
    QString text = QStringLiteral("\n\n");
    int colWidth = ui->columnWidthSpinBox->value();
    QString space = QStringLiteral(" ").repeated(colWidth);
    QString headline = QStringLiteral("-").repeated(ui->separatorColumnWidthSpinBox->value());

    for (int row = 0; row < ui->rowSpinBox->value(); row++) {
        // add all columns of the row
        for (int col = 0; col < ui->columnSpinBox->value(); col++) {
            auto item = ui->createTableWidget->item(row, col);
            QString itemText = item != nullptr ? item->text() : QString();
            text += QStringLiteral("|") +
                    (itemText.isEmpty() ? space : itemText.leftJustified(colWidth, ' '));
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
    QStringList filters = QStringList() << tr("CSV files") + " (*.csv)" << tr("All files") + " (*)";
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
            const QByteArray &text = file.readAll();
            ui->csvFileTextEdit->setPlainText(text);

            updateSeparator(text);

            _tempFile = nullptr;
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

void TableDialog::on_clipboardButton_clicked() {
    QClipboard *clipboard = QApplication::clipboard();
    QString text = clipboard->text().trimmed();

    // set text from clipboard
    if (!text.isEmpty()) {
        ui->fileLineEdit->clear();
        ui->csvFileTextEdit->show();
        ui->csvFileTextEdit->setPlainText(text);

        _tempFile = new QTemporaryFile(QDir::tempPath() + QDir::separator() +
                                       QStringLiteral("table-XXXXXX.csv"));

        if (!_tempFile->open()) {
            _tempFile = nullptr;

            return;
        }

        updateSeparator(text);

        // write file data to the temporary file in UTF-8
        _tempFile->write(text.toUtf8());

        // close the file so the CSV reader can access it
        _tempFile->close();
    }
}

void TableDialog::updateSeparator(const QString &text) const {
    const QStringList list = {"\t", ";", ","};

    foreach (const QString &character, list) {
        if (text.contains(character)) {
            // set separator if found
            ui->separatorComboBox->setCurrentText(character == "\t" ? "\\t" : character);

            return;
        }
    }
}
