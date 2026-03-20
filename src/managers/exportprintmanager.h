/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#pragma once

#include <QObject>

class MainWindow;
class QPrinter;
class QTextDocument;
class QPlainTextEdit;

namespace Ui {
class MainWindow;
}

class ExportPrintManager : public QObject {
    Q_OBJECT

   public:
    explicit ExportPrintManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                                QObject *parent = nullptr);

    bool preparePrintNotePrinter(QPrinter *printer);
    void printTextDocument(QTextDocument *textDocument);
    bool prepareExportNoteAsPDFPrinter(QPrinter *printer);
    void exportNoteAsPDF(QPlainTextEdit *textEdit);
    void exportNoteAsPDF(QTextDocument *doc);
    QTextDocument *getDocumentForPreviewExport();

   public slots:
    void on_action_Export_note_as_PDF_markdown_triggered();
    void on_action_Export_note_as_PDF_text_triggered();
    void on_action_Print_note_markdown_triggered();
    void on_action_Print_note_text_triggered();
    void on_actionExport_preview_HTML_triggered();

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
};
