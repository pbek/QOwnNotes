#pragma once

#include <QPlainTextEdit>

#include "masterdialog.h"

namespace Ui {
class MarkdownTableDialog;
}

/**
 * Dialog for editing a Markdown table visually in a grid.
 * Supports adding/removing rows and columns, changing column alignment
 * (left, center, right) and automatically formats the table on accept.
 */
class MarkdownTableDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit MarkdownTableDialog(QPlainTextEdit *textEdit, QWidget *parent = nullptr);
    ~MarkdownTableDialog() override;

   private slots:
    void on_buttonBox_accepted();
    void on_addRowButton_clicked();
    void on_removeRowButton_clicked();
    void on_addColumnButton_clicked();
    void on_removeColumnButton_clicked();
    void on_applyAlignmentButton_clicked();
    void onColumnHeaderClicked(int logicalIndex);

   private:
    Ui::MarkdownTableDialog *ui;
    QPlainTextEdit *_textEdit;

    // Position of the table in the document so we can replace it on accept
    int _tableStartPosition = 0;
    int _tableEndPosition = 0;

    // Per-column alignment: Qt::AlignLeft / Qt::AlignHCenter / Qt::AlignRight
    // Index 0 corresponds to the first data column
    QVector<Qt::AlignmentFlag> _colAlignments;

    void loadTableFromCursor();
    void populateGridFromParsed(const QList<QStringList> &rows);
    QString buildMarkdownTable() const;
    int currentColumn() const;
    void updateColumnHeaderLabels();
};
