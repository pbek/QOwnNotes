/**
 * dialog.cpp
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
#include "dialog.h"
#include "ui_sonnetui.h"

#include "backgroundchecker.h"
#include "speller.h"
#include "settings_p.h"

#include <qprogressdialog.h>

#include <QDialogButtonBox>
#include <QStringListModel>
#include <QPushButton>
#include <QMessageBox>

namespace Sonnet {
//to initially disable sorting in the suggestions listview
#define NONSORTINGCOLUMN 2

class ReadOnlyStringListModel : public QStringListModel
{
public:
    ReadOnlyStringListModel(QObject *parent) : QStringListModel(parent)
    {
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        Q_UNUSED(index);
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
};

class DialogPrivate
{
public:
    Ui_SonnetUi ui;
    ReadOnlyStringListModel *suggestionsModel = nullptr;
    QWidget *wdg = nullptr;
    QDialogButtonBox *buttonBox = nullptr;
    QProgressDialog *progressDialog = nullptr;
    QString originalBuffer;
    BackgroundChecker *checker = nullptr;

    QString currentWord;
    int currentPosition;
    QMap<QString, QString> replaceAllMap;
    bool restart;//used when text is distributed across several qtextedits, eg in KAider

    QMap<QString, QString> dictsMap;

    int progressDialogTimeout;
    bool showCompletionMessageBox;
    bool spellCheckContinuedAfterReplacement;
    bool canceled;

    void deleteProgressDialog(bool directly)
    {
        if (progressDialog) {
            progressDialog->hide();
            if (directly) {
                delete progressDialog;
            } else {
                progressDialog->deleteLater();
            }
            progressDialog = nullptr;
        }
    }
};

Dialog::Dialog(BackgroundChecker *checker, QWidget *parent)
    : QDialog(parent)
    , d(new DialogPrivate)
{
    setModal(true);
    setWindowTitle(tr("Check Spelling", "@title:window"));

    d->checker = checker;

    d->canceled = false;
    d->showCompletionMessageBox = false;
    d->spellCheckContinuedAfterReplacement = true;
    d->progressDialogTimeout = -1;
    d->progressDialog = nullptr;

    initGui();
    initConnections();
}

Dialog::~Dialog()
{
    delete d;
}

void Dialog::initConnections()
{
    connect(d->ui.m_addBtn, &QAbstractButton::clicked,
            this, &Dialog::slotAddWord);
    connect(d->ui.m_replaceBtn, &QAbstractButton::clicked,
            this, &Dialog::slotReplaceWord);
    connect(d->ui.m_replaceAllBtn, &QAbstractButton::clicked,
            this, &Dialog::slotReplaceAll);
    connect(d->ui.m_skipBtn, &QAbstractButton::clicked,
            this, &Dialog::slotSkip);
    connect(d->ui.m_skipAllBtn, &QAbstractButton::clicked,
            this, &Dialog::slotSkipAll);
    connect(d->ui.m_suggestBtn, &QAbstractButton::clicked,
            this, &Dialog::slotSuggest);
    connect(d->ui.m_language, SIGNAL(activated(QString)),
            SLOT(slotChangeLanguage(QString)));
    connect(d->ui.m_suggestions, SIGNAL(clicked(QModelIndex)),
            SLOT(slotSelectionChanged(QModelIndex)));
    connect(d->checker, SIGNAL(misspelling(QString,int)),
            SLOT(slotMisspelling(QString,int)));
    connect(d->checker, SIGNAL(done()),
            SLOT(slotDone()));
    connect(d->ui.m_suggestions, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(slotReplaceWord()));
    connect(d->buttonBox, &QDialogButtonBox::accepted, this, &Dialog::slotFinished);
    connect(d->buttonBox, &QDialogButtonBox::rejected, this, &Dialog::slotCancel);
    connect(d->ui.m_replacement, SIGNAL(returnPressed()), this, SLOT(slotReplaceWord()));
    connect(d->ui.m_autoCorrect, SIGNAL(clicked()),
            SLOT(slotAutocorrect()));
    // button use by kword/kpresenter
    // hide by default
    d->ui.m_autoCorrect->hide();
}

void Dialog::initGui()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    d->wdg = new QWidget(this);
    d->ui.setupUi(d->wdg);
    layout->addWidget(d->wdg);
    setGuiEnabled(false);

    d->buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    layout->addWidget(d->wdg);
    layout->addWidget(d->buttonBox);

    //d->ui.m_suggestions->setSorting( NONSORTINGCOLUMN );
    fillDictionaryComboBox();
    d->restart = false;

    d->suggestionsModel = new ReadOnlyStringListModel(this);
    d->ui.m_suggestions->setModel(d->suggestionsModel);
}

void Dialog::activeAutoCorrect(bool _active)
{
    if (_active) {
        d->ui.m_autoCorrect->show();
    } else {
        d->ui.m_autoCorrect->hide();
    }
}

void Dialog::showProgressDialog(int timeout)
{
    d->progressDialogTimeout = timeout;
}

void Dialog::showSpellCheckCompletionMessage(bool b)
{
    d->showCompletionMessageBox = b;
}

void Dialog::setSpellCheckContinuedAfterReplacement(bool b)
{
    d->spellCheckContinuedAfterReplacement = b;
}

void Dialog::slotAutocorrect()
{
    setGuiEnabled(false);
    setProgressDialogVisible(true);
    emit autoCorrect(d->currentWord, d->ui.m_replacement->text());
    slotReplaceWord();
}

void Dialog::setGuiEnabled(bool b)
{
    d->wdg->setEnabled(b);
}

void Dialog::setProgressDialogVisible(bool b)
{
    if (!b) {
        d->deleteProgressDialog(true);
    } else if (d->progressDialogTimeout >= 0) {
        if (d->progressDialog) {
            return;
        }
        d->progressDialog = new QProgressDialog(this);
        d->progressDialog->setLabelText(tr("Spell checking in progress...", "progress label"));
        d->progressDialog->setWindowTitle(tr("Check Spelling", "@title:window"));
        d->progressDialog->setModal(true);
        d->progressDialog->setAutoClose(false);
        d->progressDialog->setAutoReset(false);
        // create an 'indefinite' progress box as we currently cannot get progress feedback from
        // the speller
        d->progressDialog->reset();
        d->progressDialog->setRange(0, 0);
        d->progressDialog->setValue(0);
        connect(d->progressDialog, &QProgressDialog::canceled, this, &Dialog::slotCancel);
        d->progressDialog->setMinimumDuration(d->progressDialogTimeout);
    }
}

void Dialog::slotFinished()
{
    setProgressDialogVisible(false);
    emit stop();
    //FIXME: should we emit done here?
    emit done(d->checker->text());
    emit spellCheckStatus(tr("Spell check stopped."));
    accept();
}

void Dialog::slotCancel()
{
    d->canceled = true;
    d->deleteProgressDialog(false); // this method can be called in response to
    // pressing 'Cancel' on the dialog
    emit cancel();
    emit spellCheckStatus(tr("Spell check canceled."));
    reject();
}

QString Dialog::originalBuffer() const
{
    return d->originalBuffer;
}

QString Dialog::buffer() const
{
    return d->checker->text();
}

void Dialog::setBuffer(const QString &buf)
{
    d->originalBuffer = buf;
    //it is possible to change buffer inside slot connected to done() signal
    d->restart = true;
}

void Dialog::fillDictionaryComboBox()
{
    // Since m_language is changed to DictionaryComboBox most code here is gone,
    // So fillDictionaryComboBox() could be removed and code moved to initGui()
    // because the call in show() looks obsolete
    Speller speller = d->checker->speller();
    d->dictsMap = speller.availableDictionaries();

    updateDictionaryComboBox();
}

void Dialog::updateDictionaryComboBox()
{
    const Speller &speller = d->checker->speller();
    d->ui.m_language->setCurrentByDictionary(speller.language());
}

void Dialog::updateDialog(const QString &word)
{
    d->ui.m_unknownWord->setText(word);
    d->ui.m_contextLabel->setText(d->checker->currentContext());
    const QStringList suggs = d->checker->suggest(word);

    if (suggs.isEmpty()) {
        d->ui.m_replacement->clear();
    } else {
        d->ui.m_replacement->setText(suggs.first());
    }
    fillSuggestions(suggs);
}

void Dialog::show()
{
    d->canceled = false;
    fillDictionaryComboBox();
    if (d->originalBuffer.isEmpty()) {
        d->checker->start();
    } else {
        d->checker->setText(d->originalBuffer);
    }
    setProgressDialogVisible(true);
}

void Dialog::slotAddWord()
{
    setGuiEnabled(false);
    setProgressDialogVisible(true);
    d->checker->addWordToPersonal(d->currentWord);
    d->checker->continueChecking();
}

void Dialog::slotReplaceWord()
{
    setGuiEnabled(false);
    setProgressDialogVisible(true);
    QString replacementText = d->ui.m_replacement->text();
    emit replace(d->currentWord, d->currentPosition,
                 replacementText);

    if (d->spellCheckContinuedAfterReplacement) {
        d->checker->replace(d->currentPosition,
                            d->currentWord,
                            replacementText);
        d->checker->continueChecking();
    } else {
        d->checker->stop();
    }
}

void Dialog::slotReplaceAll()
{
    setGuiEnabled(false);
    setProgressDialogVisible(true);
    d->replaceAllMap.insert(d->currentWord,
                            d->ui.m_replacement->text());
    slotReplaceWord();
}

void Dialog::slotSkip()
{
    setGuiEnabled(false);
    setProgressDialogVisible(true);
    d->checker->continueChecking();
}

void Dialog::slotSkipAll()
{
    setGuiEnabled(false);
    setProgressDialogVisible(true);
    //### do we want that or should we have a d->ignoreAll list?
    Speller speller = d->checker->speller();
    speller.addToPersonal(d->currentWord);
    d->checker->setSpeller(speller);
    d->checker->continueChecking();
}

void Dialog::slotSuggest()
{
    QStringList suggs = d->checker->suggest(d->ui.m_replacement->text());
    fillSuggestions(suggs);
}

void Dialog::slotChangeLanguage(const QString &lang)
{
    Speller speller = d->checker->speller();
    QString languageCode = d->dictsMap[lang];
    if (!languageCode.isEmpty()) {
        d->checker->changeLanguage(languageCode);
        slotSuggest();
        emit languageChanged(languageCode);
    }
}

void Dialog::slotSelectionChanged(const QModelIndex &item)
{
    d->ui.m_replacement->setText(item.data().toString());
}

void Dialog::fillSuggestions(const QStringList &suggs)
{
    d->suggestionsModel->setStringList(suggs);
}

void Dialog::slotMisspelling(const QString &word, int start)
{
    setGuiEnabled(true);
    setProgressDialogVisible(false);
    emit misspelling(word, start);
    //NOTE this is HACK I had to introduce because BackgroundChecker lacks 'virtual' marks on methods
    //this dramatically reduces spellchecking time in Lokalize
    //as this doesn't fetch suggestions for words that are present in msgid
    if (!updatesEnabled()) {
        return;
    }

    d->currentWord = word;
    d->currentPosition = start;
    if (d->replaceAllMap.contains(word)) {
        d->ui.m_replacement->setText(d->replaceAllMap[ word ]);
        slotReplaceWord();
    } else {
        updateDialog(word);
    }
    QDialog::show();
}

void Dialog::slotDone()
{
    d->restart = false;
    emit done(d->checker->text());
    if (d->restart) {
        updateDictionaryComboBox();
        d->checker->setText(d->originalBuffer);
        d->restart = false;
    } else {
        setProgressDialogVisible(false);
        emit spellCheckStatus(tr("Spell check complete."));
        accept();
        if (!d->canceled && d->showCompletionMessageBox) {
            QMessageBox::information(this, tr("Spell check complete."),
                                     tr("Check Spelling", "@title:window"));
        }
    }
}
}
