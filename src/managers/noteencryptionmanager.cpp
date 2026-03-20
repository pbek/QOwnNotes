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

#include "noteencryptionmanager.h"

#include <dialogs/filedialog.h>
#include <dialogs/passworddialog.h>
#include <services/settingsservice.h>
#include <utils/gui.h>

#include <QApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"

NoteEncryptionManager::NoteEncryptionManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                                             QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui) {}

/**
 * Asks for the password if the note is encrypted and can't be decrypted
 */
void NoteEncryptionManager::askForEncryptedNotePasswordIfNeeded(const QString &additionalText) {
    Note &currentNote = _mainWindow->currentNote;
    currentNote.refetch();

    // check if the note is encrypted and can't be decrypted
    if (currentNote.hasEncryptedNoteText() && !currentNote.canDecryptNoteText()) {
        QString labelText =
            tr("Please enter the <strong>password</strong> "
               "of this encrypted note.");

        if (!additionalText.isEmpty()) {
            labelText += QStringLiteral(" ") + additionalText;
        }

        auto *dialog = new PasswordDialog(_mainWindow, labelText);
        const int dialogResult = dialog->exec();

        // if user pressed ok take the password
        if (dialogResult == QDialog::Accepted) {
            const QString password = dialog->password();
            if (!password.isEmpty()) {
                // set the password so it can be decrypted
                // for the Markdown view
                currentNote.setCryptoPassword(password);
                currentNote.store();
            }

            // warn if password is incorrect
            if (!currentNote.canDecryptNoteText()) {
                QMessageBox::warning(_mainWindow, tr("Note can't be decrypted!"),
                                     tr("It seems that your password is not valid!"));
            }
        }

        delete (dialog);
    }
}

/**
 * Asks the user for a password and encrypts the note text with it
 */
void NoteEncryptionManager::on_action_Encrypt_note_triggered() {
    Note &currentNote = _mainWindow->currentNote;
    currentNote.refetch();

    // return if the note text is already encrypted
    if (currentNote.hasEncryptedNoteText()) {
        return;
    }

    // the password dialog can be disabled by scripts
    const bool dialogDisabled = qApp->property("encryptionPasswordDisabled").toBool();

    if (!dialogDisabled) {
        const QString labelText =
            tr("Please enter your <strong>password</strong> to encrypt the note."
               "<br />Keep in mind that you have to <strong>remember</strong> "
               "your password to read the content of the note<br /> and that you "
               "can <strong>only</strong> do that <strong>in QOwnNotes</strong>!");
        auto *dialog = new PasswordDialog(_mainWindow, labelText, true);
        const int dialogResult = dialog->exec();

        // if the user didn't press ok return
        if (dialogResult != QDialog::Accepted) {
            return;
        }

        // take the password
        const QString password = dialog->password();

        // if password was empty return
        if (password.isEmpty()) {
            return;
        }

        // set the password
        currentNote.setCryptoPassword(password);
        currentNote.store();

        delete (dialog);
    }

    // encrypt the note
    const QString noteText = currentNote.encryptNoteText();
    _ui->noteTextEdit->setPlainText(noteText);
    _mainWindow->updateNoteTextEditReadOnly();
}

/**
 * Enables or disables the encrypt note buttons
 */
void NoteEncryptionManager::updateNoteEncryptionUI() {
    Note &currentNote = _mainWindow->currentNote;
    currentNote.refetch();
    const bool hasEncryptedNoteText = currentNote.hasEncryptedNoteText();

    _ui->action_Encrypt_note->setEnabled(!hasEncryptedNoteText);
    _ui->actionEdit_encrypted_note->setEnabled(hasEncryptedNoteText);
    _ui->actionDecrypt_note->setEnabled(hasEncryptedNoteText);

    // disable spell checker for encrypted text
    const bool checkSpellingEnabled =
        SettingsService().value(QStringLiteral("checkSpelling"), true).toBool();
    const bool spellCheckerShouldBeActive = !hasEncryptedNoteText && checkSpellingEnabled;

    // check if the spellchecking state is not as it should be
    if (spellCheckerShouldBeActive != _ui->noteTextEdit->isSpellCheckingEnabled()) {
        _ui->noteTextEdit->setSpellCheckingEnabled(spellCheckerShouldBeActive);
        _ui->noteTextEdit->highlighter()->rehighlight();

        // for some reason the encryptedNoteTextEdit is also affected and needs
        // to be set again
        if (hasEncryptedNoteText) {
            _ui->encryptedNoteTextEdit->setSpellCheckingEnabled(checkSpellingEnabled);
        }
    }
}

/**
 * Attempt to decrypt note text
 */
void NoteEncryptionManager::on_actionDecrypt_note_triggered() {
    Note &currentNote = _mainWindow->currentNote;
    currentNote.refetch();
    if (!currentNote.hasEncryptedNoteText()) {
        return;
    }

    QMessageBox msgBox(QMessageBox::Warning, tr("Decrypt note and store it as plain text"),
                       tr("Your note will be decrypted and stored as plain text again. "
                          "Keep in mind that the unencrypted note will possibly be "
                          "synced to your server and sensitive text may be exposed!"
                          "<br />Do you want to decrypt your note?"),
                       QMessageBox::NoButton, _mainWindow);
    msgBox.addButton(tr("&Decrypt"), QMessageBox::AcceptRole);
    QPushButton *cancelButton = msgBox.addButton(tr("&Cancel"), QMessageBox::RejectRole);
    msgBox.setDefaultButton(cancelButton);
    msgBox.exec();

    if (msgBox.clickedButton() == cancelButton) {
        return;
    }

    askForEncryptedNotePasswordIfNeeded();

    if (currentNote.canDecryptNoteText()) {
        _ui->encryptedNoteTextEdit->hide();
        _ui->noteTextEdit->setText(currentNote.fetchDecryptedNoteText());
        _ui->noteTextEdit->show();
        _ui->noteTextEdit->setFocus();
        _mainWindow->updateNoteTextEditReadOnly();
        _ui->noteTextEdit->setMarkdownLspDocumentPath(currentNote.fullNoteFilePath(),
                                                      _ui->noteTextEdit->toPlainText());
    }
}

/**
 * Lets the user edit an encrypted note text in a 2nd text edit
 */
void NoteEncryptionManager::on_actionEdit_encrypted_note_triggered() { editEncryptedNote(); }

void NoteEncryptionManager::editEncryptedNoteAsync() {
    QTimer::singleShot(0, this, &NoteEncryptionManager::editEncryptedNote);
}

void NoteEncryptionManager::editEncryptedNote() {
    Note &currentNote = _mainWindow->currentNote;
    currentNote.refetch();
    if (!currentNote.hasEncryptedNoteText()) {
        return;
    }

    askForEncryptedNotePasswordIfNeeded(tr("<br />You will be able to edit your encrypted note."));

    if (currentNote.canDecryptNoteText()) {
        const QSignalBlocker blocker(_ui->encryptedNoteTextEdit);
        Q_UNUSED(blocker)

        _ui->noteTextEdit->hide();
        const auto text = currentNote.fetchDecryptedNoteText();
        currentNote.setDecryptedText(text);
        // for some reason this still triggers a "textChanged", so we will do a
        // "currentNote.setDecryptedText" and check if the text really changed
        // in "currentNote.storeNewDecryptedText"
        _ui->encryptedNoteTextEdit->setText(text);
        _ui->encryptedNoteTextEdit->show();
        _ui->encryptedNoteTextEdit->setFocus();
        _mainWindow->setNoteViewNeedsUpdate(true);
        _mainWindow->updateNoteTextEditReadOnly();
        _ui->noteTextEdit->closeMarkdownLspDocument();
    }
}

/**
 * Puts the encrypted text back to the note text edit
 */
void NoteEncryptionManager::on_encryptedNoteTextEdit_textChanged() {
    // this also triggers when formatting is applied / syntax highlighting
    // changes!
    //    if
    //    (currentNote.storeNewDecryptedText(ui->encryptedNoteTextEdit->toPlainText()))
    //    {
    //        handleNoteTextChanged();
    //    }
}

void NoteEncryptionManager::on_encryptedNoteTextEdit_modificationChanged(bool arg1) {
    if (!arg1) {
        return;
    }

    _ui->encryptedNoteTextEdit->document()->setModified(false);

    if (_mainWindow->currentNote.storeNewDecryptedText(_ui->encryptedNoteTextEdit->toPlainText())) {
        _mainWindow->handleNoteTextChanged();
    }
}

/**
 * Exports the current note as Markdown file
 */
void NoteEncryptionManager::on_action_Export_note_as_markdown_triggered() {
    Note &currentNote = _mainWindow->currentNote;
    FileDialog dialog(QStringLiteral("NoteMarkdownExport"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Markdown files") + " (*.md)");
    dialog.setWindowTitle(tr("Export current note as Markdown file"));
    dialog.selectFile(currentNote.getName() + QStringLiteral(".md"));
    const int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            if (QFileInfo(fileName).suffix().isEmpty()) {
                fileName.append(QStringLiteral(".md"));
            }

            bool withAttachedFiles =
                (currentNote.hasMediaFiles() || currentNote.hasAttachments()) &&
                Utils::Gui::question(
                    _mainWindow, tr("Export attached files"),
                    tr("Do you also want to export media files and attachments of "
                       "the note? Files may be overwritten in the destination folder!"),
                    QStringLiteral("note-export-attachments")) == QMessageBox::Yes;

            currentNote.exportToPath(fileName, withAttachedFiles);
        }
    }
}
