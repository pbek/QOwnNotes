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

#include "qrcodewidget.h"

#include <QPainter>

#include "libraries/qr-code-generator/QrCode.hpp"

QRCodeWidget::QRCodeWidget(QWidget *parent, Qt::WindowFlags f) {
    Q_UNUSED(parent);
    Q_UNUSED(f);
    _text = "";
}

void QRCodeWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    paintQR(painter, size(), _text, QColor(0, 0, 0), QColor(255, 255, 255));
}

void QRCodeWidget::paintQR(QPainter &painter, const QSize sz, const QString &data, const QColor &fg,
                           const QColor &bg) {
    char *str = data.toUtf8().data();
    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(str, qrcodegen::QrCode::Ecc::LOW);
    const int s = qr.getSize() > 0 ? qr.getSize() : 1;
    const double w = sz.width();
    const double h = sz.height();
    const double aspect = w / h;
    const double size = ((aspect > 1.0) ? h : w);
    const double scale = size / (s + 2);
    painter.setPen(Qt::NoPen);
    painter.setBrush(bg);
    painter.drawRect(QRect(0, 0, (int)w, (int)h));
    painter.setBrush(fg);

    for (int y = 0; y < s; y++) {
        for (int x = 0; x < s; x++) {
            const int color = qr.getModule(x, y);    // 0 for white, 1 for black
            if (0x0 != color) {
                const double rx1 = (x + 1) * scale, ry1 = (y + 1) * scale;
                QRectF r(rx1, ry1, scale, scale);
                painter.drawRects(&r, 1);
            }
        }
    }
}

void QRCodeWidget::setText(const QString &text) {
    _text = text;
    repaint();
}
