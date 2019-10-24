/**
 * voikkodict.cpp
 *
 * Copyright (C)  2015  Jesse Jaara <jesse.jaara@gmail.com>
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

#include "voikkodict.h"
#include "voikkodebug.h"

#include <QDir>
#include <QStandardPaths>
#include <QVector>
#ifdef Q_IS_WIN
    #include <QSysInfo>
#endif

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace {
// QString literals used in loading and storing user dictionary
inline const QString replacement_bad_str() Q_DECL_NOEXCEPT
{
    return QStringLiteral("bad");
}

inline const QString replacement_good_str() Q_DECL_NOEXCEPT
{
    return QStringLiteral("good");
}

inline const QString personal_words_str() Q_DECL_NOEXCEPT
{
    return QStringLiteral("PersonalWords");
}

inline const QString replacements_str() Q_DECL_NOEXCEPT
{
    return QStringLiteral("Replacements");
}

// Set path to: QStandardPaths::GenericDataLocation/Sonnet/Voikko-user-dictionary.json
QString getUserDictionaryPath() Q_DECL_NOEXCEPT
{
    QString directory = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

        #ifdef Q_OS_WIN
    // Resolve the windows' Roaming directory manually
    directory = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    if (QSysInfo::windowsVersion() == QSysInfo::WV_XP || QSysInfo::windowsVersion() == QSysInfo::WV_2003) {
        // In Xp Roaming is "<user>/Application Data"
        // DataLocation: "<user>/Local Settings/Application Data"
        directory += QStringLiteral("/../../Application Data");
    } else {
        directory += QStringLiteral("/../Roaming");
    }
        #endif

    directory += QStringLiteral("/Sonnet");
    QDir path(directory);
    path.mkpath(path.absolutePath());

    return path.absoluteFilePath(QStringLiteral("Voikko-user-dictionary.json"));
}

void addReplacementToNode(QJsonObject &languageNode, const QString &bad,
                          const QString &good) Q_DECL_NOEXCEPT
{
    QJsonObject pair;
    pair[replacement_bad_str()] = good;
    pair[replacement_good_str()] = bad;

    auto replaceList = languageNode[replacements_str()].toArray();
    replaceList.append(pair);
    languageNode[replacements_str()] = replaceList;
}

void addPersonalWordToNode(QJsonObject &languageNode, const QString &word) Q_DECL_NOEXCEPT
{
    auto arr = languageNode[personal_words_str()].toArray();
    arr.append(word);
    languageNode[personal_words_str()] = arr;
}

/**
 * Read and return the root json object from fileName.
 *
 * Returns an empty node in case of an IO error or the file is empty.
 */
QJsonObject readJsonRootObject(const QString &fileName) Q_DECL_NOEXCEPT
{
    QFile userDictFile(fileName);

    if (!userDictFile.exists()) {
        return QJsonObject();     // Nothing has been saved so far.
    }

    if (!userDictFile.open(QIODevice::ReadOnly)) {
        qCWarning(SONNET_VOIKKO) << "Could not open personal dictionary. Failed to open file"
                                 << fileName;
        qCWarning(SONNET_VOIKKO) << "Reason:" << userDictFile.errorString();
        return QJsonObject();
    }

    QJsonDocument dictDoc = QJsonDocument::fromJson(userDictFile.readAll());
    userDictFile.close();

    return dictDoc.object();
}
}

class VoikkoDictPrivate
{
public:
    VoikkoHandle *m_handle;
    const VoikkoDict *q;

    QSet<QString> m_sessionWords;
    QSet<QString> m_personalWords;
    QHash<QString, QString> m_replacements;

    QString m_userDictionaryFilepath;

    // Used when converting Qstring to wchar_t strings
    QVector<wchar_t> m_conversionBuffer;

    VoikkoDictPrivate(const QString &language, const VoikkoDict *publicPart) Q_DECL_NOEXCEPT
        : q(publicPart)
        , m_userDictionaryFilepath(getUserDictionaryPath())
        , m_conversionBuffer(256)
    {
        const char *error;
        m_handle = voikkoInit(&error, language.toUtf8().data(), nullptr);

        if (error != nullptr) {
            qCWarning(SONNET_VOIKKO) << "Failed to initialize Voikko spelling backend. Reason:"
                                     << error;
        } else { // Continue to load user's own words
            loadUserDictionary();
        }
    }

    /**
     * Store a new ignored/personal word or replacement pair in the user's
     * dictionary m_userDictionaryFilepath.
     *
     * returns true on success else false
     */
    bool storePersonal(const QString &personalWord,
                       const QString &bad = QString(),
                       const QString &good = QString()) const Q_DECL_NOEXCEPT
    {
        QFile userDictFile(m_userDictionaryFilepath);

        if (!userDictFile.open(QIODevice::ReadWrite)) {
            qCWarning(SONNET_VOIKKO) << "Could not save personal dictionary. Failed to open file:"
                                     << m_userDictionaryFilepath;
            qCWarning(SONNET_VOIKKO) << "Reason:" << userDictFile.errorString();
            return false;
        }

        QJsonDocument dictDoc = QJsonDocument::fromJson(userDictFile.readAll());
        auto root = readJsonRootObject(m_userDictionaryFilepath);
        auto languageNode = root[q->language()].toObject();

        // Empty value means we are storing a bad:good pair
        if (personalWord.isEmpty()) {
            addReplacementToNode(languageNode, bad, good);
        } else {
            addPersonalWordToNode(languageNode, personalWord);
        }

        root[q->language()] = languageNode;
        dictDoc.setObject(root);

        userDictFile.reset();
        userDictFile.write(dictDoc.toJson());
        userDictFile.close();
        qCDebug(SONNET_VOIKKO) << "Changes to user dictionary saved to file: "
                               << m_userDictionaryFilepath;

        return true;
    }

    /**
     * Load user's own personal words and replacement pairs from
     * m_userDictionaryFilepath.
     */
    void loadUserDictionary() Q_DECL_NOEXCEPT
    {
        // If root is empty we will fail later on when checking if
        // languageNode is empty.
        auto root = readJsonRootObject(m_userDictionaryFilepath);
        auto languageNode = root[q->language()].toObject();

        if (languageNode.isEmpty()) {
            return; // Nothing to load
        }

        loadUserWords(languageNode);
        loadUserReplacements(languageNode);
    }

    /**
     * Convert the given QString to a \0 terminated wchar_t string.
     * Uses QVector as a buffer and return it's internal data pointer.
     */
    inline const wchar_t *QStringToWchar(const QString &str) Q_DECL_NOEXCEPT
    {
        m_conversionBuffer.resize(str.length() + 1);
        int size = str.toWCharArray(m_conversionBuffer.data());
        m_conversionBuffer[size] = '\0';

        return m_conversionBuffer.constData();
    }

private:
    /**
     * Extract and append user defined words from the languageNode.
     */
    inline void loadUserWords(const QJsonObject &languageNode) Q_DECL_NOEXCEPT
    {
        const auto words = languageNode[personal_words_str()].toArray();
        for (auto word : words) {
            m_personalWords.insert(word.toString());
        }
        qCDebug(SONNET_VOIKKO)
            << QStringLiteral("Loaded %1 words from the user dictionary.").arg(words.size());
    }

    /**
     * Extract and append user defined replacement pairs from the languageNode.
     */
    inline void loadUserReplacements(const QJsonObject &languageNode) Q_DECL_NOEXCEPT
    {
        const auto words = languageNode[replacements_str()].toArray();
        for (auto pair : words) {
            m_replacements[pair.toObject()[replacement_bad_str()].toString()]
                = pair.toObject()[replacement_good_str()].toString();
        }
        qCDebug(SONNET_VOIKKO)
            << QStringLiteral("Loaded %1 replacements from the user dictionary.").arg(words.size());
    }
};

VoikkoDict::VoikkoDict(const QString &language) Q_DECL_NOEXCEPT
    : SpellerPlugin(language)
    , d(new VoikkoDictPrivate(language, this))
{
    qCDebug(SONNET_VOIKKO) << "Loading dictionary for language:" << language;
}

VoikkoDict::~VoikkoDict()
{
}

bool VoikkoDict::isCorrect(const QString &word) const
{
    // Check the session word list and personal word list first
    if (d->m_sessionWords.contains(word) || d->m_personalWords.contains(word)) {
        return true;
    }

    return voikkoSpellUcs4(d->m_handle, d->QStringToWchar(word)) == VOIKKO_SPELL_OK;
}

QStringList VoikkoDict::suggest(const QString &word) const
{
    QStringList suggestions;

    auto userDictPos = d->m_replacements.constFind(word);
    if (userDictPos != d->m_replacements.constEnd()) {
        suggestions.append(*userDictPos);
    }

    auto voikkoSuggestions = voikkoSuggestUcs4(d->m_handle, d->QStringToWchar(word));

    if (!voikkoSuggestions) {
        return suggestions;
    }

    for (int i = 0; voikkoSuggestions[i] != nullptr; ++i) {
        QString suggestion = QString::fromWCharArray(voikkoSuggestions[i]);
        suggestions.append(suggestion);
    }
    qCDebug(SONNET_VOIKKO) << "Misspelled:" << word
                           << "|Suggestons:" << suggestions.join(QLatin1String(", "));

    voikko_free_suggest_ucs4(voikkoSuggestions);

    return suggestions;
}

bool VoikkoDict::storeReplacement(const QString &bad, const QString &good)
{
    qCDebug(SONNET_VOIKKO) << "Adding new replacement pair to user dictionary:"
                           << bad << "->" << good;
    d->m_replacements[bad] = good;
    return d->storePersonal(QString(), bad, good);
}

bool VoikkoDict::addToPersonal(const QString &word)
{
    qCDebug(SONNET_VOIKKO()) << "Adding new word to user dictionary" << word;
    d->m_personalWords.insert(word);
    return d->storePersonal(word);
}

bool VoikkoDict::addToSession(const QString &word)
{
    qCDebug(SONNET_VOIKKO()) << "Adding new word to session dictionary" << word;
    d->m_sessionWords.insert(word);
    return true;
}

bool VoikkoDict::initFailed() const Q_DECL_NOEXCEPT
{
    return !d->m_handle;
}
