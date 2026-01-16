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
 */

#include "notepreviewwidget.h"

#include <dialogs/filedialog.h>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QLayout>
#include <QMenu>
#include <QMovie>
#include <QProxyStyle>
#include <QRegularExpression>
#include <QTextTable>
#include <QTextTableFormat>

#include "utils/misc.h"

class NoDottedOutlineForLinksStyle : public QProxyStyle {
   public:
    int styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget,
                  QStyleHintReturn *returnData) const Q_DECL_OVERRIDE {
        if (hint == SH_TextControl_FocusIndicatorTextCharFormat) return 0;
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

NotePreviewWidget::NotePreviewWidget(QWidget *parent) : QTextBrowser(parent) {
    // add the hidden search widget
    _searchWidget = new QTextEditSearchWidget(this);
    _searchWidget->setReplaceEnabled(false);

    // add a layout to the widget
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch();
    this->setLayout(layout);
    this->layout()->addWidget(_searchWidget);

    installEventFilter(this);
    viewport()->installEventFilter(this);

    auto proxyStyle = new NoDottedOutlineForLinksStyle;
    proxyStyle->setParent(this);
    setStyle(proxyStyle);
}

void NotePreviewWidget::resizeEvent(QResizeEvent *event) {
    emit resize(event->size(), event->oldSize());

    // we need this, otherwise the preview is always blank
    QTextBrowser::resizeEvent(event);
}

bool NotePreviewWidget::eventFilter(QObject *obj, QEvent *event) {
    //    qDebug() << event->type();
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        // disallow keys if widget hasn't focus
        if (!this->hasFocus()) {
            return true;
        }

        if ((keyEvent->key() == Qt::Key_Escape) && _searchWidget->isVisible()) {
            _searchWidget->deactivate();
            return true;
        } else if ((keyEvent->key() == Qt::Key_F) &&
                   keyEvent->modifiers().testFlag(Qt::ControlModifier)) {
            _searchWidget->activate();
            return true;
        } else if ((keyEvent->key() == Qt::Key_F3)) {
            _searchWidget->doSearch(!keyEvent->modifiers().testFlag(Qt::ShiftModifier));
            return true;
        }

        return false;
    }

    return QTextBrowser::eventFilter(obj, event);
}

/**
 * @brief Extract local gif urls from html
 * @param text
 * @return Urls to gif files
 */
QStringList NotePreviewWidget::extractGifUrls(const QString &text) const {
    QSet<QString> urlSet;
    static const QRegularExpression regex(R"(<img[^>]+src=\"(file:\/\/\/[^\"]+\.gif)\")",
                                          QRegularExpression::CaseInsensitiveOption);
    int pos = 0;

    while (true) {
        QRegularExpressionMatch match;
        pos = text.indexOf(regex, pos, &match);
        if (pos == -1 || !match.hasMatch()) break;
        QString url = match.captured(1);
        urlSet.insert(url);
        pos += match.capturedLength();
    }

#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    return urlSet.toList();
#else
    return QStringList(urlSet.begin(), urlSet.end());
#endif
}

/**
 * @brief Setup animations for gif
 * @return
 */
void NotePreviewWidget::animateGif(const QString &text) {
    // clear resources
    if (QTextDocument *doc = document()) doc->clear();

    QStringList urls = extractGifUrls(text);

    for (QMovie *&movie : _movies) {
        QString url = movie->property("URL").toString();
        if (urls.contains(url))
            urls.removeAll(url);
        else {
            movie->deleteLater();
            movie = nullptr;
        }
    }
    _movies.removeAll(nullptr);

    for (const QString &url : Utils::asConst(urls)) {
        auto *movie = new QMovie(this);
        movie->setFileName(QUrl(url).toLocalFile());
        movie->setCacheMode(QMovie::CacheNone);

        if (!movie->isValid() || movie->frameCount() < 2) {
            movie->deleteLater();
            continue;
        }

        movie->setProperty("URL", url);
        _movies.append(movie);

        connect(movie, &QMovie::frameChanged, this, [this, url, movie](int) {
            if (auto doc = document()) {
                doc->addResource(QTextDocument::ImageResource, url, movie->currentPixmap());
                doc->markContentsDirty(0, doc->characterCount());
            }
        });

        movie->start();
    }
}

void NotePreviewWidget::setHtml(const QString &text) {
    animateGif(text);
    _html = Utils::Misc::parseTaskList(text, true);

    QTextBrowser::setHtml(_html);

    // Optionally apply table formatting programmatically
    // applyTableFormats();
}

/**
 * @brief Programmatically applies QTextTableFormat to all tables in the document
 * This is an alternative approach to CSS styling for tables
 */
void NotePreviewWidget::applyTableFormats() {
    QTextDocument *doc = document();
    if (!doc) return;

    QTextCursor cursor(doc);

    // Iterate through all blocks in the document to find tables
    while (!cursor.atEnd()) {
        QTextTable *table = cursor.currentTable();
        if (table) {
            QTextTableFormat format = table->format();

            // Set border collapse to false (equivalent to CSS border-collapse: separate)
            format.setBorderCollapse(false);

            // Set border style
            format.setBorder(1.0);
            format.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);

            // Set cell spacing (equivalent to CSS border-spacing)
            format.setCellSpacing(2.0);

            // Set cell padding
            format.setCellPadding(5.0);

            // Apply the format
            table->setFormat(format);

            // Move cursor past the table
            cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 1);
            cursor = table->lastCursorPosition();
        }

        cursor.movePosition(QTextCursor::NextBlock);
    }
}

/**
 * @brief Returns the searchWidget instance
 * @return
 */
QTextEditSearchWidget *NotePreviewWidget::searchWidget() { return _searchWidget; }

/**
 * Uses another widget as parent for the search widget
 */
void NotePreviewWidget::initSearchFrame(QWidget *searchFrame, bool darkMode) {
    _searchFrame = searchFrame;

    // remove the search widget from our layout
    layout()->removeWidget(_searchWidget);

    QLayout *layout = _searchFrame->layout();

    // create a grid layout for the frame and add the search widget to it
    if (layout == nullptr) {
        layout = new QVBoxLayout(_searchFrame);
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);
    }

    _searchWidget->setDarkMode(darkMode);
    _searchWidget->setReplaceEnabled(false);
    layout->addWidget(_searchWidget);
    _searchFrame->setLayout(layout);
}

/**
 * Hides the preview and the search widget
 */
void NotePreviewWidget::hide() {
    _searchWidget->hide();
    QWidget::hide();
}

/**
 * Shows a context menu for the note preview
 *
 * @param event
 */
void NotePreviewWidget::contextMenuEvent(QContextMenuEvent *event) {
    QPoint pos = event->pos();
    QPoint globalPos = event->globalPos();
    QMenu *menu = this->createStandardContextMenu();

    QTextCursor c = this->cursorForPosition(pos);
    QTextFormat format = c.charFormat();
    const QString &anchorHref = format.toCharFormat().anchorHref();
    bool isImageFormat = format.isImageFormat();
    bool isAnchor = !anchorHref.isEmpty();

    if (isImageFormat || isAnchor) {
        menu->addSeparator();
    }

    QAction *copyImageAction = nullptr;
    QAction *copyLinkLocationAction = nullptr;

    // check if clicked object was an image
    if (isImageFormat) {
        copyImageAction = menu->addAction(tr("Copy image file path"));
        auto *copyImageClipboardAction = menu->addAction(tr("Copy image to clipboard"));

        connect(copyImageClipboardAction, &QAction::triggered, this, [format]() {
            QString imagePath = format.toImageFormat().name();
            QUrl imageUrl = QUrl(imagePath);
            QClipboard *clipboard = QApplication::clipboard();
            if (imageUrl.isLocalFile()) {
                clipboard->setImage(QImage(imageUrl.toLocalFile()));
            } else if (imagePath.startsWith(QLatin1String("data:image/"), Qt::CaseInsensitive)) {
                QStringList parts = imagePath.split(QStringLiteral(";base64,"));
                if (parts.count() == 2) {
                    clipboard->setImage(
                        QImage::fromData(QByteArray::fromBase64(parts[1].toLatin1())));
                }
            }
        });
    }

    if (isAnchor) {
        copyLinkLocationAction = menu->addAction(tr("Copy link location"));
    }

    auto *htmlFileExportAction = menu->addAction(tr("Export generated raw HTML"));

    QAction *selectedItem = menu->exec(globalPos);

    if (selectedItem) {
        // copy the image file path to the clipboard
        if (selectedItem == copyImageAction) {
            QString imagePath = format.toImageFormat().name();
            QUrl imageUrl = QUrl(imagePath);

            if (imageUrl.isLocalFile()) {
                imagePath = imageUrl.toLocalFile();
            }

            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(imagePath);
        }
        // copy link location to the clipboard
        else if (selectedItem == copyLinkLocationAction) {
            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(anchorHref);
        }
        // export the generated html as html file
        else if (selectedItem == htmlFileExportAction) {
            exportAsHTMLFile();
        }
    }
}

QVariant NotePreviewWidget::loadResource(int type, const QUrl &file) {
    if (type == QTextDocument::ImageResource && file.isValid()) {
        QString fileName = file.toLocalFile();
        int fileSize = QFileInfo(fileName).size();

        // We only use our cache for images > 512KB
        if (fileSize > (512 * 1000)) {
            QPixmap pm;
            if (lookupCache(fileName, pm)) {
                return pm;
            }

            pm = QPixmap(fileName);
            insertInCache(fileName, pm);
            return pm;
        }
    }

    return QTextBrowser::loadResource(type, file);
}

bool NotePreviewWidget::lookupCache(const QString &key, QPixmap &pm) {
    auto it = std::find_if(_largePixmapCache.begin(), _largePixmapCache.end(),
                           [key](const LargePixmap &l) { return key == l.fileName; });
    if (it == _largePixmapCache.end()) return false;
    pm = it->pixmap;
    return true;
}

void NotePreviewWidget::insertInCache(const QString &key, const QPixmap &pm) {
    _largePixmapCache.push_back({key, std::move(pm)});

    // limit to 6 images
    while (_largePixmapCache.size() > 6) _largePixmapCache.erase(_largePixmapCache.begin());
}

void NotePreviewWidget::exportAsHTMLFile() {
    FileDialog dialog(QStringLiteral("PreviewHTMLFileExport"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("HTML files") + " (*.html)");
    dialog.setWindowTitle(
        tr("Export preview as raw HTML file",
           "\"Raw\" means that actually the html that was fed to the preview will be stored (the "
           "QTextBrowser modifies the html that it is showing)"));
    dialog.selectFile(QStringLiteral("preview.html"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            if (QFileInfo(fileName).suffix().isEmpty()) {
                fileName.append(".html");
            }

            QFile file(fileName);

            qDebug() << "exporting raw preview html file: " << fileName;

            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qCritical() << file.errorString();
                return;
            }
            QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            out.setCodec("UTF-8");
#endif
            out << _html;
            file.flush();
            file.close();
            Utils::Misc::openFolderSelect(fileName);
        }
    }
}
