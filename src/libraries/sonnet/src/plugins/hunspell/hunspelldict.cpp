/**
 * kspell_hunspelldict.cpp
 *
 * Copyright (C)  2009  Montel Laurent <montel@kde.org>
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

#include "hunspelldict.h"

//#include "config-hunspell.h"
#include "hunspelldebug.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextCodec>
#include <QTextStream>
#include <QStandardPaths>
#include <utils/misc.h>
#include <QStringBuilder>
#include <QtConcurrent/QtConcurrent>
#include <thread>

using namespace Sonnet;

HunspellDict::HunspellDict(const QString &lang, QString path)
    : SpellerPlugin(lang)
{
#ifdef HUNSPELL_DEBUG_ON
    qCDebug(SONNET_HUNSPELL) << "Loading dictionary for" << lang << "from" << path;
#endif
    if (!path.endsWith(QLatin1Char('/'))) {
        path += QLatin1Char('/');
    }
    path += lang;
    QString dictionary = path + QStringLiteral(".dic");
    QString aff = path + QStringLiteral(".aff");
    //QFuture<void> f;
    QtConcurrent::run([=](){ //load dictionaries asynchronously
        if (QFileInfo::exists(dictionary) && QFileInfo::exists(aff)) {

            m_speller
                    = new Hunspell(aff.toLocal8Bit().constData(), dictionary.toLocal8Bit().constData());
            m_codec = QTextCodec::codecForName(m_speller->get_dic_encoding());
            if (!m_codec) {
                qCWarning(SONNET_HUNSPELL) << "Failed to find a text codec for name"
                                           << m_speller->get_dic_encoding()
                                           << "defaulting to locale text codec";
                m_codec = QTextCodec::codecForLocale();
                Q_ASSERT(m_codec);
            }

        } else {
            qCWarning(SONNET_HUNSPELL) << "Unable to find dictionary for" << lang << "in path" << path;
        }
        QString userDic;
        if (Utils::Misc::isInPortableMode()) {
            userDic = Utils::Misc::portableDataPath() + QLatin1Char('/')
                    + QLatin1String(".hunspell_") % lang;
        } else {
            userDic = QDir::home().filePath(QLatin1String(".hunspell_") % lang);
        }

        QFile userDicFile(userDic);
        if (userDicFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
#ifdef HUNSPELL_DEBUG_ON
            qCDebug(SONNET_HUNSPELL) << "Load a user dictionary" << userDic;
#endif
            QTextStream userDicIn(&userDicFile);
            while (!userDicIn.atEnd()) {
                QString word = userDicIn.readLine();
                if (word.contains(QLatin1Char('/'))) {
                    QStringList wordParts = word.split(QLatin1Char('/'));
                    m_speller->add_with_affix(toDictEncoding(wordParts.at(
                                                                 0)).constData(),
                                              toDictEncoding(wordParts.at(1)).constData());
                }
                if (word.at(0) == QLatin1Char('*')) {
                    m_speller->remove(toDictEncoding(word.mid(1)).constData());
                } else {
                    m_speller->add(toDictEncoding(word).constData());
                }
            }
            userDicFile.close();
        }

#ifdef HUNSPELL_DEBUG_ON
        qCDebug(SONNET_HUNSPELL) << "Created " << m_speller;
#endif
}); //QtConcurrent::Run end
}

HunspellDict::~HunspellDict()
{
    delete m_speller;
}

QByteArray HunspellDict::toDictEncoding(const QString &word) const
{
    if (m_codec) {
        return m_codec->fromUnicode(word);
    }
    return {};
}

bool HunspellDict::isCorrect(const QString &word) const
{
   // qCDebug(SONNET_HUNSPELL) << " isCorrect :" << word;
    if (!m_speller) {
        return false;
    }

#if USE_OLD_HUNSPELL_API
    int result = m_speller->spell(toDictEncoding(word).constData());
    qCDebug(SONNET_HUNSPELL) << " result :" << result;
    return result != 0;
#else

#if QT_VERSION >= 0x050400
    bool result = m_speller->spell(toDictEncoding(word).toStdString());

#else
    bool result = m_speller->spell(QString(toDictEncoding(word)).toStdString());
#endif
   // qCDebug(SONNET_HUNSPELL) << " result :" << result;
    return result;
#endif
}

QStringList HunspellDict::suggest(const QString &word) const
{
    if (!m_speller) {
        return QStringList();
    }

    QStringList lst;
#if USE_OLD_HUNSPELL_API
    char **selection;
    int nbWord = m_speller->suggest(&selection, toDictEncoding(word).constData());
    for (int i = 0; i < nbWord; ++i) {
        lst << m_codec->toUnicode(selection[i]);
    }
    m_speller->free_list(&selection, nbWord);
#else
#if QT_VERSION >= 0x050400
    const auto suggestions = m_speller->suggest(toDictEncoding(word).toStdString());
#else
    const auto suggestions = m_speller->suggest(QString(toDictEncoding(word)).toStdString());
#endif
    std::for_each (suggestions.begin(), suggestions.end(), [this, &lst](const std::string &suggestion) {
            lst << m_codec->toUnicode(suggestion.c_str()); });
#endif

    return lst;
}

bool HunspellDict::storeReplacement(const QString &bad, const QString &good)
{
    Q_UNUSED(bad)
    Q_UNUSED(good)
    if (!m_speller) {
        return false;
    }
#ifdef HUNSPELL_DEBUG_ON
    qCDebug(SONNET_HUNSPELL) << "HunspellDict::storeReplacement not implemented";
#endif
    return false;
}

bool HunspellDict::addToPersonal(const QString &word)
{
    if (!m_speller) {
        return false;
    }
    m_speller->add(toDictEncoding(word).constData());
    //QString userDic = QDir::home().filePath(QLatin1String(".hunspell_"));
    //QString userDic = QDir::home().filePath(QLatin1String(".hunspell_") % language());
    QString userDic;
    if (Utils::Misc::isInPortableMode()) {
        userDic = Utils::Misc::portableDataPath() + QLatin1Char('/')
                           + QLatin1String(".hunspell_") % language();
    } else {
        userDic = QDir::home().filePath(QLatin1String(".hunspell_") % language());
    }
    QFile userDicFile(userDic);
    if (userDicFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&userDicFile);
        out << word << '\n';
        userDicFile.close();
        return true;
    }

    return false;
}

bool HunspellDict::addToSession(const QString &word)
{
    if (!m_speller) {
        return false;
    }
    int r = m_speller->add(toDictEncoding(word).constData());
    return r == 0;
}
