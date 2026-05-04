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
#include <QScrollBar>
#include <QTextBoundaryFinder>

#include "ui_qtexteditsearchwidget.h"

namespace {
bool isEmojiCodePoint(unsigned int codePoint) {
    return codePoint == 0x20E3 || codePoint == 0x00A9 || codePoint == 0x00AE ||
           codePoint == 0x203C || codePoint == 0x2049 || codePoint == 0x2122 ||
           codePoint == 0x2139 || (codePoint >= 0x2194 && codePoint <= 0x21AA) ||
           (codePoint >= 0x231A && codePoint <= 0x2328) || codePoint == 0x23CF ||
           (codePoint >= 0x23E9 && codePoint <= 0x23FA) || codePoint == 0x24C2 ||
           (codePoint >= 0x25AA && codePoint <= 0x25AB) || codePoint == 0x25B6 ||
           codePoint == 0x25C0 || (codePoint >= 0x25FB && codePoint <= 0x25FE) ||
           (codePoint >= 0x2600 && codePoint <= 0x27BF) ||
           (codePoint >= 0x2934 && codePoint <= 0x2935) ||
           (codePoint >= 0x2B05 && codePoint <= 0x2B55) || codePoint == 0x3030 ||
           codePoint == 0x303D || codePoint == 0x3297 || codePoint == 0x3299 ||
           (codePoint >= 0x1F000 && codePoint <= 0x1FAFF);
}

int graphemeCount(const QString &text, int maxCount) {
    if (text.isEmpty()) {
        return 0;
    }

    QTextBoundaryFinder finder(QTextBoundaryFinder::Grapheme, text);
    finder.toStart();

    int count = 0;
    while (finder.toNextBoundary() != -1) {
        ++count;

        if (count >= maxCount) {
            break;
        }
    }

    return count;
}

bool shouldStartSearch(const QString &text) {
    const int minimumSearchLength = 2;
    const int count = graphemeCount(text, minimumSearchLength);
    if (count >= minimumSearchLength) {
        return true;
    }

    if (count != 1) {
        return false;
    }

    const auto codePoints = text.toUcs4();
    for (unsigned int codePoint : codePoints) {
        if (isEmojiCodePoint(codePoint)) {
            return true;
        }
    }

    return false;
}
}    // namespace

QTextEditSearchWidget::QTextEditSearchWidget(QTextEdit *parent)
    : QWidget(parent), ui(new Ui::QTextEditSearchWidget), _textEdit(parent), _darkMode(false) {
    ui->setupUi(this);
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

    // Set up debounce timer so the search is delayed while the user is still
    // typing
    _debounceTimer.setSingleShot(true);
    _debounceTimer.setInterval(300);
    QObject::connect(&_debounceTimer, &QTimer::timeout, this, &QTextEditSearchWidget::doSearchDown);

    installEventFilter(this);
    ui->searchLineEdit->installEventFilter(this);
    ui->replaceLineEdit->installEventFilter(this);

#ifdef Q_OS_MAC
    // set the spacing to 8 for macOS
    layout()->setSpacing(8);
    ui->buttonFrame->layout()->setSpacing(9);

    // set the margin to 0 for the top buttons for macOS
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

    const int verticalScrollBarValue = _textEdit->verticalScrollBar()->value();
    const int horizontalScrollBarValue = _textEdit->horizontalScrollBar()->value();

    // Preset the selected text as search text if there is any, replacing any
    // existing search text. Position the cursor one character before the
    // selection start so that doSearchDown() (which searches strictly after the
    // cursor) lands on the originally selected occurrence rather than the next
    // one, keeping the view at the user's current position (for #3541).
    const QTextCursor originalCursor = _textEdit->textCursor();
    const QString selectedText = originalCursor.selectedText();
    if (!selectedText.isEmpty()) {
        ui->searchLineEdit->setText(selectedText);

        // QTextEdit::find() starts searching strictly after the cursor position,
        // so place the cursor one character before the selection start to ensure
        // the first hit is the originally selected word itself.
        // (setText above may have already triggered an intermediate search via
        // the textChanged signal, so we always reset the position here.)
        QTextCursor cursor = originalCursor;
        const int pos = originalCursor.selectionStart();
        cursor.setPosition(pos > 0 ? pos - 1 : 0);
        _textEdit->setTextCursor(cursor);
    }

    ui->searchLineEdit->setFocus();
    ui->searchLineEdit->selectAll();
    doSearchDown();

    _textEdit->verticalScrollBar()->setValue(verticalScrollBarValue);
    _textEdit->horizontalScrollBar()->setValue(horizontalScrollBarValue);
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
    // If the search term is too short, just clear the style without jumping to
    // the top of the document
    if (!shouldStartSearch(arg1)) {
        _debounceTimer.stop();
        ui->searchLineEdit->setStyleSheet(QString());
        return;
    }

    // Debounce: delay the search while the user is still typing
    _debounceTimer.start();
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

    if (!shouldStartSearch(text)) {
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

    if (_textEdit->layout() != nullptr) {
        QRect rect = _textEdit->cursorRect();
        QMargins margins = _textEdit->layout()->contentsMargins();
        int searchWidgetHotArea = _textEdit->height() - this->height();
        int marginBottom = (rect.y() > searchWidgetHotArea) ? (this->height() + 10) : 0;

        // move the search box a bit up if we would block the search result
        if (margins.bottom() != marginBottom) {
            margins.setBottom(marginBottom);
            _textEdit->layout()->setContentsMargins(margins);
        }
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
