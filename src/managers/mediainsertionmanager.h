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
class QFile;
class QMimeData;

namespace Ui {
class MainWindow;
}

class MediaInsertionManager : public QObject {
    Q_OBJECT

   public:
    explicit MediaInsertionManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                                   QObject *parent = nullptr);

    void handleInsertingFromMimeData(const QMimeData *mimeData);
    bool insertMedia(QFile *file, QString title = QString());
    bool insertAttachment(QFile *file, const QString &title = QString(),
                          const QString &fileName = QString());
    void insertNoteText(const QString &text);
    bool insertTextAsAttachment(const QString &text);
    void insertHtmlAsMarkdownIntoCurrentNote(QString html);
    void pasteMediaIntoNote();
    bool insertDataUrlAsFileIntoCurrentNote(const QString &dataUrl);
    static bool isValidMediaFile(QFile *file);
    static bool isValidNoteFile(QFile *file);

   public slots:
    void on_actionInsert_image_triggered();
    void on_actionPaste_image_triggered();
    void on_actionInsert_attachment_triggered();

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
};
