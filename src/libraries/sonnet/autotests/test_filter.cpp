/**
 * test_filter.cpp
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

#include "test_filter.h"
#include "tokenizer_p.h"
#include <QVector>
#include <qtest.h>

QTEST_GUILESS_MAIN(SonnetFilterTest)

using namespace Sonnet;

struct Hit {
    Hit(const QString &w, int s) : word(w), start(s) {}
    QString word;
    int start;
};

void SonnetFilterTest::testLatin()
{
    QString buffer(QStringLiteral("  This is     a sample thing. Please test me ...     He's don't Le'Clerk."));
    QList<Hit> hits;
    hits.append(Hit(QStringLiteral("This"), 2));
    hits.append(Hit(QStringLiteral("is"), 7));
    hits.append(Hit(QStringLiteral("a"), 14));
    hits.append(Hit(QStringLiteral("sample"), 16));
    hits.append(Hit(QStringLiteral("thing"), 23));
    hits.append(Hit(QStringLiteral("Please"), 30));
    hits.append(Hit(QStringLiteral("test"), 37));
    hits.append(Hit(QStringLiteral("me"), 42));
    hits.append(Hit(QStringLiteral("He's"), 53));
    hits.append(Hit(QStringLiteral("don't"), 58));
    hits.append(Hit(QStringLiteral("Le'Clerk"), 64));

    WordTokenizer tokenizer;
    tokenizer.setBuffer(buffer);

    QStringRef w;
    int hitNumber = 0;
    while (tokenizer.hasNext()) {
        w = tokenizer.next();
        QCOMPARE(w.toString(), hits[hitNumber].word);
        QCOMPARE(w.position(), hits[hitNumber].start);
        ++hitNumber;
    }
    QCOMPARE(hitNumber, hits.count());
}

static QVector<ushort>
convertToUnicode(const QString &str)
{
    QVector<ushort> unicode;
    for (int i = 0; i < str.length(); ++i) {
        unicode += str[i].unicode();
    }
    return unicode;
}

void SonnetFilterTest::testIndic()
{
    QString buffer;
    QList<Hit> hits;
    hits.append(Hit(QString::fromUtf8("मराठी"), 0));
    hits.append(Hit(QString::fromUtf8("भाषा"), 6));
    hits.append(Hit(QString::fromUtf8("महाराष्ट्र"), 11));
    hits.append(Hit(QString::fromUtf8("व"), 22));
    hits.append(Hit(QString::fromUtf8("गोवा"), 24));
    hits.append(Hit(QString::fromUtf8("राज्याची"), 29));
    hits.append(Hit(QString::fromUtf8("राजभाषा"), 38));
    hits.append(Hit(QString::fromUtf8("असून"), 46));
    hits.append(Hit(QString::fromUtf8("सुमारे"), 51));
    hits.append(Hit( QString::fromUtf8("९"), 58 ) ); // This is the number 9, so we don't spell-check it
    hits.append(Hit(QString::fromUtf8("कोटी"), 60));
    hits.append(Hit(QString::fromUtf8("लोकांची"), 65));
    hits.append(Hit(QString::fromUtf8("मातृभाषा"), 73));
    hits.append(Hit(QString::fromUtf8("आहे"), 82));
    hits.append(Hit(QString::fromUtf8("मराठी"), 87));
    hits.append(Hit(QString::fromUtf8("भाषा"), 93));
    hits.append(Hit(QString::fromUtf8("कमीत"), 98));
    hits.append(Hit(QString::fromUtf8("कमी"), 103));
    hits.append( Hit( QString::fromUtf8("१०००"), 107 ) ); // just a number
    hits.append(Hit(QString::fromUtf8("वर्षापासून"), 112));
    hits.append(Hit(QString::fromUtf8("अस्तित्वात"), 123));
    hits.append(Hit(QString::fromUtf8("आहे"), 134));

    buffer = QString::fromUtf8("मराठी भाषा महाराष्ट्र व गोवा राज्याची राजभाषा असून सुमारे ९ कोटी लोकांची मातृभाषा आहे. मराठी भाषा कमीत कमी १००० वर्षापासून अस्तित्वात आहे.");

    WordTokenizer tokenizer;
    tokenizer.setBuffer(buffer);
    QStringRef w;
    int hitNumber = 0;
    while (tokenizer.hasNext()) {
        w = tokenizer.next();
         QVector<ushort> unicode = convertToUnicode(w.toString());
        QCOMPARE(w.toString(), hits[hitNumber].word);
        QCOMPARE(w.position(), hits[hitNumber].start);
        ++hitNumber;
    }
    QCOMPARE(hitNumber, hits.count());
}

void SonnetFilterTest::testSentence()
{
    QString buffer(QStringLiteral("This is     a sample thing. Please test me ...     He's don't Le'Clerk."));
    QList<Hit> hits;
    hits.append(Hit(QStringLiteral("This is     a sample thing. "), 0));
    hits.append(Hit(QStringLiteral("Please test me ...     "), 28));
    hits.append(Hit(QStringLiteral("He's don't Le'Clerk."), 51));

    SentenceTokenizer tokenizer;
    tokenizer.setBuffer(buffer);

    QStringRef w;
    int hitNumber = 0;
    while (tokenizer.hasNext()) {
        w = tokenizer.next();
        QCOMPARE(w.toString(), hits[hitNumber].word);
        QCOMPARE(w.position(), hits[hitNumber].start);
        ++hitNumber;
    }
    QCOMPARE(hitNumber, hits.count());
}

