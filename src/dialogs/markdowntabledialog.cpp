#include "markdowntabledialog.h"

#include <mainwindow.h>
#include <utils/gui.h>

#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCursor>

#include "ui_markdowntabledialog.h"

MarkdownTableDialog::MarkdownTableDialog(QPlainTextEdit *textEdit, QWidget *parent)
    : MasterDialog(parent), ui(new Ui::MarkdownTableDialog), _textEdit(textEdit) {
    ui->setupUi(this);
    afterSetupUI();

    // Ignore the return key so we can edit cells without closing the dialog
    setIgnoreReturnKey(true);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked |
                                     QAbstractItemView::EditKeyPressed |
                                     QAbstractItemView::AnyKeyPressed);

    // Clicking a column header selects the whole column and syncs the alignment combobox
    connect(ui->tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this,
            &MarkdownTableDialog::onColumnHeaderClicked);

    loadTableFromCursor();
}

MarkdownTableDialog::~MarkdownTableDialog() { delete ui; }

/**
 * Parses the Markdown table at the cursor position and populates the grid widget
 */
void MarkdownTableDialog::loadTableFromCursor() {
    if (_textEdit == nullptr) {
        return;
    }

    QTextCursor cursor = _textEdit->textCursor();
    QTextBlock block = cursor.block();

    // Verify the cursor is actually inside a table
    if (!block.text().trimmed().startsWith(QLatin1String("|"))) {
        return;
    }

    // Walk backwards to find the first row of the table
    while (true) {
        QTextBlock prev = block.previous();
        if (!prev.isValid() || !prev.text().trimmed().startsWith(QLatin1String("|"))) {
            break;
        }
        block = prev;
    }

    _tableStartPosition = block.position();

    // Collect all table lines
    QList<QStringList> rows;
    QTextBlock current = block;
    int endPos = block.position();

    while (current.isValid() && current.text().trimmed().startsWith(QLatin1String("|"))) {
        const QString &line = current.text().trimmed();
        rows << line.split(QLatin1Char('|'));
        endPos = current.position() + current.text().size();
        current = current.next();
    }

    _tableEndPosition = endPos;

    // Parse column alignments from the separator row (row index 1)
    _colAlignments.clear();
    static const QRegularExpression sepRe(QStringLiteral(R"(^(:)?-+(:)?$)"));

    if (rows.size() >= 2) {
        const QStringList &sepRow = rows.at(1);
        // Parts[0] is always empty (before the leading |), data starts at index 1
        for (int col = 1; col < sepRow.size() - 1; ++col) {
            const QString t = sepRow.at(col).trimmed();
            if (!sepRe.match(t).hasMatch()) {
                _colAlignments << Qt::AlignLeft;
                continue;
            }
            const bool left = t.startsWith(QLatin1Char(':'));
            const bool right = t.endsWith(QLatin1Char(':'));
            if (left && right) {
                _colAlignments << Qt::AlignHCenter;
            } else if (right) {
                _colAlignments << Qt::AlignRight;
            } else {
                _colAlignments << Qt::AlignLeft;
            }
        }
    }

    populateGridFromParsed(rows);
}

/**
 * Fills the QTableWidget from a list of parsed rows.
 * Row 0 = header row, Row 1 = separator (skipped in UI), Rows 2+ = data rows.
 * The grid shows: header row + data rows (separator is reconstructed on output).
 */
void MarkdownTableDialog::populateGridFromParsed(const QList<QStringList> &rows) {
    if (rows.isEmpty()) {
        return;
    }

    // Determine column count (parts[0] is empty before leading |, parts[last] may be empty
    // after trailing |)
    int maxCols = 0;
    for (const QStringList &row : rows) {
        // Subtract 2 for the leading and trailing empty strings around the pipes
        int dataCols = row.size() - 2;
        if (dataCols > maxCols) {
            maxCols = dataCols;
        }
    }

    if (maxCols <= 0) {
        return;
    }

    // Collect display rows: header (index 0) + data rows (index 2+), skip separator (index 1)
    QList<QStringList> displayRows;
    for (int i = 0; i < rows.size(); ++i) {
        if (i == 1) {
            // Skip the separator row — it will be reconstructed from _colAlignments
            continue;
        }
        displayRows << rows.at(i);
    }

    ui->tableWidget->setRowCount(displayRows.size());
    ui->tableWidget->setColumnCount(maxCols);

    for (int r = 0; r < displayRows.size(); ++r) {
        const QStringList &row = displayRows.at(r);
        for (int c = 0; c < maxCols; ++c) {
            // parts: [empty, col1, col2, ..., empty]  → data starts at index 1
            const int partIdx = c + 1;
            QString cellText;
            if (partIdx < row.size() - 1) {
                cellText = row.at(partIdx).trimmed();
            }
            auto *item = new QTableWidgetItem(cellText);
            ui->tableWidget->setItem(r, c, item);
        }
    }

    // Ensure _colAlignments matches the actual column count
    while (_colAlignments.size() < maxCols) {
        _colAlignments << Qt::AlignLeft;
    }
    if (_colAlignments.size() > maxCols) {
        _colAlignments.resize(maxCols);
    }

    updateColumnHeaderLabels();
    ui->tableWidget->resizeColumnsToContents();
}

/**
 * Updates the horizontal header labels to show column number and alignment indicator.
 * Format: "1 ←" (left), "1 ↔" (center), "1 →" (right), "1 ·" (default)
 */
void MarkdownTableDialog::updateColumnHeaderLabels() {
    QStringList labels;
    for (int c = 0; c < ui->tableWidget->columnCount(); ++c) {
        const Qt::AlignmentFlag align = _colAlignments.value(c, Qt::AlignLeft);
        QString indicator;
        if (align == Qt::AlignHCenter) {
            indicator = QStringLiteral(" ↔");
        } else if (align == Qt::AlignRight) {
            indicator = QStringLiteral(" →");
        } else if (align == Qt::AlignLeft) {
            indicator = QStringLiteral(" ←");
        } else {
            // Default (no explicit colon markers)
            indicator = QStringLiteral(" ·");
        }
        labels << QString::number(c + 1) + indicator;
    }
    ui->tableWidget->setHorizontalHeaderLabels(labels);
}

/**
 * Called when a column header section is clicked.
 * Selects the entire column and syncs the alignment combobox to the column's current alignment.
 */
void MarkdownTableDialog::onColumnHeaderClicked(int logicalIndex) {
    // Select the entire column so the user can see which column will be affected
    ui->tableWidget->selectColumn(logicalIndex);

    // Sync the alignment combobox to reflect the current alignment of this column
    const Qt::AlignmentFlag align =
        _colAlignments.value(logicalIndex, static_cast<Qt::AlignmentFlag>(0));
    int comboIndex = 0;    // Default
    if (align == Qt::AlignLeft) {
        comboIndex = 1;
    } else if (align == Qt::AlignHCenter) {
        comboIndex = 2;
    } else if (align == Qt::AlignRight) {
        comboIndex = 3;
    }
    ui->alignmentComboBox->setCurrentIndex(comboIndex);
}

/**
 * Returns the column index of the currently selected/active cell, or -1 if none
 */
int MarkdownTableDialog::currentColumn() const {
    const QList<QTableWidgetSelectionRange> ranges = ui->tableWidget->selectedRanges();
    if (!ranges.isEmpty()) {
        return ranges.constFirst().leftColumn();
    }
    const QTableWidgetItem *current = ui->tableWidget->currentItem();
    if (current != nullptr) {
        return current->column();
    }
    return -1;
}

/**
 * Builds the formatted Markdown table string from the current grid state
 */
QString MarkdownTableDialog::buildMarkdownTable() const {
    const int rows = ui->tableWidget->rowCount();
    const int cols = ui->tableWidget->columnCount();

    if (rows == 0 || cols == 0) {
        return {};
    }

    // Compute per-column max content width (minimum 3 for separator dashes)
    QVector<int> colWidths(cols, 3);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            const QTableWidgetItem *item = ui->tableWidget->item(r, c);
            const int len = item != nullptr ? item->text().length() : 0;
            colWidths[c] = std::max(colWidths[c], len);
        }
    }

    QString result;

    auto buildRow = [&](int r) {
        QString line = QStringLiteral("|");
        for (int c = 0; c < cols; ++c) {
            const QTableWidgetItem *item = ui->tableWidget->item(r, c);
            const QString text = item != nullptr ? item->text() : QString();
            const Qt::AlignmentFlag align = _colAlignments.value(c, Qt::AlignLeft);
            const int w = colWidths.at(c);
            QString cell;
            if (align == Qt::AlignHCenter) {
                const int pad = w - text.length();
                const int leftPad = pad / 2;
                const int rightPad = pad - leftPad;
                cell =
                    QString(leftPad, QLatin1Char(' ')) + text + QString(rightPad, QLatin1Char(' '));
            } else if (align == Qt::AlignRight) {
                cell = text.rightJustified(w);
            } else {
                cell = text.leftJustified(w);
            }
            line += QStringLiteral(" ") + cell + QStringLiteral(" |");
        }
        return line;
    };

    // Header row (always row 0 in the grid)
    result += buildRow(0) + QStringLiteral("\n");

    // Separator row based on alignments
    QString sep = QStringLiteral("|");
    for (int c = 0; c < cols; ++c) {
        const Qt::AlignmentFlag align = _colAlignments.value(c, Qt::AlignLeft);
        const int w = colWidths.at(c);
        QString dashes = QString(w, QLatin1Char('-'));
        if (align == Qt::AlignHCenter) {
            sep += QStringLiteral(" :") + dashes + QStringLiteral(": |");
        } else if (align == Qt::AlignRight) {
            sep += QStringLiteral(" ") + dashes + QStringLiteral(": |");
        } else if (align == Qt::AlignLeft) {
            sep += QStringLiteral(" :") + dashes + QStringLiteral("- |");
        } else {
            // Default: no colons
            sep += QStringLiteral(" ") + dashes + QStringLiteral(" |");
        }
    }
    result += sep + QStringLiteral("\n");

    // Data rows (rows 1+ in the grid)
    for (int r = 1; r < rows; ++r) {
        result += buildRow(r);
        if (r < rows - 1) {
            result += QStringLiteral("\n");
        }
    }

    return result;
}

/**
 * Replaces the original table in the text edit with the edited one
 */
void MarkdownTableDialog::on_buttonBox_accepted() {
    if (_textEdit == nullptr) {
        return;
    }

    const QString newTable = buildMarkdownTable();
    if (newTable.isEmpty()) {
        return;
    }

    QTextCursor cursor = _textEdit->textCursor();
    cursor.setPosition(_tableStartPosition);
    cursor.setPosition(_tableEndPosition, QTextCursor::KeepAnchor);
    cursor.insertText(newTable);
    _textEdit->setTextCursor(cursor);

    // Auto-format the table using the existing utility
    Utils::Gui::autoFormatTableAtCursor(_textEdit);
}

/**
 * Appends a new empty row at the bottom of the table
 */
void MarkdownTableDialog::on_addRowButton_clicked() {
    const int cols = ui->tableWidget->columnCount();
    const int newRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(newRow);
    for (int c = 0; c < cols; ++c) {
        ui->tableWidget->setItem(newRow, c, new QTableWidgetItem());
    }
}

/**
 * Removes the currently selected row (but not the header row at index 0)
 */
void MarkdownTableDialog::on_removeRowButton_clicked() {
    const int row = ui->tableWidget->currentRow();
    if (row < 0) {
        return;
    }
    if (row == 0) {
        QMessageBox::information(this, tr("Cannot remove header"),
                                 tr("The header row cannot be removed."));
        return;
    }
    if (ui->tableWidget->rowCount() <= 2) {
        QMessageBox::information(this, tr("Cannot remove row"),
                                 tr("The table must have at least one data row."));
        return;
    }
    ui->tableWidget->removeRow(row);
}

/**
 * Appends a new empty column to the right of the table
 */
void MarkdownTableDialog::on_addColumnButton_clicked() {
    const int newCol = ui->tableWidget->columnCount();
    ui->tableWidget->insertColumn(newCol);
    _colAlignments << Qt::AlignLeft;
    for (int r = 0; r < ui->tableWidget->rowCount(); ++r) {
        ui->tableWidget->setItem(r, newCol, new QTableWidgetItem());
    }
    updateColumnHeaderLabels();
}

/**
 * Removes the currently selected column
 */
void MarkdownTableDialog::on_removeColumnButton_clicked() {
    const int col = currentColumn();
    if (col < 0) {
        return;
    }
    if (ui->tableWidget->columnCount() <= 1) {
        QMessageBox::information(this, tr("Cannot remove column"),
                                 tr("The table must have at least one column."));
        return;
    }
    ui->tableWidget->removeColumn(col);
    if (col < _colAlignments.size()) {
        _colAlignments.remove(col);
    }
    updateColumnHeaderLabels();
}

/**
 * Applies the selected alignment to all currently selected columns
 */
void MarkdownTableDialog::on_applyAlignmentButton_clicked() {
    // Determine which columns are selected
    QSet<int> selectedCols;
    const QList<QTableWidgetSelectionRange> ranges = ui->tableWidget->selectedRanges();
    for (const QTableWidgetSelectionRange &range : ranges) {
        for (int c = range.leftColumn(); c <= range.rightColumn(); ++c) {
            selectedCols.insert(c);
        }
    }

    // Fall back to current item column if nothing is selected
    if (selectedCols.isEmpty()) {
        const int col = currentColumn();
        if (col >= 0) {
            selectedCols.insert(col);
        }
    }

    if (selectedCols.isEmpty()) {
        return;
    }

    Qt::AlignmentFlag align = Qt::AlignLeft;
    switch (ui->alignmentComboBox->currentIndex()) {
        case 1:
            align = Qt::AlignLeft;
            break;
        case 2:
            align = Qt::AlignHCenter;
            break;
        case 3:
            align = Qt::AlignRight;
            break;
        default:
            // Index 0 = "Default (left)" — use no colon markers (implicit left)
            align = static_cast<Qt::AlignmentFlag>(0);
            break;
    }

    for (int col : qAsConst(selectedCols)) {
        if (col < _colAlignments.size()) {
            _colAlignments[col] = align;
        }
    }

    updateColumnHeaderLabels();
}
