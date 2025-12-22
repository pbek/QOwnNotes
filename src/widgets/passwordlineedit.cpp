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

#include "passwordlineedit.h"

#include <QAction>

PasswordLineEdit::PasswordLineEdit(QWidget *parent) : QLineEdit(parent) {
    setEchoMode(QLineEdit::Password);

    static const auto visibleIcon = QIcon::fromTheme(
        QStringLiteral("view-visible"), QIcon(":/icons/breeze-qownnotes/16x16/view-visible.svg"));
    static const auto hiddenIcon = QIcon::fromTheme(
        QStringLiteral("view-hidden"), QIcon(":/icons/breeze-qownnotes/16x16/view-hidden.svg"));
    static const auto visibleText = tr("Show password");
    static const auto hiddenText = tr("Hide password");

    auto action = new QAction(visibleIcon, visibleText);

    // Add key event to action
    connect(action, &QAction::triggered, this, [this, action]() {
        if (echoMode() == QLineEdit::Password) {
            setEchoMode(QLineEdit::Normal);
            action->setIcon(hiddenIcon);
            action->setToolTip(hiddenText);
        } else {
            setEchoMode(QLineEdit::Password);
            action->setIcon(visibleIcon);
            action->setToolTip(visibleText);
        }
    });

    this->addAction(action, QLineEdit::TrailingPosition);
}
