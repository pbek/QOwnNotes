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

#include "exportprintmanager.h"

#include <dialogs/filedialog.h>
#include <entities/note.h>
#include <entities/notefolder.h>
#include <services/settingsservice.h>
#include <utils/misc.h>

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QInputDialog>
#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QTextStream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widgets/qownnotesmarkdowntextedit.h"

ExportPrintManager::ExportPrintManager(MainWindow *mainWindow, Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui) {}

bool ExportPrintManager::preparePrintNotePrinter(QPrinter *printer) {
    Utils::Misc::loadPrinterSettings(printer, QStringLiteral("Printer/NotePrinting"));

    QPrintDialog dialog(printer, _mainWindow);
    dialog.setWindowTitle(tr("Print note"));
    const int ret = dialog.exec();

    if (ret != QDialog::Accepted) {
        return false;
    }

    Utils::Misc::storePrinterSettings(printer, QStringLiteral("Printer/NotePrinting"));
    return true;
}

/**
 * @brief Prints the content of a text document
 * @param textEdit
 */
void ExportPrintManager::printTextDocument(QTextDocument *textDocument) {
    QPrinter printer;
    if (preparePrintNotePrinter(&printer)) {
        textDocument->print(&printer);
    }
}

/**
 * @brief Prepares the printer dialog to exports the content of a text edit
 *        widget as PDF
 * @param printer
 */
bool ExportPrintManager::prepareExportNoteAsPDFPrinter(QPrinter *printer) {
#ifdef Q_OS_LINUX
    Utils::Misc::loadPrinterSettings(printer, QStringLiteral("Printer/NotePDFExport"));

    // Ensure color mode is always set to Color for PDF export
    printer->setColorMode(QPrinter::Color);

    // under Linux we use the QPageSetupDialog to change layout
    // settings of the PDF export
    QPageSetupDialog pageSetupDialog(printer, _mainWindow);

    if (pageSetupDialog.exec() != QDialog::Accepted) {
        return false;
    }

    // Ensure color mode is still set to Color before storing settings
    printer->setColorMode(QPrinter::Color);

    Utils::Misc::storePrinterSettings(printer, QStringLiteral("Printer/NotePDFExport"));
#else
    // under OS X and Windows the QPageSetupDialog dialog doesn't work,
    // we will use a workaround to select page sizes and the orientation

    // Ensure color mode is always set to Color for PDF export
    printer->setColorMode(QPrinter::Color);

    SettingsService settings;

    // select the page size
    QStringList pageSizeStrings;
    pageSizeStrings << QStringLiteral("A0") << QStringLiteral("A1") << QStringLiteral("A2")
                    << QStringLiteral("A3") << QStringLiteral("A4") << QStringLiteral("A5")
                    << QStringLiteral("A6") << QStringLiteral("A7") << QStringLiteral("A8")
                    << QStringLiteral("A9") << tr("Letter");
    QList<QPageSize::PageSizeId> pageSizes;
    pageSizes << QPageSize::A0 << QPageSize::A1 << QPageSize::A2 << QPageSize::A3 << QPageSize::A4
              << QPageSize::A5 << QPageSize::A6 << QPageSize::A7 << QPageSize::A8 << QPageSize::A9
              << QPageSize::Letter;

    bool ok;
    QString pageSizeString = QInputDialog::getItem(
        _mainWindow, tr("Page size"), tr("Page size:"), pageSizeStrings,
        settings.value(QStringLiteral("Printer/NotePDFExportPageSize"), 4).toInt(), false, &ok);

    if (!ok || pageSizeString.isEmpty()) {
        return false;
    }

    int pageSizeIndex = pageSizeStrings.indexOf(pageSizeString);
    if (pageSizeIndex == -1) {
        return false;
    }

    QPageSize pageSize(pageSizes.at(pageSizeIndex));
    settings.setValue(QStringLiteral("Printer/NotePDFExportPageSize"), pageSizeIndex);
    printer->setPageSize(pageSize);

    // select the orientation
    QStringList orientationStrings;
    orientationStrings << tr("Portrait") << tr("Landscape");
    QList<QPageLayout::Orientation> orientations;
    orientations << QPageLayout::Portrait << QPageLayout::Landscape;

    QString orientationString = QInputDialog::getItem(
        _mainWindow, tr("Orientation"), tr("Orientation:"), orientationStrings,
        settings.value(QStringLiteral("Printer/NotePDFExportOrientation"), 0).toInt(), false, &ok);

    if (!ok || orientationString.isEmpty()) {
        return false;
    }

    int orientationIndex = orientationStrings.indexOf(orientationString);
    if (orientationIndex == -1) {
        return false;
    }

    printer->setPageOrientation(orientations.at(orientationIndex));
    settings.setValue(QStringLiteral("Printer/NotePDFExportOrientation"), orientationIndex);
#endif

    FileDialog dialog(QStringLiteral("NotePDFExport"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("PDF files") + QStringLiteral(" (*.pdf)"));
    dialog.setWindowTitle(tr("Export current note as PDF"));
    dialog.selectFile(_mainWindow->getCurrentNote().getName() + QStringLiteral(".pdf"));
    int ret = dialog.exec();

    if (ret != QDialog::Accepted) {
        return false;
    }

    QString fileName = dialog.selectedFile();

    if (fileName.isEmpty()) {
        return false;
    }

    if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName.append(QLatin1String(".pdf"));
    }

    // Ensure color mode is set to Color right before setting output format
    printer->setColorMode(QPrinter::Color);
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName(fileName);
    return true;
}

/**
 * @brief Exports the content of a plain text edit widget as PDF
 * @param textEdit
 */
void ExportPrintManager::exportNoteAsPDF(QPlainTextEdit *textEdit) {
    exportNoteAsPDF(textEdit->document());
}

/**
 * @brief Exports the document as PDF
 * @param doc
 */
void ExportPrintManager::exportNoteAsPDF(QTextDocument *doc) {
    auto *printer = new QPrinter(QPrinter::HighResolution);
    printer->setColorMode(QPrinter::Color);

    if (prepareExportNoteAsPDFPrinter(printer)) {
        doc->print(printer);
        Utils::Misc::openFolderSelect(printer->outputFileName());
    }

    delete printer;
}

QTextDocument *ExportPrintManager::getDocumentForPreviewExport() {
    bool decrypt = _ui->noteTextEdit->isHidden();
    QString html = _mainWindow->currentNote.toMarkdownHtml(
        NoteFolder::currentLocalPath(), _mainWindow->getMaxImageWidth(),
        Utils::Misc::useInternalExportStylingForPreview(), decrypt);
    html = Utils::Misc::parseTaskList(html, false);

    // Windows 10 has troubles with the QTextDocument from the QTextBrowser
    // see: https://github.com/pbek/QOwnNotes/issues/2015
    //    auto doc = ui->noteTextView->document()->clone();
    auto doc = new QTextDocument(_mainWindow);
    doc->setHtml(html);

    return doc;
}

void ExportPrintManager::on_action_Export_note_as_PDF_markdown_triggered() {
    auto doc = getDocumentForPreviewExport();
    exportNoteAsPDF(doc);
    doc->deleteLater();
}

/**
 * @brief Exports the current note as PDF (text)
 */
void ExportPrintManager::on_action_Export_note_as_PDF_text_triggered() {
    QOwnNotesMarkdownTextEdit *textEdit = _mainWindow->activeNoteTextEdit();
    exportNoteAsPDF(textEdit);
}

/**
 * @brief Prints the current note (Markdown)
 */
void ExportPrintManager::on_action_Print_note_markdown_triggered() {
    auto doc = getDocumentForPreviewExport();
    printTextDocument(doc);
    doc->deleteLater();
}

/**
 * @brief Prints the current note (text)
 */
void ExportPrintManager::on_action_Print_note_text_triggered() {
    printTextDocument(_mainWindow->activeNoteTextEdit()->document());
}

/**
 * Exports the note preview as HTML
 */
void ExportPrintManager::on_actionExport_preview_HTML_triggered() {
    FileDialog dialog(QStringLiteral("NoteHTMLExport"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("HTML files") + " (*.html)");
    dialog.setWindowTitle(tr("Export current note as HTML file"));
    dialog.selectFile(_mainWindow->getCurrentNote().getName() + QStringLiteral(".html"));
    const int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            if (QFileInfo(fileName).suffix().isEmpty()) {
                fileName.append(QStringLiteral(".html"));
            }

            QFile file(fileName);

            qDebug() << "exporting html file: " << fileName;

            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qCritical() << file.errorString();
                return;
            }
            QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            out.setCodec("UTF-8");
#endif
            out << _mainWindow->currentNote.toMarkdownHtml(
                NoteFolder::currentLocalPath(), _mainWindow->getMaxImageWidth(), true, true, true);
            file.flush();
            file.close();
            Utils::Misc::openFolderSelect(fileName);
        }
    }
}
