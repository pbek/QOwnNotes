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

#include "newnotedialog.h"

#include <QFontMetrics>

#include "ui_newnotedialog.h"

NewNoteDialog::NewNoteDialog(const QString &defaultNoteName, QWidget *parent)
    : MasterDialog(parent), ui(new Ui::NewNoteDialog) {
    ui->setupUi(this);
    afterSetupUI();

    ui->nameLineEdit->setText(defaultNoteName);
    ui->nameLineEdit->setFocus();

    // Make the dialog wide enough to show the full default note name, with some
    // extra space for the dialog margins. If the dialog was already resized by
    // the user, the stored geometry is restored by MasterDialog when it is opened
    const int textWidth = ui->nameLineEdit->fontMetrics().boundingRect(defaultNoteName).width();
    resize(qMax(sizeHint().width(), textWidth + 50), sizeHint().height());
}

NewNoteDialog::~NewNoteDialog() { delete ui; }

/**
 * Returns the entered note name without surrounding whitespace
 */
QString NewNoteDialog::noteName() const { return ui->nameLineEdit->text().trimmed(); }

void NewNoteDialog::showEvent(QShowEvent *event) {
    MasterDialog::showEvent(event);

    // Select the note name, so it can be overwritten right away
    ui->nameLineEdit->selectAll();
}
