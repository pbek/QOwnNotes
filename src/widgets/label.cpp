/*
 * Copyright (c) 2014-2023 Patrizio Bekerle -- <patrizio@bekerle.com>
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

#include "label.h"

#include <utils/misc.h>

Label::Label(QWidget *parent, Qt::WindowFlags f) {
    Q_UNUSED(parent);
    Q_UNUSED(f);
}

Label::Label(const QString &text, QWidget *parent, Qt::WindowFlags f) {
    Q_UNUSED(text);
    Q_UNUSED(parent);
    Q_UNUSED(f);
    //    QLabel::QLabel(injectCSS(text), parent, f);
}

/**
 * Sets a text with injected CSS styles
 *
 * @param text
 */
void Label::setText(const QString &text) { QLabel::setText(injectCSS(text)); }

/**
 * Injects CSS styles into the text
 *
 * @param text
 * @return
 */
QString Label::injectCSS(const QString &text) {
    //    text = text.remove("<html>").remove("</html>").remove("<head/>");
    QString ret;
    ret.reserve(text.size() + 15 + Utils::Misc::genericCSS().size() + text.size());
    ret.append(QStringLiteral("<style>"));
    ret.append(Utils::Misc::genericCSS());
    ret.append(QStringLiteral("</style>"));
    ret.append(text);
    return ret;
}
