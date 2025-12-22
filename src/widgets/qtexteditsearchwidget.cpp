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

#include "qtexteditsearchwidget.h"

#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

#include "ui_qtexteditsearchwidget.h"

QTextEditSearchWidget::QTextEditSearchWidget(QTextEdit *parent)
    : QWidget(parent), ui(new Ui::QTextEditSearchWidget) {
    ui->setupUi(this);
    _textEdit = parent;
    hide();

    QObject::connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(deactivate()));
    QObject::connect(ui->searchLineEdit, SIGNAL(textChanged(QString)), this,
                     SLOT(searchLineEditTextChanged(QString)));
    QObject::connect(ui->searchDownButton, SIGNAL(clicked()), this, SLOT(doSearchDown()));
    QObject::connect(ui->searchUpButton, SIGNAL(clicked()), this, SLOT(doSearchUp()));
    QObject::connect(ui->replaceToggleButton, SIGNAL(toggled(bool)), this,
                     SLOT(setReplaceMode(bool)));
    QObject::connect(ui->replaceButton, SIGNAL(clicked()), this, SLOT(doReplace()));
    QObject::connect(ui->replaceAllButton, SIGNAL(clicked()), this, SLOT(doReplaceAll()));

    installEventFilter(this);
    ui->searchLineEdit->installEventFilter(this);
    ui->replaceLineEdit->installEventFilter(this);

#ifdef Q_OS_MAC
    // set the spacing to 8 for OS X
    layout()->setSpacing(8);
    ui->buttonFrame->layout()->setSpacing(9);

    // set the margin to 0 for the top buttons for OS X
    QString buttonStyle = "QPushButton {margin: 0}";
    ui->closeButton->setStyleSheet(buttonStyle);
    ui->searchDownButton->setStyleSheet(buttonStyle);
    ui->searchUpButton->setStyleSheet(buttonStyle);
    ui->replaceToggleButton->setStyleSheet(buttonStyle);
    ui->matchCaseSensitiveButton->setStyleSheet(buttonStyle);
#endif
}

QTextEditSearchWidget::~QTextEditSearchWidget() { delete ui; }

void QTextEditSearchWidget::setReplaceEnabled(bool enabled) {
    ui->replaceToggleButton->setVisible(enabled);
}

void QTextEditSearchWidget::activate() {
    setReplaceMode(false);
    show();

    // preset the selected text as search text if there is any and there is no
    // other search text
    QString selectedText = _textEdit->textCursor().selectedText();
    if (!selectedText.isEmpty() && ui->searchLineEdit->text().isEmpty()) {
        ui->searchLineEdit->setText(selectedText);
    }

    ui->searchLineEdit->setFocus();
    ui->searchLineEdit->selectAll();
    doSearchDown();
}

void QTextEditSearchWidget::activateReplace() {
    // replacing is prohibited if the text edit is readonly
    if (_textEdit->isReadOnly()) {
        return;
    }

    ui->searchLineEdit->setText(_textEdit->textCursor().selectedText());
    ui->searchLineEdit->selectAll();
    activate();
    setReplaceMode(true);
}

void QTextEditSearchWidget::deactivate() {
    hide();
    _textEdit->setFocus();
}

void QTextEditSearchWidget::setReplaceMode(bool enabled) {
    ui->replaceToggleButton->setChecked(enabled);
    ui->replaceLabel->setVisible(enabled);
    ui->replaceLineEdit->setVisible(enabled);
    ui->modeLabel->setVisible(enabled);
    ui->buttonFrame->setVisible(enabled);
    ui->matchCaseSensitiveButton->setVisible(enabled);
}

bool QTextEditSearchWidget::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_Escape) {
            deactivate();
            return true;
        } else if ((keyEvent->modifiers().testFlag(Qt::ShiftModifier) &&
                    (keyEvent->key() == Qt::Key_Return)) ||
                   (keyEvent->key() == Qt::Key_Up)) {
            doSearchUp();
            return true;
        } else if ((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Down)) {
            doSearchDown();
            return true;
        } else if (keyEvent->key() == Qt::Key_F3) {
            doSearch(!keyEvent->modifiers().testFlag(Qt::ShiftModifier));
            return true;
        }

        //        if ((obj == ui->replaceLineEdit) && (keyEvent->key() ==
        //        Qt::Key_Tab)
        //                && ui->replaceToggleButton->isChecked()) {
        //            ui->replaceLineEdit->setFocus();
        //        }

        return false;
    }

    return QWidget::eventFilter(obj, event);
}

void QTextEditSearchWidget::searchLineEditTextChanged(const QString &arg1) {
    Q_UNUSED(arg1);
    doSearchDown();
}

void QTextEditSearchWidget::doSearchUp() { doSearch(false); }

void QTextEditSearchWidget::doSearchDown() { doSearch(true); }

bool QTextEditSearchWidget::doReplace(bool forAll) {
    if (_textEdit->isReadOnly()) {
        return false;
    }

    QTextCursor c = _textEdit->textCursor();

    if (!forAll && c.selectedText().isEmpty()) {
        return false;
    }

    int searchMode = ui->modeComboBox->currentIndex();
    if (searchMode == RegularExpressionMode) {
        QString text = c.selectedText();
        text.replace(QRegularExpression(ui->searchLineEdit->text()), ui->replaceLineEdit->text());
        c.insertText(text);
    } else {
        c.insertText(ui->replaceLineEdit->text());
    }

    if (!forAll) {
        int position = c.position();

        if (!doSearch(true)) {
            // restore the last cursor position if text wasn't found any more
            c.setPosition(position);
            _textEdit->setTextCursor(c);
        }
    }

    return true;
}

void QTextEditSearchWidget::doReplaceAll() {
    if (_textEdit->isReadOnly()) {
        return;
    }

    // start at the top
    _textEdit->moveCursor(QTextCursor::Start);

    // replace until everything to the bottom is replaced
    while (doSearch(true, false) && doReplace(true)) {
    }
}

/**
 * @brief Searches for text in the text edit
 * @returns true if found
 */
bool QTextEditSearchWidget::doSearch(bool searchDown, bool allowRestartAtTop) {
    QString text = ui->searchLineEdit->text();

    if (text.isEmpty()) {
        ui->searchLineEdit->setStyleSheet(QString());
        return false;
    }

    int searchMode = ui->modeComboBox->currentIndex();

    QFlags<QTextDocument::FindFlag> options =
        searchDown ? QTextDocument::FindFlag(0) : QTextDocument::FindBackward;
    if (searchMode == WholeWordsMode) {
        options |= QTextDocument::FindWholeWords;
    }

    if (ui->matchCaseSensitiveButton->isChecked()) {
        options |= QTextDocument::FindCaseSensitively;
    }

    bool found;
    if (searchMode == RegularExpressionMode) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
        found = _textEdit->find(QRegularExpression(text), options);
#else
        found = _textEdit->find(QRegExp(text), options);
#endif
    } else {
        found = _textEdit->find(text, options);
    }

    // start at the top (or bottom) if not found
    if (!found && allowRestartAtTop) {
        _textEdit->moveCursor(searchDown ? QTextCursor::Start : QTextCursor::End);
        found = _textEdit->find(text, options);
    }

    QRect rect = _textEdit->cursorRect();
    QMargins margins = _textEdit->layout()->contentsMargins();
    int searchWidgetHotArea = _textEdit->height() - this->height();
    int marginBottom = (rect.y() > searchWidgetHotArea) ? (this->height() + 10) : 0;

    // move the search box a bit up if we would block the search result
    if (margins.bottom() != marginBottom) {
        margins.setBottom(marginBottom);
        _textEdit->layout()->setContentsMargins(margins);
    }

    // add a background color according if we found the text or not
    QString colorCode = found ? QStringLiteral("#D5FAE2") : QStringLiteral("#FAE9EB");

    if (_darkMode) {
        colorCode = found ? QStringLiteral("#135a13") : QStringLiteral("#8d2b36");
    }

    ui->searchLineEdit->setStyleSheet(QStringLiteral("* { background: ") + colorCode +
                                      QStringLiteral("; }"));

    return found;
}

void QTextEditSearchWidget::setDarkMode(bool enabled) { _darkMode = enabled; }
