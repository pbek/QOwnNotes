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

#pragma once

#include <QWidget>

class QRCodeWidget : public QWidget {
    Q_OBJECT

   public:
    explicit QRCodeWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    void setText(const QString &text);

   protected:
    void paintEvent(QPaintEvent *event) override;
    static void paintQR(QPainter &painter, QSize sz, const QString &data, const QColor &fg,
                        const QColor &bg);

   private:
    QString _text;
};
