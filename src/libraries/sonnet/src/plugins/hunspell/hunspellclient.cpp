/**
 * kspell_hunspellclient.cpp
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
#include "hunspellclient.h"
#include "hunspelldict.h"
#include "hunspelldebug.h"

#include <QDir>
#include <QString>
#include <QStandardPaths>

using namespace Sonnet;

HunspellClient::HunspellClient(QObject *parent)
    : Client(parent)
{
    qCDebug(SONNET_HUNSPELL) << " HunspellClient::HunspellClient";

    QStringList dirList;
    // search QStandardPaths
    dirList.append(QStandardPaths::locateAll(
                       QStandardPaths::GenericDataLocation,
                       QStringLiteral("hunspell"),
                       QStandardPaths::LocateDirectory));

    auto maybeAddPath = [&dirList](const QString &path)
    {
        if (QFileInfo::exists(path)) {
            dirList.append(path);

            QDir dir(path);
            for (const QString &subDir : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                dirList.append(dir.absoluteFilePath(subDir));
            }

        }
    };
#ifdef Q_OS_WIN
    maybeAddPath(QStringLiteral(SONNET_INSTALL_PREFIX "/bin/data/hunspell/"));
#else
    maybeAddPath(QStringLiteral("/System/Library/Spelling"));
    maybeAddPath(QStringLiteral("/usr/share/hunspell/"));
    maybeAddPath(QStringLiteral("/usr/share/myspell/"));
#endif

    for (const QString &dirString : dirList) {
        QDir dir(dirString);
        const auto dicts = dir.entryInfoList({QStringLiteral("*.aff")}, QDir::Files);
        for (const QFileInfo &dict : dicts) {
            m_languagePaths.insert(dict.baseName(), dict.canonicalPath());
        }
    }
}

HunspellClient::~HunspellClient()
{
}

SpellerPlugin *HunspellClient::createSpeller(const QString &language)
{
    qCDebug(SONNET_HUNSPELL)
    << " SpellerPlugin *HunspellClient::createSpeller(const QString &language) ;" << language;
    HunspellDict *ad = new HunspellDict(language, m_languagePaths.value(language));
    return ad;
}

QStringList HunspellClient::languages() const
{
    return m_languagePaths.keys();
}
