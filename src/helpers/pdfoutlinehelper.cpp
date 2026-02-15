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

#include "pdfoutlinehelper.h"

#include <QDebug>
#include <QFile>
#include <QPrinter>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextFormat>
#include <QThread>

#ifdef PODOFO_ENABLED
#include <podofo/podofo.h>
using namespace PoDoFo;
#endif

/**
 * @brief Extract headings from a QTextDocument
 *
 * This function scans through a QTextDocument and extracts all headings
 * (h1-h6) based on the block format. Markdown headings are typically
 * represented as QTextBlockFormat with specific heading levels.
 */
QVector<HeadingInfo> PdfOutlineHelper::extractHeadings(QTextDocument *doc) {
    QVector<HeadingInfo> headings;

    if (!doc) {
        return headings;
    }

    // Iterate through all blocks in the document
    QTextBlock block = doc->begin();
    while (block.isValid()) {
        QTextBlockFormat format = block.blockFormat();
        int headingLevel = format.headingLevel();

        // Check if this block is a heading (heading level 1-6)
        if (headingLevel > 0 && headingLevel <= 6) {
            QString text = block.text().trimmed();

            // Skip empty headings
            if (!text.isEmpty()) {
                int position = block.position();
                headings.append(HeadingInfo(text, headingLevel, position));

                qDebug() << "Found heading:" << text << "Level:" << headingLevel
                         << "Position:" << position;
            }
        }

        block = block.next();
    }

    return headings;
}

/**
 * @brief Add outline entries to a PDF file
 *
 * This function opens an existing PDF file and adds outline/bookmark entries
 * based on the provided heading information. It uses the PoDoFo library for
 * PDF manipulation.
 */
bool PdfOutlineHelper::addOutlineToPdf(const QString &pdfFilePath,
                                       const QVector<HeadingInfo> &headings) {
#ifdef PODOFO_ENABLED
    if (headings.isEmpty()) {
        qDebug() << "No headings to add to PDF outline";
        return true;    // Not an error, just nothing to do
    }

    try {
        // Ensure the file exists and is readable
        QFile file(pdfFilePath);
        if (!file.exists()) {
            qWarning() << "PDF file does not exist:" << pdfFilePath;
            return false;
        }

        // Check if file has content
        if (file.size() == 0) {
            qWarning() << "PDF file is empty:" << pdfFilePath;
            return false;
        }

        // Wait a moment to ensure file is fully written and closed
        // This is necessary because Qt might buffer writes
        QThread::msleep(200);

        // Try to open the file to ensure it's not locked
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Cannot open PDF file for reading:" << pdfFilePath;
            return false;
        }
        file.close();

        qDebug() << "Opening PDF file for adding outlines:" << pdfFilePath << "Size:" << file.size()
                 << "bytes";

        // Open the existing PDF document
        PdfMemDocument document;
        document.Load(pdfFilePath.toStdString());

        // Make a mutable copy of headings so we can update page numbers
        QVector<HeadingInfo> mutableHeadings = headings;

        // Get the pages collection
        auto &pages = document.GetPages();
        unsigned pageCount = pages.GetCount();

        qDebug() << "PDF has" << pageCount << "pages," << mutableHeadings.size() << "headings";

        // Assign page numbers to headings based on their character position
        // This is a heuristic that assumes fairly uniform text distribution
        if (pageCount > 0 && !mutableHeadings.isEmpty()) {
            // Get the last heading's position to estimate total document size
            int maxPosition = 0;
            for (const HeadingInfo &heading : mutableHeadings) {
                if (heading.position > maxPosition) {
                    maxPosition = heading.position;
                }
            }

            // Estimate characters per page
            double charsPerPage =
                maxPosition > 0 ? static_cast<double>(maxPosition) / pageCount : 1000;

            // Assign pages based on position
            for (HeadingInfo &heading : mutableHeadings) {
                int estimatedPage = static_cast<int>(heading.position / charsPerPage) + 1;
                heading.page = qMin(qMax(estimatedPage, 1), static_cast<int>(pageCount));
                qDebug() << "Heading" << heading.text << "at position" << heading.position
                         << "-> page" << heading.page;
            }
        }

        // Get or create the document outline
        PdfOutlines *outlines = document.GetOutlines();
        if (!outlines) {
            // Create outlines if they don't exist
            outlines = &document.GetOrCreateOutlines();
        }

        // Track parent outline items for each heading level
        // This allows us to create a proper tree structure
        QVector<PdfOutlineItem *> levelParents(7, nullptr);    // Levels 0-6

        // Add outline entries for each heading
        for (const HeadingInfo &heading : mutableHeadings) {
            // Determine the parent outline item based on heading level
            PdfOutlineItem *parent = nullptr;

            // Find the appropriate parent (closest higher-level heading)
            for (int i = heading.level - 1; i >= 0; --i) {
                if (levelParents[i] != nullptr) {
                    parent = levelParents[i];
                    break;
                }
            }

            // Get the destination page (pages are 0-indexed in PoDoFo)
            unsigned pageIndex = heading.page > 0 ? static_cast<unsigned>(heading.page - 1) : 0;

            // Ensure page index is valid
            if (pageIndex >= pageCount) {
                pageIndex = pageCount - 1;
            }

            // Get the page object
            PdfPage &page = pages.GetPageAt(pageIndex);

            // Create a destination to the page (top of page, fit width)
            auto dest = document.CreateDestination();
            dest->SetDestination(page, PdfDestinationFit::FitH, 0);

            // Create the outline item
            PdfOutlineItem *outlineItem;
            if (parent) {
                // Add as child of parent heading
                outlineItem = &parent->CreateChild(PdfString(heading.text.toStdString()));
            } else {
                // Add at root level
                outlineItem = &outlines->CreateRoot(PdfString(heading.text.toStdString()));
            }

            // Set destination to the page
            outlineItem->SetDestination(*dest);

            // Store this item as a potential parent for lower-level headings
            if (heading.level < levelParents.size()) {
                levelParents[heading.level] = outlineItem;

                // Clear any lower-level parents
                for (int i = heading.level + 1; i < levelParents.size(); ++i) {
                    levelParents[i] = nullptr;
                }
            }

            qDebug() << "Added outline entry:" << heading.text << "Level:" << heading.level
                     << "Page:" << heading.page;
        }

        // Save the modified PDF (full rewrite, not incremental update)
        // SaveUpdate() only writes the changes, which loses the original content
        QString tempPath = pdfFilePath + ".tmp";
        try {
            document.Save(tempPath.toStdString());
            qDebug() << "Saved modified PDF to temporary file:" << tempPath;

            // Verify the temp file has content
            QFile tempFile(tempPath);
            if (!tempFile.exists() || tempFile.size() == 0) {
                qWarning() << "Temporary PDF file is invalid or empty";
                QFile::remove(tempPath);
                return false;
            }

            qDebug() << "Temporary PDF size:" << tempFile.size() << "bytes";

            // Replace the original file with the modified one
            QFile::remove(pdfFilePath);
            if (QFile::rename(tempPath, pdfFilePath)) {
                qDebug() << "Successfully replaced original PDF with modified version";
            } else {
                qWarning() << "Failed to rename temporary file to original path";
                // Try to copy instead
                if (QFile::copy(tempPath, pdfFilePath)) {
                    QFile::remove(tempPath);
                    qDebug() << "Successfully copied modified PDF to original location";
                } else {
                    qWarning() << "Failed to copy temporary file. Modified PDF is at:" << tempPath;
                    return false;
                }
            }
        } catch (const PdfError &error) {
            qWarning() << "PoDoFo error while saving modified PDF:"
                       << QString::fromStdString(error.what());
            QFile::remove(tempPath);    // Clean up temp file
            throw;                      // Re-throw to be caught by outer catch
        }

        qDebug() << "Successfully added" << mutableHeadings.size()
                 << "outline entries to PDF:" << pdfFilePath;
        return true;
    } catch (const PdfError &error) {
        qWarning() << "PoDoFo error while adding PDF outline:"
                   << QString::fromStdString(error.what());
        return false;
    } catch (const std::exception &e) {
        qWarning() << "Error while adding PDF outline:" << e.what();
        return false;
    } catch (...) {
        qWarning() << "Unknown error while adding PDF outline";
        return false;
    }
#else
    Q_UNUSED(pdfFilePath)
    Q_UNUSED(headings)
    qDebug() << "PoDoFo support not enabled. Cannot add PDF outline.";
    return false;
#endif
}

#ifdef PODOFO_ENABLED
/**
 * @brief Calculate page numbers for headings
 *
 * This function estimates which page each heading appears on by analyzing
 * the document layout. This is a simplified approach that works reasonably
 * well for most documents.
 */
void PdfOutlineHelper::calculatePageNumbers(QTextDocument *doc, QVector<HeadingInfo> &headings,
                                            const QPrinter *printer) {
    if (!doc || !printer || headings.isEmpty()) {
        return;
    }

    // Clone the document to avoid modifying the original
    QTextDocument cloneDoc;
    cloneDoc.setHtml(doc->toHtml());
    cloneDoc.setPageSize(printer->pageRect(QPrinter::Point).size());

    // Calculate page breaks
    qreal pageHeight = printer->pageRect(QPrinter::Point).height();
    qreal currentHeight = 0.0;
    int currentPage = 1;
    int headingIndex = 0;

    QTextBlock block = cloneDoc.begin();
    while (block.isValid() && headingIndex < headings.size()) {
        // Check if we've reached the next heading
        if (block.position() >= headings[headingIndex].position) {
            headings[headingIndex].page = currentPage;
            headingIndex++;
        }

        // Add block height to current page height
        QTextLayout *layout = block.layout();
        if (layout) {
            currentHeight += layout->boundingRect().height();

            // Check if we need to start a new page
            if (currentHeight >= pageHeight) {
                currentPage++;
                currentHeight = 0.0;
            }
        }

        block = block.next();
    }

    // Assign remaining headings to the last page
    while (headingIndex < headings.size()) {
        headings[headingIndex].page = currentPage;
        headingIndex++;
    }
}
#endif
