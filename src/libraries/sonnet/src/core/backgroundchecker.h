/*
 * backgroundchecker.h
 *
 * Copyright (C)  2004  Zack Rusin <zack@kde.org>
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
#ifndef SONNET_BACKGROUNDCHECKER_H
#define SONNET_BACKGROUNDCHECKER_H

#include <QObject>

#include "backgroundchecker_p.h"
#include "speller.h"
class BackgroundCheckerPrivate;

/**
 * The sonnet namespace.
 */
namespace Sonnet {
class Speller;

/**
 *
 * BackgroundChecker is used to perform spell checking without
 * blocking the application. You can use it as is by calling
 * the checkText function or subclass it and reimplement
 * getMoreText function.
 *
 * The misspelling signal is emitted whenever a misspelled word
 * is found. The background checker stops right before emitting
 * the signal. So the parent has to call continueChecking function
 * to resume the checking.
 *
 * done signal is emitted when whole text is spell checked.
 *
 * @author Zack Rusin <zack@kde.org>
 * @short class used for spell checking in the background
 */
class BackgroundChecker : public QObject {
    Q_OBJECT
   public:
    explicit BackgroundChecker(QObject *parent = nullptr);
    explicit BackgroundChecker(const Speller &speller, QObject *parent = nullptr);
    ~BackgroundChecker();

    /**
     * This method is used to spell check static text.
     * It automatically invokes start().
     *
     * Use fetchMoreText() with start() to spell check a stream.
     */
    void setText(const QString &text);
    QString text() const;

    QString currentContext() const;

    Speller speller() const;
    void setSpeller(const Speller &speller);

    bool checkWord(const QString &word);
    QStringList suggest(const QString &word) const;
    bool addWordToPersonal(const QString &word);

    /**
     * This method is used to add a word to the session of the
     * speller currently set in BackgroundChecker.
     *
     * @since 5.55
     */
    bool addWordToSession(const QString &word);

   public Q_SLOTS:
    virtual void start();
    virtual void stop();
    void replace(int start, const QString &oldText, const QString &newText);
    void changeLanguage(const QString &lang);

    /**
     * After emitting misspelling signal the background
     * checker stops. The catcher is responsible for calling
     * continueChecking function to resume checking.
     */
    virtual void continueChecking();

   Q_SIGNALS:
    /**
     * Emitted whenever a misspelled word is found
     */
    void misspelling(const QString &word, int start);

    /**
     * Emitted after the whole text has been spell checked.
     */
    void done();

   protected:
    /**
     * This function is called to get the text to spell check.
     * It will be called continuesly until it returns QString()
     * in which case the done() signal is emitted.
     * Note: the start parameter in mispelling() is not a combined
     * position but a position in the last string returned
     * by fetchMoreText. You need to store the state in the derivatives.
     */
    virtual QString fetchMoreText();

    /**
     * This function will be called whenever the background checker
     * will be finished text which it got from fetchMoreText.
     */
    virtual void finishedCurrentFeed();

   protected Q_SLOTS:
    void slotEngineDone();

   private:
    BackgroundCheckerPrivate *const d;
};
}    // namespace Sonnet

#endif
