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

#include <utils/misc.h>

#include <QDir>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>
#include <QString>

#include "hunspelldebug.h"
#include "hunspelldict.h"

using namespace Sonnet;

HunspellClient::HunspellClient(QObject *parent) : Client(parent) {
#ifdef HUNSPELL_DEBUG_ON
    qCDebug(SONNET_HUNSPELL) << " HunspellClient::HunspellClient";
#endif
    QStringList dirList;
    QSettings settings;
    bool disableExternalDictionaries =
        settings.value(QStringLiteral("disableExternalDictionaries")).toBool();

    if (!disableExternalDictionaries) {
        // search QStandardPaths
        dirList.append(QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QStringLiteral("hunspell"),
                                                 QStandardPaths::LocateDirectory));
    }

    auto maybeAddPath = [&dirList](const QString &path) {
        if (QFileInfo::exists(path)) {
            dirList.append(path);

            QDir dir(path);
            for (const QString &subDir : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                dirList.append(dir.absoluteFilePath(subDir));
            }
        }
    };

    if (!disableExternalDictionaries) {
// custom paths
#ifdef Q_OS_WIN
        //    maybeAddPath(QStringLiteral(SONNET_INSTALL_PREFIX "/bin/data/hunspell/"));
        QString home = QDir::homePath() + "/dicts";
        QString current = QDir::currentPath() + "/dicts";
        maybeAddPath(home);
        maybeAddPath(current);
#endif
#ifdef Q_OS_LINUX
        maybeAddPath(QStringLiteral("/usr/share/hunspell/"));
        maybeAddPath(QStringLiteral("/usr/share/myspell/"));
        maybeAddPath(QStringLiteral("~/.local/share/hunspell/"));
        maybeAddPath(QStringLiteral("~/.local/share/myspell/"));

        // for snap packages
        // note that ~/.local is not readable for snaps
        QString snapDictPath = QDir::homePath() + QStringLiteral("/hunspell/");
        snapDictPath.remove(QRegularExpression(R"(snap\/qownnotes\/\w\d+\/)"));
        maybeAddPath(snapDictPath);
#endif
#ifdef Q_OS_MACOS
        // Waqar: enable this one only if we use hunspell for mac
        maybeAddPath(QStringLiteral("/System/Library/Spelling"));
        maybeAddPath(QDir::homePath() + QStringLiteral("/Library/Spelling"));
#endif
    }

    maybeAddPath(Utils::Misc::localDictionariesPath());

    for (const QString &dirString : dirList) {
        QDir dir(dirString);
        const auto dicts = dir.entryInfoList({QStringLiteral("*.aff")}, QDir::Files);
        for (const QFileInfo &dict : dicts) {
            m_languagePaths.insert(dict.baseName(), dict.canonicalPath());
        }
    }
}

HunspellClient::~HunspellClient() {}

SpellerPlugin *HunspellClient::createSpeller(const QString &language) {
#ifdef HUNSPELL_DEBUG_ON
    qCDebug(SONNET_HUNSPELL)
        << " SpellerPlugin *HunspellClient::createSpeller(const QString &language) ;" << language;
#endif
    HunspellDict *ad = new HunspellDict(language, m_languagePaths.value(language));
    return ad;
}

QStringList HunspellClient::languages() const { return m_languagePaths.keys(); }
