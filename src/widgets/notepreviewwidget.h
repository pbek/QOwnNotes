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

#pragma once

#include <widgets/qtexteditsearchwidget.h>

#include <QPoint>
#include <QResizeEvent>
#include <QTextBrowser>

class NotePreviewWidget : public QTextBrowser {
    Q_OBJECT

   public:
    explicit NotePreviewWidget(QWidget *parent = nullptr);
    void initSearchFrame(QWidget *searchFrame, bool darkMode = false);
    QTextEditSearchWidget *searchWidget();

    void setHtml(const QString &text);

    void exportAsHTMLFile();

   protected:
    QTextEditSearchWidget *_searchWidget;
    QWidget *_searchFrame;
    QString _html;

    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

    QStringList extractGifUrls(const QString &text) const;
    void animateGif(const QString &text);

    void contextMenuEvent(QContextMenuEvent *event) override;

    QVariant loadResource(int type, const QUrl &file) override;

    bool lookupCache(const QString &key, QPixmap &pm);
    void insertInCache(const QString &key, const QPixmap &pm);

   public slots:
    void hide();

   signals:
    void resize(QSize size, QSize oldSize);

   private:
    QList<QMovie *> _movies;
    QString _currentFile;

    struct LargePixmap {
        QString fileName;
        QPixmap pixmap;
    };
    std::vector<LargePixmap> _largePixmapCache;
};
