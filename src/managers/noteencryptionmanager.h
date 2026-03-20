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

namespace Ui {
class MainWindow;
}

class NoteEncryptionManager : public QObject {
    Q_OBJECT

   public:
    explicit NoteEncryptionManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                                   QObject *parent = nullptr);

    void askForEncryptedNotePasswordIfNeeded(const QString &additionalText = QString());
    void updateNoteEncryptionUI();
    void editEncryptedNote();
    void editEncryptedNoteAsync();

   public slots:
    void on_action_Encrypt_note_triggered();
    void on_actionDecrypt_note_triggered();
    void on_actionEdit_encrypted_note_triggered();
    void on_encryptedNoteTextEdit_textChanged();
    void on_encryptedNoteTextEdit_modificationChanged(bool arg1);
    void on_action_Export_note_as_markdown_triggered();

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
};
