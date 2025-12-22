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

#include <QLabel>

class Label : public QLabel {
    Q_OBJECT

   public:
    explicit Label(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    explicit Label(const QString &text, QWidget *parent = nullptr,
                   Qt::WindowFlags f = Qt::WindowFlags());

   public Q_SLOTS:
    void setText(const QString &text);

   public:
    void setEnabled(bool enabled);

   protected:
    static QString injectCSS(const QString &text, bool disabled = false);
    QString m_originalText;
};
