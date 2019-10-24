/*
 * highlighter.h
 *
 * Copyright (C)  2004  Zack Rusin <zack@kde.org>
 * Copyright (C)  2013  Martin Sandsmark <martin.sandsmark@kde.org>
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
#ifndef SONNET_HIGHLIGHTER_H
#define SONNET_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QStringList>
#include "sonnetui_export.h"

class QTextEdit;
class QPlainTextEdit;

namespace Sonnet {
class HighlighterPrivate;
/// The Sonnet Highlighter class, used for drawing pretty red lines in text fields
class SONNETUI_EXPORT Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit Highlighter(QTextEdit *textEdit, const QColor &col = QColor());

    /**
     * @brief Highlighter
     * @param textEdit
     * @param col define spellchecking color.
     * @since 5.12
     */
    explicit Highlighter(QPlainTextEdit *textEdit, const QColor &col = QColor());
    ~Highlighter() override;

    /**
     * Returns whether a spell checking backend with support for the
     * @ref currentLanguage was found.
     *
     * @return true if spell checking is supported for the current language.
     */
    bool spellCheckerFound() const;

    /**
     * Returns the current language used for spell checking.
     *
     * @return the language code for the current language.
     */
    QString currentLanguage() const;

    /**
     * @short Enable/Disable spell checking.
     *
     * If @p active is true then spell checking is enabled; otherwise it
     * is disabled. Note that you have to disable automatic (de)activation
     * with @ref setAutomatic() before you change the state of spell
     * checking if you want to persistently enable/disable spell
     * checking.
     *
     * @param active if true, then spell checking is enabled
     *
     * @see isActive(), setAutomatic()
     */
    void setActive(bool active);

    /**
     * Returns the state of spell checking.
     *
     * @return true if spell checking is active
     *
     * @see setActive()
     */
    bool isActive() const;

    /**
     * Returns the state of the automatic disabling of spell checking.
     *
     * @return true if spell checking is automatically disabled if there's
     * too many errors
     */
    bool automatic() const;

    /**
     * Sets whether to automatically disable spell checking if there's too
     * many errors.
     *
     * @param automatic if true, spell checking will be disabled if there's
     * a significant amount of errors.
     */
    void setAutomatic(bool automatic);

    /**
     * Adds the given word permanently to the dictionary. It will never
     * be marked as misspelled again, even after restarting the application.
     *
     * @param word the word which will be added to the dictionary
     * @since 4.1
     */
    void addWordToDictionary(const QString &word);

    /**
     * Ignores the given word. This word will not be marked misspelled for
     * this session. It will again be marked as misspelled when creating
     * new highlighters.
     *
     * @param word the word which will be ignored
     * @since 4.1
     */
    void ignoreWord(const QString &word);

    /**
     * Returns a list of suggested replacements for the given misspelled word.
     * If the word is not misspelled, the list will be empty.
     *
     * @param word the misspelled word
     * @param max at most this many suggestions will be returned. If this is
     *            -1, as many suggestions as the spell backend supports will
     *            be returned.
     * @return a list of suggested replacements for the word
     * @since 4.1
     */
    QStringList suggestionsForWord(const QString &word, int max = 10);

    /**
     * Returns a list of suggested replacements for the given misspelled word.
     * If the word is not misspelled, the list will be empty.
     *
     * @param word the misspelled word
     * @param cursor the cursor pointing to the beginning of that word. This is used
     *               to determine the language to use, when AutoDetectLanguage is enabled.
     * @param max at most this many suggestions will be returned. If this is
     *            -1, as many suggestions as the spell backend supports will
     *            be returned.
     * @return a list of suggested replacements for the word
     * @since 5.42
     */
    QStringList suggestionsForWord(const QString &word, const QTextCursor &cursor, int max = 10);

    /**
     * Checks if a given word is marked as misspelled by the highlighter.
     *
     * @param word the word to be checked
     * @return true if the given word is misspelled.
     * @since 4.1
     */
    bool isWordMisspelled(const QString &word);

    /**
     * Sets the color in which the highlighter underlines misspelled words.
     * @since 4.2
     */
    void setMisspelledColor(const QColor &color);

    /**
     * Return true if checker is enabled by default
     * @since 4.5
     */
    bool checkerEnabledByDefault() const;

    /**
     * Set a new @ref QTextDocument for this highlighter to operate on.
     *
     * @param document the new document to operate on.
     */
    void setDocument(QTextDocument *document);

Q_SIGNALS:

    /**
     * Emitted when as-you-type spell checking is enabled or disabled.
     *
     * @param description is a i18n description of the new state,
     *        with an optional reason
     */
    void activeChanged(const QString &description);

protected:
    void highlightBlock(const QString &text) override;
    virtual void setMisspelled(int start, int count);
    virtual void unsetMisspelled(int start, int count);

    bool eventFilter(QObject *o, QEvent *e) override;
    bool intraWordEditing() const;
    void setIntraWordEditing(bool editing);

public Q_SLOTS:
    /**
     * Set language to use for spell checking.
     *
     * @param language the language code for the new language to use.
     */
    void setCurrentLanguage(const QString &language);

    /**
     * Run auto detection, disabling spell checking if too many errors are found.
     */
    void slotAutoDetection();

    /**
     * Force a new highlighting.
     */
    void slotRehighlight();

private Q_SLOTS:
    void contentsChange(int pos, int added, int removed);

private:
    HighlighterPrivate *const d;
    Q_DISABLE_COPY(Highlighter)
};
}

#endif
