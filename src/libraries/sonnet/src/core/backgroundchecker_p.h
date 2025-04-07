/**
 * backgroundchecker_p.h
 *
 * Copyright (C)  2009  Jakub Stachowski <qbast@go2.pl>
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
#ifndef SONNET_BACKGROUNDCHECKER_P_H
#define SONNET_BACKGROUNDCHECKER_P_H

#include <QObject>

#include "backgroundchecker.h"
#include "languagefilter_p.h"
#include "speller.h"
#include "tokenizer_p.h"

using namespace Sonnet;

class BackgroundCheckerPrivate : public QObject {
    Q_OBJECT
   public:
    BackgroundCheckerPrivate() : mainTokenizer(new SentenceTokenizer), sentenceOffset(-1) {}

    void start();
    void continueChecking();

    LanguageFilter mainTokenizer;
    WordTokenizer words;
    Token lastMisspelled;
    Speller currentDict;
    int sentenceOffset;

    virtual ~BackgroundCheckerPrivate() {}

   private slots:
    void checkNext();
   signals:
    void misspelling(const QString &, int);
    void done();
};

#endif
