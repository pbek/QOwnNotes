/*
 * spellcheckdecorator.h
 *
 * Copyright (C)  2013  Aurélien Gâteau <agateau@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */
#ifndef SPELLCHECKDECORATOR_H
#define SPELLCHECKDECORATOR_H

#include <QObject>

#include "sonnetui_export.h"

class QTextEdit;
class QPlainTextEdit;

namespace Sonnet {
class Highlighter;

/**
 * @short Connects a Sonnet::Highlighter to a QTextEdit extending the context menu of the text edit with spell check suggestions
 * @author Aurélien Gâteau <agateau@kde.org>
 * @since 5.0
 **/

class SONNETUI_EXPORT SpellCheckDecorator : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates a spell-check decorator.
     *
     * @param textEdit the QTextEdit in need of spell-checking. It also is used as the QObject parent for the decorator.
     */
    explicit SpellCheckDecorator(QTextEdit *textEdit);

    /**
     * Creates a spell-check decorator.
     *
     * @param textEdit the QPlainTextEdit in need of spell-checking. It also is used as the QObject parent for the decorator.
     * @since 5.12
     */
    explicit SpellCheckDecorator(QPlainTextEdit *textEdit);

    ~SpellCheckDecorator() override;

    /**
     * Set a custom highlighter on the decorator.
     *
     * SpellCheckDecorator does not take ownership of the new highlighter,
     * and you must manually delete the old highlighter.
     */
    void setHighlighter(Highlighter *highlighter);

    /**
     * Returns the hightlighter used by the decorator
     */
    Highlighter *highlighter() const;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

    /**
     * Returns true if the spell checking should be enabled for a given block of text
     * The default implementation always returns true.
     */
    virtual bool isSpellCheckingEnabledForBlock(const QString &textBlock) const;

private:
    class Private;
    Private *const d;
    Q_DISABLE_COPY(SpellCheckDecorator)
};
}

#endif /* SPELLCHECKDECORATOR_H */
