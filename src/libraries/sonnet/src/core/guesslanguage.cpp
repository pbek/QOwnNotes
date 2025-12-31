/*  This file is part of the KDE libraries
    Copyright (c) 2006 Jacob R Rideout <kde@jacobrideout.net>
    Copyright (c) 2009 Jakub Stachowski <qbast@go2.pl>
    Copyright (c) 2013 Martin Sandsmark <martin.sandsmark@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "guesslanguage.h"

#include <QDataStream>
#include <QFile>
#include <QLocale>
#include <QStandardPaths>
#include <QVector>
#include <QtGlobal>

#include "core_debug.h"
#include "loader_p.h"
#include "speller.h"
#include "spellerplugin_p.h"
#include "tokenizer_p.h"

/*
All language tags should be valid according to IETF BCP 47, as codified in RFC 4646.
ISO 639-1 codes should be used for the language part except for cases where there
exists no code, then 639-3 codes should be used. Country codes should only be used
in special cases. Scripts can be differentiated by IANA subtags, available here:
http://www.iana.org/assignments/language-subtag-registry
The script tags correspond to ISO 15924

An overview of the best practices concerning language tagging is available here:
http://www.w3.org/International/articles/language-tags/Overview.en.php

lang tags should use underscores (_) rather than hyphens (-) to separate subsections.

EXCEPTIONS:
For cases of known differences from the above tagging scheme and major
spellcheckers such aspell/hunspell/myspell, the scheme used by the spell checkers
shall be used. All exception shall be noted here:

BCP        SPELLCHECK
az-Latn    az

*/

namespace Sonnet {
class GuessLanguagePrivate {
   public:
    GuessLanguagePrivate();
    //            language       trigram  score
    static QHash<QString, QHash<QString, int>> s_knownModels;

    void loadModels();
    QList<QChar::Script> findRuns(const QString &text);
    QVector<QString> createOrderedModel(const QString &content);
    int distance(const QVector<QString> &model, const QHash<QString, int> &knownModel);
    QStringList guessFromTrigrams(const QString &sample, const QStringList &langs);
    QStringList identify(const QString &sample, const QList<QChar::Script> &scripts);
    QString guessFromDictionaries(const QString &sentence, const QStringList &candidates);

    static QSet<QString> s_knownDictionaries;
    static QMultiHash<QChar::Script, QString> s_scriptLanguages;
    static QMap<QString, QString> s_dictionaryNameMap;

    const int MIN_LENGTH;
    int m_maxItems;
    double m_minConfidence;
};

QHash<QString, QHash<QString, int>> GuessLanguagePrivate::s_knownModels;
QSet<QString> GuessLanguagePrivate::s_knownDictionaries;
QMultiHash<QChar::Script, QString> GuessLanguagePrivate::s_scriptLanguages;
QMap<QString, QString> GuessLanguagePrivate::s_dictionaryNameMap;

QStringList getNames(QLocale::Script script) {
    QStringList locales;
    const auto matchingLocales =
        QLocale::matchingLocales(QLocale::AnyLanguage, script, QLocale::AnyCountry);
    locales.reserve(matchingLocales.size());
    for (const QLocale &locale : matchingLocales) {
        locales << locale.name();
    }
    return locales;
}

GuessLanguagePrivate::GuessLanguagePrivate() : MIN_LENGTH(5), m_maxItems(1), m_minConfidence(0) {
    if (!s_scriptLanguages.isEmpty()) {
        return;
    }

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    s_knownDictionaries = Loader::openLoader()->languages().toSet();
#else
    const auto langs = Loader::openLoader()->languages();
    s_knownDictionaries = QSet<QString>(langs.cbegin(), langs.cend());
#endif
    QSet<QString> dictionaryLanguages;
#if QT_VERSION >= 0x050700
    for (const QString &dictName : qAsConst(s_knownDictionaries)) {
#else
    for (const QString &dictName : (s_knownDictionaries)) {
#endif
        QString languageName = QLocale(dictName).name();
        if (languageName.isEmpty()) {
            qWarning() << "Unable to parse name for dictionary" << dictName;
            continue;
        }
        dictionaryLanguages.insert(languageName);
    }

    QSet<QString> allLanguages;
    for (int i = 0; i < int(QChar::ScriptCount); i++) {
        QChar::Script script = static_cast<QChar::Script>(i);
        QStringList names;
        switch (script) {
            case QChar::Script_Latin:
                names = getNames(QLocale::LatinScript);
                break;
            case QChar::Script_Greek:
                names = getNames(QLocale::GreekScript);
                break;
            case QChar::Script_Cyrillic:
                names = getNames(QLocale::CyrillicScript);
                break;
            case QChar::Script_Armenian:
                names = getNames(QLocale::ArmenianScript);
                break;
            case QChar::Script_Hebrew:
                names = getNames(QLocale::HebrewScript);
                break;
            case QChar::Script_Arabic:
                names = getNames(QLocale::ArabicScript);
                break;
            case QChar::Script_Syriac:
                names = getNames(QLocale::SyriacScript);
                break;
            case QChar::Script_Thaana:
                names = getNames(QLocale::ThaanaScript);
                break;
            case QChar::Script_Devanagari:
                names = getNames(QLocale::DevanagariScript);
                break;
            case QChar::Script_Bengali:
                names = getNames(QLocale::BengaliScript);
                break;
            case QChar::Script_Gurmukhi:
                names = getNames(QLocale::GurmukhiScript);
                break;
            case QChar::Script_Gujarati:
                names = getNames(QLocale::GujaratiScript);
                break;
            case QChar::Script_Oriya:
                names = getNames(QLocale::OriyaScript);
                break;
            case QChar::Script_Tamil:
                names = getNames(QLocale::TamilScript);
                break;
            case QChar::Script_Telugu:
                names = getNames(QLocale::TeluguScript);
                break;
            case QChar::Script_Kannada:
                names = getNames(QLocale::KannadaScript);
                break;
            case QChar::Script_Malayalam:
                names = getNames(QLocale::MalayalamScript);
                break;
            case QChar::Script_Sinhala:
                names = getNames(QLocale::SinhalaScript);
                break;
            case QChar::Script_Thai:
                names = getNames(QLocale::ThaiScript);
                break;
            case QChar::Script_Lao:
                names = getNames(QLocale::LaoScript);
                break;
            case QChar::Script_Tibetan:
                names = getNames(QLocale::TibetanScript);
                break;
            case QChar::Script_Myanmar:
                names = getNames(QLocale::MyanmarScript);
                break;
            case QChar::Script_Georgian:
                names = getNames(QLocale::GeorgianScript);
                break;
            case QChar::Script_Hangul:
                names = getNames(QLocale::HangulScript);
                break;
            case QChar::Script_Ethiopic:
                names = getNames(QLocale::EthiopicScript);
                break;
            case QChar::Script_Cherokee:
                names = getNames(QLocale::CherokeeScript);
                break;
            case QChar::Script_CanadianAboriginal:
                names = getNames(QLocale::CanadianAboriginalScript);
                break;
            case QChar::Script_Ogham:
                names = getNames(QLocale::OghamScript);
                break;
            case QChar::Script_Runic:
                names = getNames(QLocale::RunicScript);
                break;
            case QChar::Script_Khmer:
                names = getNames(QLocale::KhmerScript);
                break;
            case QChar::Script_Mongolian:
                names = getNames(QLocale::MongolianScript);
                break;
            case QChar::Script_Hiragana:
                names = getNames(QLocale::HiraganaScript);
                break;
            case QChar::Script_Katakana:
                names = getNames(QLocale::KatakanaScript);
                break;
            case QChar::Script_Bopomofo:
                names = getNames(QLocale::BopomofoScript);
                break;
            case QChar::Script_Han:
                names = getNames(QLocale::HanScript);
                break;
            case QChar::Script_Yi:
                names = getNames(QLocale::YiScript);
                break;
            case QChar::Script_OldItalic:
                names = getNames(QLocale::OldItalicScript);
                break;
            case QChar::Script_Gothic:
                names = getNames(QLocale::GothicScript);
                break;
            case QChar::Script_Deseret:
                names = getNames(QLocale::DeseretScript);
                break;
            case QChar::Script_Tagalog:
                names = getNames(QLocale::TagalogScript);
                break;
            case QChar::Script_Hanunoo:
                names = getNames(QLocale::HanunooScript);
                break;
            case QChar::Script_Buhid:
                names = getNames(QLocale::BuhidScript);
                break;
            case QChar::Script_Tagbanwa:
                names = getNames(QLocale::TagbanwaScript);
                break;
            case QChar::Script_Coptic:
                names = getNames(QLocale::CopticScript);
                break;
            case QChar::Script_Limbu:
                names = getNames(QLocale::LimbuScript);
                break;
            case QChar::Script_TaiLe:
                names = getNames(QLocale::TaiLeScript);
                break;
            case QChar::Script_LinearB:
                names = getNames(QLocale::LinearBScript);
                break;
            case QChar::Script_Ugaritic:
                names = getNames(QLocale::UgariticScript);
                break;
            case QChar::Script_Shavian:
                names = getNames(QLocale::ShavianScript);
                break;
            case QChar::Script_Osmanya:
                names = getNames(QLocale::OsmanyaScript);
                break;
            case QChar::Script_Cypriot:
                names = getNames(QLocale::CypriotScript);
                break;
            case QChar::Script_Braille:
                names = getNames(QLocale::BrailleScript);
                break;
            case QChar::Script_Buginese:
                names = getNames(QLocale::BugineseScript);
                break;
            case QChar::Script_NewTaiLue:
                names = getNames(QLocale::NewTaiLueScript);
                break;
            case QChar::Script_Glagolitic:
                names = getNames(QLocale::GlagoliticScript);
                break;
            case QChar::Script_Tifinagh:
                names = getNames(QLocale::TifinaghScript);
                break;
            case QChar::Script_SylotiNagri:
                names = getNames(QLocale::SylotiNagriScript);
                break;
            case QChar::Script_OldPersian:
                names = getNames(QLocale::OldPersianScript);
                break;
            case QChar::Script_Kharoshthi:
                names = getNames(QLocale::KharoshthiScript);
                break;
            case QChar::Script_Balinese:
                names = getNames(QLocale::BalineseScript);
                break;
            case QChar::Script_Cuneiform:
                names = getNames(QLocale::CuneiformScript);
                break;
            case QChar::Script_Phoenician:
                names = getNames(QLocale::PhoenicianScript);
                break;
            case QChar::Script_PhagsPa:
                names = getNames(QLocale::PhagsPaScript);
                break;
            case QChar::Script_Nko:
                names = getNames(QLocale::NkoScript);
                break;
            case QChar::Script_Sundanese:
                names = getNames(QLocale::SundaneseScript);
                break;
            case QChar::Script_Lepcha:
                names = getNames(QLocale::LepchaScript);
                break;
            case QChar::Script_OlChiki:
                names = getNames(QLocale::OlChikiScript);
                break;
            case QChar::Script_Vai:
                names = getNames(QLocale::VaiScript);
                break;
            case QChar::Script_Saurashtra:
                names = getNames(QLocale::SaurashtraScript);
                break;
            case QChar::Script_KayahLi:
                names = getNames(QLocale::KayahLiScript);
                break;
            case QChar::Script_Rejang:
                names = getNames(QLocale::RejangScript);
                break;
            case QChar::Script_Lycian:
                names = getNames(QLocale::LycianScript);
                break;
            case QChar::Script_Carian:
                names = getNames(QLocale::CarianScript);
                break;
            case QChar::Script_Lydian:
                names = getNames(QLocale::LydianScript);
                break;
            case QChar::Script_Cham:
                names = getNames(QLocale::ChamScript);
                break;
            case QChar::Script_TaiTham:
                names = getNames(QLocale::LannaScript);
                break;
            case QChar::Script_TaiViet:
                names = getNames(QLocale::TaiVietScript);
                break;
            case QChar::Script_Avestan:
                names = getNames(QLocale::AvestanScript);
                break;
            case QChar::Script_EgyptianHieroglyphs:
                names = getNames(QLocale::EgyptianHieroglyphsScript);
                break;
            case QChar::Script_Samaritan:
                names = getNames(QLocale::SamaritanScript);
                break;
            case QChar::Script_Lisu:
                names = getNames(QLocale::FraserScript);
                break;
            case QChar::Script_Bamum:
                names = getNames(QLocale::BamumScript);
                break;
            case QChar::Script_Javanese:
                names = getNames(QLocale::JavaneseScript);
                break;
            case QChar::Script_MeeteiMayek:
                names = getNames(QLocale::MeiteiMayekScript);
                break;
            case QChar::Script_ImperialAramaic:
                names = getNames(QLocale::ImperialAramaicScript);
                break;
            case QChar::Script_OldSouthArabian:
                names = getNames(QLocale::OldSouthArabianScript);
                break;
            case QChar::Script_InscriptionalParthian:
                names = getNames(QLocale::InscriptionalParthianScript);
                break;
            case QChar::Script_InscriptionalPahlavi:
                names = getNames(QLocale::InscriptionalPahlaviScript);
                break;
            case QChar::Script_Kaithi:
                names = getNames(QLocale::KaithiScript);
                break;
            case QChar::Script_Batak:
                names = getNames(QLocale::BatakScript);
                break;
            case QChar::Script_Brahmi:
                names = getNames(QLocale::BrahmiScript);
                break;
            case QChar::Script_Mandaic:
                names = getNames(QLocale::MandaeanScript);
                break;
            case QChar::Script_Chakma:
                names = getNames(QLocale::ChakmaScript);
                break;
            case QChar::Script_MeroiticCursive:
            case QChar::Script_MeroiticHieroglyphs:
                names = getNames(QLocale::MeroiticCursiveScript);
                names.append(getNames(QLocale::MeroiticScript));
                break;
            case QChar::Script_Miao:
                names = getNames(QLocale::PollardPhoneticScript);
                break;
            case QChar::Script_Sharada:
                names = getNames(QLocale::SharadaScript);
                break;
            case QChar::Script_SoraSompeng:
                names = getNames(QLocale::SoraSompengScript);
                break;
            case QChar::Script_Takri:
                names = getNames(QLocale::TakriScript);
                break;
#if QT_VERSION >= 0x050500
            case QChar::Script_CaucasianAlbanian:
                names = getNames(QLocale::CaucasianAlbanianScript);
                break;
            case QChar::Script_BassaVah:
                names = getNames(QLocale::BassaVahScript);
                break;
            case QChar::Script_Duployan:
                names = getNames(QLocale::DuployanScript);
                break;
            case QChar::Script_Elbasan:
                names = getNames(QLocale::ElbasanScript);
                break;
            case QChar::Script_Grantha:
                names = getNames(QLocale::GranthaScript);
                break;
            case QChar::Script_PahawhHmong:
                names = getNames(QLocale::PahawhHmongScript);
                break;
            case QChar::Script_Khojki:
                names = getNames(QLocale::KhojkiScript);
                break;
            case QChar::Script_LinearA:
                names = getNames(QLocale::LinearAScript);
                break;
            case QChar::Script_Mahajani:
                names = getNames(QLocale::MahajaniScript);
                break;
            case QChar::Script_Manichaean:
                names = getNames(QLocale::ManichaeanScript);
                break;
            case QChar::Script_MendeKikakui:
                names = getNames(QLocale::MendeKikakuiScript);
                break;
            case QChar::Script_Modi:
                names = getNames(QLocale::ModiScript);
                break;
            case QChar::Script_Mro:
                names = getNames(QLocale::MroScript);
                break;
            case QChar::Script_OldNorthArabian:
                names = getNames(QLocale::OldNorthArabianScript);
                break;
            case QChar::Script_Nabataean:
                names = getNames(QLocale::NabataeanScript);
                break;
            case QChar::Script_Palmyrene:
                names = getNames(QLocale::PalmyreneScript);
                break;
            case QChar::Script_PauCinHau:
                names = getNames(QLocale::PauCinHauScript);
                break;
            case QChar::Script_OldPermic:
                names = getNames(QLocale::OldPermicScript);
                break;
            case QChar::Script_PsalterPahlavi:
                names = getNames(QLocale::PsalterPahlaviScript);
                break;
            case QChar::Script_Siddham:
                names = getNames(QLocale::SiddhamScript);
                break;
            case QChar::Script_Khudawadi:
                names = getNames(QLocale::KhudawadiScript);
                break;
            case QChar::Script_Tirhuta:
                names = getNames(QLocale::TirhutaScript);
                break;
            case QChar::Script_WarangCiti:
                names = getNames(QLocale::VarangKshitiScript);
                break;
#endif
#if QT_VERSION >= 0x050700
            case QChar::Script_Ahom:
                names = getNames(QLocale::AhomScript);
                break;
            case QChar::Script_AnatolianHieroglyphs:
                names = getNames(QLocale::AnatolianHieroglyphsScript);
                break;

            case QChar::Script_Hatran:
                names = getNames(QLocale::HatranScript);
                break;

            case QChar::Script_Multani:
                names = getNames(QLocale::MultaniScript);
                break;
            case QChar::Script_OldHungarian:
                names = getNames(QLocale::OldHungarianScript);
                break;

            case QChar::Script_Unknown:
            case QChar::Script_Inherited:
            case QChar::Script_Common:
            case QChar::Script_OldTurkic:
            case QChar::Script_SignWriting:
#endif
                break;
            default:
#ifdef SONNET_DEBUGGING_ON
                qCDebug(SONNET_LOG_CORE) << "Unhandled script" << script;
#endif
                break;
        }
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        allLanguages.unite(names.toSet());
#else
        allLanguages.unite(QSet<QString>(names.cbegin(), names.cend()));
#endif
        {    // Remove unknown languages
            QStringList pruned;
#if QT_VERSION >= 0x050700
            for (const QString &name : qAsConst(names)) {
#else
            for (const QString &name : (names)) {
#endif
                if (!dictionaryLanguages.contains(name)) {
                    continue;
                }
                pruned.append(name);
            }
            names = pruned;
        }

        if (names.isEmpty()) {
            continue;
        }
#if QT_VERSION >= 0x050700
        for (const QString &name : qAsConst(names)) {
#else
        for (const QString &name : (names)) {
#endif
            s_scriptLanguages.insert(script, name);
        }
    }

    // Try to handle some badly named dictionaries
    if (!allLanguages.contains(s_knownDictionaries)) {
        QSet<QString> dicts(s_knownDictionaries);
        dicts.subtract(allLanguages);
        for (const QString &dictName : dicts) {
            QString languageName = QLocale(dictName).name();
            if (languageName.isEmpty()) {
                qWarning() << "Unable to parse language name" << dictName;
                continue;
            }
            s_dictionaryNameMap[languageName] = dictName;
            if (!s_scriptLanguages.values().contains(languageName)) {
                qWarning() << "Unable to handle language from dictionary" << dictName
                           << languageName;
            }
        }
    }
}

GuessLanguage::GuessLanguage() : d(new GuessLanguagePrivate) {}

GuessLanguage::~GuessLanguage() { delete d; }

QString GuessLanguage::identify(const QString &text, const QStringList &suggestionsListIn) const {
    if (text.isEmpty()) {
        return QString();
    }

    // Filter for available dictionaries
    QStringList suggestionsList;
    for (const QString &suggestion : suggestionsListIn) {
        if (d->s_knownDictionaries.contains(suggestion) && !suggestionsList.contains(suggestion)) {
            suggestionsList.append(suggestion);
        }
    }

    // Load the model on demand
    if (d->s_knownModels.isEmpty()) {
        d->loadModels();
    }

    QStringList candidateLanguages = d->identify(text, d->findRuns(text));

    // Hack for some bad dictionary names
    for (int i = 0; i < candidateLanguages.count(); i++) {
        if (d->s_dictionaryNameMap.contains(candidateLanguages[i])) {
            candidateLanguages[i] = d->s_dictionaryNameMap.value(candidateLanguages[i]);
        }
    }

    if (candidateLanguages.count() == 1) {
        return candidateLanguages.first();
    }

    // Wasn't able to get a good guess with the trigrams, try checking all
    // dictionaries for the suggested languages.
    candidateLanguages.append(suggestionsList);
    candidateLanguages.removeDuplicates();
    QString identified = d->guessFromDictionaries(text, candidateLanguages);
    if (!identified.isEmpty()) {
        return identified;
    }

    //    qCDebug(SONNET_LOG_CORE()) << "Unable to identify string with dictionaries:" << text;

    // None of our methods worked, just return the best suggestion
    if (!suggestionsList.isEmpty()) {
        return suggestionsList.first();
    }

    //    qCDebug(SONNET_LOG_CORE) << "Unable to find any suggestion for" << text;

    // Not even any suggestions, give up
    return QString();
}

void GuessLanguage::setLimits(int maxItems, double minConfidence) {
    d->m_maxItems = maxItems;
    d->m_minConfidence = minConfidence;
}

void GuessLanguagePrivate::loadModels() {
    QString triMapFile;

    triMapFile = QStringLiteral(":/libraries/sonnet/src/trigrams.map");
    qDebug() << "Loading trigrams from" << triMapFile;

    QFile sin(triMapFile);
    if (!sin.open(QIODevice::ReadOnly)) {
        qWarning() << "Sonnet: Unable to load trigram models from file" << triMapFile;
        return;
    }

    QDataStream in(&sin);
    in >> s_knownModels;

    // Sanity check
    QSet<QString> availableLanguages;
    QHashIterator<QString, QHash<QString, int>> iterator(s_knownModels);
    while (iterator.hasNext()) {
        iterator.next();
        if (iterator.value().count() < MAXGRAMS) {
            qWarning() << iterator.key() << "is has only" << iterator.value().count()
                       << "trigrams, expected" << MAXGRAMS;
        }
        availableLanguages.insert(iterator.key());
    }
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    QSet<QString> knownLanguages(s_scriptLanguages.values().toSet());
#else
    QSet<QString> knownLanguages(s_scriptLanguages.cbegin(), s_scriptLanguages.cend());
#endif
    knownLanguages.subtract(availableLanguages);
    if (!knownLanguages.isEmpty()) {
        qDebug() << "Missing trigrams for languages:" << knownLanguages;
    }
}

QList<QChar::Script> GuessLanguagePrivate::findRuns(const QString &text) {
    QChar::Script script = QChar::Script_Unknown;
    QHash<QChar::Script, int> scriptCounts;

    int totalCount = 0;

    for (const QChar c : text) {
        script = c.script();

        if (script == QChar::Script_Common || script == QChar::Script_Inherited) {
            continue;
        }

        if (!c.isLetter()) {
            continue;
        }

        scriptCounts[script]++;
        totalCount++;
    }

    QList<QChar::Script> relevantScripts;

    if (totalCount == 0) {
        return relevantScripts;
    }

    for (const QChar::Script script : scriptCounts.keys()) {
        // return run types that used for 40% or more of the string
        int scriptCount = scriptCounts[script];
        if (scriptCount * 100 / totalCount >= 40) {
            relevantScripts << script;
            // always return basic latin if found more than 15%.
        } else if (script == QChar::Script_Latin && scriptCount * 100 / totalCount >= 15) {
            relevantScripts << script;
        }
    }

    return relevantScripts;
}

QStringList GuessLanguagePrivate::identify(const QString &sample,
                                           const QList<QChar::Script> &scripts) {
    if (sample.size() < MIN_LENGTH) {
        return QStringList();
    }

    QStringList guesses;
    for (const QChar::Script script : scripts) {
        guesses.append(guessFromTrigrams(sample, s_scriptLanguages.values(script)));
    }

    // if guesses are empty, we just append the languages of the scripts
    if (guesses.isEmpty() && !scripts.isEmpty()) {
        for (const QChar::Script script : scripts) {
            guesses.append(s_scriptLanguages.values(script));
        }
    }

    return guesses;
}

QStringList GuessLanguagePrivate::guessFromTrigrams(const QString &sample,
                                                    const QStringList &languages) {
    QStringList ret;

    const QVector<QString> sampleTrigrams = createOrderedModel(sample);

    // Sort by score
    QVector<QPair<int, QString>> scores;
    scores.reserve(s_knownModels.size());
    for (const QString &language : languages) {
        const auto model = s_knownModels.value(language, {});
        if (!model.isEmpty()) {
            scores.append({distance(sampleTrigrams, model), language});
        }
    }

    std::sort(scores.begin(), scores.end());

    // Skip if either no results or best result is completely unknown (distance >= maxdistance)
    if (scores.isEmpty() || scores.first().first >= MAXGRAMS * sampleTrigrams.size()) {
        //         qCDebug(SONNET_LOG_CORE) << "No scores for" << sample;
        return ret;
    }

    int counter = 0;
    double confidence = 0;

    QVectorIterator<QPair<int, QString>> it(scores);
    const auto el = it.next();

    QString prevItem = el.second;
    int prevScore = el.first;
    ret.reserve(scores.size());

    while (it.hasNext() && counter < m_maxItems && confidence < m_minConfidence) {
        auto el = it.next();
        counter++;
        confidence += (el.first - prevScore) / (double)el.first;
        ret += prevItem;
        prevItem = el.second;
        prevScore = el.first;
    }
    if (counter < m_maxItems && confidence < m_minConfidence) {
        ret += prevItem;
    }

    return ret;
}

QVector<QString> GuessLanguagePrivate::createOrderedModel(const QString &content) {
    QHash<QString, int> trigramCounts;

    // collect trigrams
    trigramCounts.reserve(content.size() - 2);
    for (int i = 0; i < (content.size() - 2); ++i) {
        QString tri = content.mid(i, 3).toLower();
        trigramCounts[tri]++;
    }

    // invert the map <freq, trigram>
    QVector<QPair<int, QString>> temp;
    temp.reserve(trigramCounts.size());

    auto it = trigramCounts.constBegin();
    for (; it != trigramCounts.constEnd(); ++it) {
        const QChar *data = it.key().constData();
        bool hasTwoSpaces = (data[1].isSpace() && (data[0].isSpace() || data[2].isSpace()));

        if (!hasTwoSpaces) {
            const int freq = it.value();
            const QString &trigram = it.key();
            temp.append({freq, trigram});
        }
    }

    auto i = std::partition(temp.begin(), temp.end(),
                            [](const QPair<int, QString> &a) { return a.first != 1; });

    std::sort(temp.begin(), i, [](const QPair<int, QString> &a, const QPair<int, QString> &b) {
        return a.first > b.first;
    });

    QVector<QString> orderedTrigrams;
    orderedTrigrams.reserve(temp.size());
    const auto &constOrd = temp;
    for (const auto &tri : constOrd) {
        orderedTrigrams.append(tri.second);
    }

    return orderedTrigrams;
}

int GuessLanguagePrivate::distance(const QVector<QString> &model,
                                   const QHash<QString, int> &knownModel) {
    int counter = -1;
    int dist = 0;

    for (const QString &trigram : model) {
        int val = knownModel.value(trigram, -1);
        if (val != -1) {
            dist += qAbs(++counter - val);
        } else {
            dist += MAXGRAMS;
        }

        if (counter == (MAXGRAMS - 1)) {
            break;
        }
    }

    return dist;
}

QString GuessLanguagePrivate::guessFromDictionaries(const QString &sentence,
                                                    const QStringList &candidates) {
    // Try to see how many languages we can get spell checking for
    QList<QSharedPointer<SpellerPlugin>> spellers;
    for (const QString &lang : candidates) {
        if (!Loader::openLoader()->languages().contains(lang)) {
            qWarning() << "Dictionary asked for invalid speller" << lang;
            continue;
        }
        QSharedPointer<SpellerPlugin> plugin = Loader::openLoader()->cachedSpeller(lang);
        if (!plugin.isNull()) {
            spellers.append(plugin);
        }
    }

    // If there's no spell checkers, give up
    if (spellers.isEmpty()) {
        return QString();
    }

    QMap<QString, int> correctHits;

    WordTokenizer tokenizer(sentence);
    int maxCorrect = tokenizer.count() % 2 == 0 ? tokenizer.count() / 2 : tokenizer.count() / 2 + 1;
    int count = 0;
    for (int i = 0; i < spellers.count(); ++i) {
        const auto &speller = spellers[i];
        const QString language = speller->language();
        while (tokenizer.hasNext()) {
            Token word = tokenizer.next();
            if (!tokenizer.isSpellcheckable()) {
                continue;
            }

            if (speller->isCorrect(word.toString())) {
                correctHits[language]++;
                count++;
                if (count >= maxCorrect) break;
            }
        }
        if (count >= maxCorrect) break;
        count = 0;
        tokenizer.reset();
    }

    if (correctHits.isEmpty()) {
        return QString();
    }

    QMap<QString, int>::const_iterator max = correctHits.constBegin();
    for (QMap<QString, int>::const_iterator itr = correctHits.constBegin();
         itr != correctHits.constEnd(); ++itr) {
        if (itr.value() > max.value()) {
            max = itr;
        }
    }
    return max.key();
}
}    // namespace Sonnet
