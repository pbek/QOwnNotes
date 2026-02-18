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

#include "qlitehtmlsearchwidget.h"

#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

#include "qlitehtmlwidget.h"
#include "ui_qlitehtmlsearchwidget.h"

QLiteHtmlSearchWidget::QLiteHtmlSearchWidget(QLiteHtmlWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QLiteHtmlSearchWidget)
{
    ui->setupUi(this);
    _liteHtmlWidget = parent;
    hide();

    QObject::connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(deactivate()));
    QObject::connect(ui->searchLineEdit,
                     SIGNAL(textChanged(QString)),
                     this,
                     SLOT(searchLineEditTextChanged(QString)));
    QObject::connect(ui->searchDownButton, SIGNAL(clicked()), this, SLOT(doSearchDown()));
    QObject::connect(ui->searchUpButton, SIGNAL(clicked()), this, SLOT(doSearchUp()));

    installEventFilter(this);
    ui->searchLineEdit->installEventFilter(this);

#ifdef Q_OS_MAC
    layout()->setSpacing(8);
    QString buttonStyle = "QPushButton {margin: 0}";
    ui->closeButton->setStyleSheet(buttonStyle);
    ui->searchDownButton->setStyleSheet(buttonStyle);
    ui->searchUpButton->setStyleSheet(buttonStyle);
    ui->matchCaseSensitiveButton->setStyleSheet(buttonStyle);
#endif
}

QLiteHtmlSearchWidget::~QLiteHtmlSearchWidget()
{
    delete ui;
}

void QLiteHtmlSearchWidget::activate()
{
    show();

    QString selectedText = _liteHtmlWidget->selectedText();
    if (!selectedText.isEmpty() && ui->searchLineEdit->text().isEmpty()) {
        ui->searchLineEdit->setText(selectedText);
    }

    ui->searchLineEdit->setFocus();
    ui->searchLineEdit->selectAll();
    doSearchDown();
}

void QLiteHtmlSearchWidget::deactivate()
{
    hide();
    _liteHtmlWidget->setFocus();
}

bool QLiteHtmlSearchWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_Escape) {
            deactivate();
            return true;
        } else if ((keyEvent->modifiers().testFlag(Qt::ShiftModifier)
                    && (keyEvent->key() == Qt::Key_Return))
                   || (keyEvent->key() == Qt::Key_Up)) {
            doSearchUp();
            return true;
        } else if ((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Down)) {
            doSearchDown();
            return true;
        } else if (keyEvent->key() == Qt::Key_F3) {
            doSearch(!keyEvent->modifiers().testFlag(Qt::ShiftModifier));
            return true;
        }

        return false;
    }

    return QWidget::eventFilter(obj, event);
}

void QLiteHtmlSearchWidget::searchLineEditTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    doSearchDown();
}

void QLiteHtmlSearchWidget::doSearchUp()
{
    doSearch(false);
}

void QLiteHtmlSearchWidget::doSearchDown()
{
    doSearch(true);
}

/**
 * @brief Searches for text in the LiteHtml widget
 * @returns true if found
 */
bool QLiteHtmlSearchWidget::doSearch(bool searchDown, bool allowRestartAtTop)
{
    QString text = ui->searchLineEdit->text();

    if (text.isEmpty()) {
        ui->searchLineEdit->setStyleSheet(QString());
        return false;
    }

    int searchMode = ui->modeComboBox->currentIndex();

    QFlags<QTextDocument::FindFlag> options = searchDown ? QTextDocument::FindFlag(0)
                                                         : QTextDocument::FindBackward;
    if (searchMode == WholeWordsMode) {
        options |= QTextDocument::FindWholeWords;
    }

    if (ui->matchCaseSensitiveButton->isChecked()) {
        options |= QTextDocument::FindCaseSensitively;
    }

    bool wrapped = false;
    bool found = _liteHtmlWidget->findText(text, options, false, &wrapped);

    if (!found && allowRestartAtTop) {
        _liteHtmlWidget->findText(text, options, false, &wrapped);
    }

    QString colorCode = found ? QStringLiteral("#D5FAE2") : QStringLiteral("#FAE9EB");

    if (_darkMode) {
        colorCode = found ? QStringLiteral("#135a13") : QStringLiteral("#8d2b36");
    }

    ui->searchLineEdit->setStyleSheet(QStringLiteral("* { background: ") + colorCode
                                      + QStringLiteral("; }"));

    return found;
}

void QLiteHtmlSearchWidget::setDarkMode(bool enabled)
{
    _darkMode = enabled;
}
