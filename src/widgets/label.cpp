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
void Label::setText(const QString &text) {
    m_originalText = text;
    QLabel::setText(injectCSS(text, !isEnabled()));
}

void Label::setEnabled(bool enabled) {
    QLabel::setEnabled(enabled);

    // Re-apply text with correct CSS for enabled/disabled state
    QLabel::setText(injectCSS(m_originalText, !enabled));
}

/**
 * Injects CSS styles into the text
 *
 * @param text
 * @return
 */
QString Label::injectCSS(const QString &text, bool disabled) {
    QString ret;
    const QString &genericCss = Utils::Misc::genericCSS();
    const int tagSize = disabled ? 94 : 15;    // 15 for enabled, 94 for disabled links

    ret.reserve(text.size() + tagSize + genericCss.size() + text.size());
    ret.append(QStringLiteral("<style>"));
    ret.append(genericCss);

    if (disabled) {
        // Set a different color for disabled links
        ret.append(QStringLiteral(
            "a { color: gray !important; pointer-events: none; text-decoration: underline; }"));
    }

    ret.append(QStringLiteral("</style>"));
    ret.append(text);
    return ret;
}
