// krazy:excludeall=spelling
/**
 * Copyright (C) 2017  David Faure <faure@kde.org>
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

#include "highlighter.h"
#include "speller.h"

#include <QPlainTextEdit>
#include <QStandardPaths>
#include <QObject>
#include <qtest.h>

using namespace Sonnet;

class HighlighterTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testEnglish();
    void testFrench();
    void testMultipleLanguages();
};

void HighlighterTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);

    Speller speller(QStringLiteral("en_US"));
    if (!speller.availableBackends().contains(QLatin1String("ASpell"))) {
        QSKIP("ASpell not available");
    }
    // Doing this here affects all the highlighters created later on, due to the weird hidden "Settings" class saving stuff behind the API's back...
    speller.setDefaultClient(QStringLiteral("ASpell"));
    if (!speller.availableLanguages().contains(QLatin1String("en"))) {
        QSKIP("'en' not available");
    }

    // How weird to have to do this here and not with the Highlighter API....
    speller.setAttribute(Speller::AutoDetectLanguage, true);
}

static const char s_englishSentence[] = "Hello helo this is the highlighter test enviroment guvernment"; // words from test_suggest.cpp

void HighlighterTest::testEnglish()
{
    // GIVEN
    QPlainTextEdit textEdit;
    textEdit.setPlainText(QString::fromLatin1(s_englishSentence));
    Sonnet::Highlighter highlighter(&textEdit);
    highlighter.setCurrentLanguage(QStringLiteral("en"));
    QVERIFY(highlighter.spellCheckerFound());
    highlighter.rehighlight();
    QTextCursor cursor(textEdit.document());

    // WHEN
    cursor.setPosition(6);
    const QStringList suggestionsForHelo = highlighter.suggestionsForWord(QStringLiteral("helo"), cursor);
    const QStringList unlimitedSuggestions = highlighter.suggestionsForWord(QStringLiteral("helo"), cursor, -1);
    cursor.setPosition(40);
    const QStringList suggestionsForEnviroment = highlighter.suggestionsForWord(QStringLiteral("enviroment"), cursor);

    // THEN
    QCOMPARE(suggestionsForHelo.count(), 10);
    QVERIFY2(suggestionsForHelo.contains(QLatin1String("hello")), qPrintable(suggestionsForHelo.join(QLatin1Char(','))));
    QVERIFY2(suggestionsForEnviroment.contains(QLatin1String("environment")), qPrintable(suggestionsForEnviroment.join(QLatin1Char(','))));
    QVERIFY(unlimitedSuggestions.count() > 10);
}

static const char s_frenchSentence[] = "Bnjour est un bon mot pour tester le dictionnare.";

void HighlighterTest::testFrench()
{
    // GIVEN
    QPlainTextEdit textEdit;
    textEdit.setPlainText(QString::fromLatin1(s_frenchSentence));
    Sonnet::Highlighter highlighter(&textEdit);
    highlighter.setCurrentLanguage(QStringLiteral("fr_FR"));
    QVERIFY(highlighter.spellCheckerFound());
    highlighter.rehighlight();
    QTextCursor cursor(textEdit.document());

    // WHEN
    cursor.setPosition(0);
    const QStringList suggestionsForBnjour = highlighter.suggestionsForWord(QStringLiteral("Bnjour"), cursor);
    cursor.setPosition(37);
    const QStringList suggestionsForDict = highlighter.suggestionsForWord(QStringLiteral("dictionnare"), cursor);

    // THEN
    QVERIFY2(suggestionsForBnjour.contains(QLatin1String("Bonjour")), qPrintable(suggestionsForBnjour.join(QLatin1Char(','))));
    QVERIFY2(suggestionsForDict.contains(QLatin1String("dictionnaire")), qPrintable(suggestionsForDict.join(QLatin1Char(','))));
}

void HighlighterTest::testMultipleLanguages()
{
    // GIVEN
    QPlainTextEdit textEdit;
    const QString englishSentence = QString::fromLatin1(s_englishSentence) + QLatin1Char('\n');
    textEdit.setPlainText(englishSentence + QString::fromLatin1(s_frenchSentence));
    Sonnet::Highlighter highlighter(&textEdit);
    highlighter.rehighlight();
    QTextCursor cursor(textEdit.document());

    // WHEN
    cursor.setPosition(6);
    const QStringList suggestionsForHelo = highlighter.suggestionsForWord(QStringLiteral("helo"), cursor);
    cursor.setPosition(40);
    const QStringList suggestionsForEnviroment = highlighter.suggestionsForWord(QStringLiteral("enviroment"), cursor);
    cursor.setPosition(englishSentence.size());
    const QStringList suggestionsForBnjour = highlighter.suggestionsForWord(QStringLiteral("Bnjour"), cursor);
    cursor.setPosition(englishSentence.size() + 37);
    const QStringList suggestionsForDict = highlighter.suggestionsForWord(QStringLiteral("dictionnare"), cursor);

    // THEN
    QVERIFY2(suggestionsForHelo.contains(QLatin1String("hello")), qPrintable(suggestionsForHelo.join(QLatin1Char(','))));
    QVERIFY2(suggestionsForEnviroment.contains(QLatin1String("environment")), qPrintable(suggestionsForEnviroment.join(QLatin1Char(','))));
    QVERIFY2(suggestionsForBnjour.contains(QLatin1String("Bonjour")), qPrintable(suggestionsForBnjour.join(QLatin1Char(','))));
    QVERIFY2(suggestionsForDict.contains(QLatin1String("dictionnaire")), qPrintable(suggestionsForDict.join(QLatin1Char(','))));
}

QTEST_MAIN(HighlighterTest)

#include "test_highlighter.moc"
