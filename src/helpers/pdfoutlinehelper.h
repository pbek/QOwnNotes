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

#include <QString>
#include <QTextDocument>
#include <QVector>

// Forward declarations
class QPrinter;

/**
 * @brief Structure to hold information about a heading in a document
 */
struct HeadingInfo {
    QString text;    // The heading text
    int level;       // Heading level (1-6 for h1-h6)
    int position;    // Character position in document
    int page;        // Page number where heading appears (will be calculated)

    HeadingInfo(const QString &t, int l, int pos) : text(t), level(l), position(pos), page(0) {}
};

/**
 * @brief Helper class for creating PDF outlines/bookmarks
 *
 * This class provides functionality to extract headings from a QTextDocument
 * and add them as bookmarks/outlines to a PDF file using the PoDoFo library.
 */
class PdfOutlineHelper {
   public:
    /**
     * @brief Extract headings from a QTextDocument
     * @param doc The text document to extract headings from
     * @return Vector of heading information structures
     */
    static QVector<HeadingInfo> extractHeadings(QTextDocument *doc);

    /**
     * @brief Add outline entries to a PDF file
     * @param pdfFilePath Path to the PDF file to modify
     * @param headings Vector of headings to add as outline entries
     * @return true if successful, false otherwise
     *
     * This function requires QOwnNotes to be compiled with PODOFO_ENABLED.
     * If PoDoFo support is not available, this function will return false.
     */
    static bool addOutlineToPdf(const QString &pdfFilePath, const QVector<HeadingInfo> &headings);

   private:
#ifdef PODOFO_ENABLED
    /**
     * @brief Calculate page numbers for headings based on document layout
     * @param doc The source document
     * @param headings Vector of headings to update with page numbers
     * @param printer The printer used for PDF generation (to get page metrics)
     */
    static void calculatePageNumbers(QTextDocument *doc, QVector<HeadingInfo> &headings,
                                     const QPrinter *printer);
#endif
};
