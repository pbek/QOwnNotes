/*
 * dialog.h
 *
 * Copyright (C)  2003  Zack Rusin <zack@kde.org>
 * Copyright (C)  2009-2010  Michel Ludwig <michel.ludwig@kdemail.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */
#ifndef SONNET_DIALOG_H
#define SONNET_DIALOG_H

#include <QDialog>
#include "sonnetui_export.h"

class QListWidgetItem;
class QModelIndex;

namespace Sonnet {
class BackgroundChecker;
class DialogPrivate;
/**
 * @short Spellcheck dialog
 *
 * \code
 * Sonnet::Dialog dlg = new Sonnet::Dialog(
 *       new Sonnet::BackgroundChecker(this), this);
 * //connect signals
 * ...
 * dlg->setBuffer( someText );
 * dlg->show();
 * \endcode
 *
 * You can change buffer inside a slot connected to done() signal
 * and spellcheck will continue with new data automatically.
 */
class SONNETUI_EXPORT Dialog : public QDialog
{
    Q_OBJECT
public:
    Dialog(BackgroundChecker *checker, QWidget *parent);
    ~Dialog();

    QString originalBuffer() const;
    QString buffer() const;

    void show();
    void activeAutoCorrect(bool _active);

    // Hide warning about done(), which is a slot in QDialog and a signal here.
    using QDialog::done;

    /**
     * Controls whether an (indefinite) progress dialog is shown when the spell
     * checking takes longer than the given time to complete. By default no
     * progress dialog is shown. If the progress dialog is set to be shown, no
     * time consuming operation (for example, showing a notification message) should
     * be performed in a slot connected to the 'done' signal as this might trigger
     * the progress dialog unnecessarily.
     *
     * @param timeout time after which the progress dialog should appear; a negative
     *                value can be used to hide it
     * @since 4.4
     */
    void showProgressDialog(int timeout = 500);

    /**
     * Controls whether a message box indicating the completion of the spell checking
     * is shown or not. By default it is not shown.
     *
     * @since 4.4
     */
    void showSpellCheckCompletionMessage(bool b = true);

    /**
     * Controls whether the spell checking is continued after the replacement of a
     * misspelled word has been performed. By default it is continued.
     *
     * @since 4.4
     */
    void setSpellCheckContinuedAfterReplacement(bool b);

public Q_SLOTS:
    void setBuffer(const QString &);

Q_SIGNALS:
    /**
     * The dialog won't be closed if you setBuffer() in slot connected to this signal
     *
     * Also emitted after stop() signal
     */
    void done(const QString &newBuffer);
    void misspelling(const QString &word, int start);
    void replace(const QString &oldWord, int start, const QString &newWord);

    void stop();
    void cancel();
    void autoCorrect(const QString &currentWord, const QString &replaceWord);

    /**
     * Signal sends when spell checking is finished/stopped/completed
     * @since 4.1
     */
    void spellCheckStatus(const QString &);

    /**
     * Emitted when the user changes the language used for spellchecking,
     * which is shown in a combobox of this dialog.
     *
     * @param dictionary the new language the user selected
     * @since 4.1
     */
    void languageChanged(const QString &language);

private Q_SLOTS:
    void slotMisspelling(const QString &word, int start);
    void slotDone();

    void slotFinished();
    void slotCancel();

    void slotAddWord();
    void slotReplaceWord();
    void slotReplaceAll();
    void slotSkip();
    void slotSkipAll();
    void slotSuggest();
    void slotChangeLanguage(const QString &);
    void slotSelectionChanged(const QModelIndex &);
    void slotAutocorrect();

    void setGuiEnabled(bool b);
    void setProgressDialogVisible(bool b);

private:
    void updateDialog(const QString &word);
    void fillDictionaryComboBox();
    void updateDictionaryComboBox();
    void fillSuggestions(const QStringList &suggs);
    void initConnections();
    void initGui();
    void continueChecking();

private:
    DialogPrivate *const d;
    Q_DISABLE_COPY(Dialog)
};
}

#endif
