/*
 * Copyright (c) 2014-2025 Patrizio Bekerle -- <patrizio@bekerle.com>
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

#pragma once

#include <QContextMenuEvent>
#include <QLabel>
#include <QString>
#include <QWidget>

class NoteFilePathLabel : public QLabel {
    Q_OBJECT

   public:
    explicit NoteFilePathLabel(QWidget *parent = nullptr);
    void setText(const QString &text);
    void updateText();

   protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

   private:
    void adjustTextToFit();
    QString originalText;
};
