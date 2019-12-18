/*
 * Copyright (c) 2014-2019 Patrizio Bekerle -- http://www.bekerle.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#pragma once

#include <QString>
#include <QUrl>
#include <QFile>
#include <QByteArray>
#include <QPrinter>
#include <QDataStream>
#include <QStringList>
#include <QMap>
#include <QSqlDatabase>
#include "entities/bookmark.h"

/*  Miscellaneous functions that can be useful */

namespace Utils {
    namespace Misc {
        struct SearchEngine {
            QString name;
            QString searchUrl;
            int id;
        };


        void openPath(const QString& absolutePath);
        void openFolderSelect(const QString& absolutePath);
        QString removeIfStartsWith(QString text, const QString& removeString);
        QString removeIfEndsWith(QString text, const QString& removeString);
        QString prependIfDoesNotStartWith(QString text, const QString& startString);
        QString appendIfDoesNotEndWith(QString text, const QString& endString);
        bool startDetachedProcess(const QString& executablePath,
                                  const QStringList& parameters = QStringList(),
                                  QString workingDirectory = "");
        QString shorten(QString text, int length, const QString& sequence = QStringLiteral("..."));
        QString cycleTextCase(const QString& text);
        QString toSentenceCase(const QString& text);
        QString toStartCase(const QString& text);
        QString defaultNotesPath();
        QString dirSeparator();
        void waitMsecs(int msecs);
        QString portableDataPath();
        bool isInPortableMode();
        QString prependPortableDataPathIfNeeded(QString path,
                                                bool ifNotEmptyOnly = false);
        QString makePathRelativeToPortableDataPathIfNeeded(QString path);
        QString htmlToMarkdown(QString text);
        QString parseTaskList(const QString &html, bool clickable);
        QByteArray startSynchronousProcess(
                const QString& executablePath, const QStringList &parameters,
                const QByteArray &data = QByteArray());
        QList<QObject *> getParents(QObject *object);
        QString appDataPath();
        QString logFilePath();
        QString transformLineFeeds(QString text);
        QString replaceOwnCloudText(QString text, bool useShortText = false);
        void restartApplication();
        void needRestart();
        bool downloadUrlToFile(const QUrl &url, QFile *file);
        QByteArray downloadUrl(const QUrl &url);
        QString genericCSS();
        QHash<int, SearchEngine> getSearchEnginesHashMap();
        int getDefaultSearchEngineId();
        void presetDisableAutomaticUpdateDialog();
        QList<int> getSearchEnginesIds();
        QDataStream &dataStreamWrite(QDataStream &os, const QPrinter &printer);
        QDataStream &dataStreamRead(QDataStream &is, QPrinter &printer);
        void storePrinterSettings(QPrinter *printer, const QString& settingsKey);
        void loadPrinterSettings(QPrinter *printer, const QString& settingsKey);
        bool isNoteEditingAllowed();
        bool useInternalExportStylingForPreview();
        bool isSocketServerEnabled();
        QString unescapeHtml(QString html);
        QString htmlspecialchars(QString text);
        void printInfo(const QString& text);
        bool doAutomaticNoteFolderDatabaseClosing();
        bool isNoteListPreview();
        bool isEnableNoteTree();
        QString indentCharacters();
        int indentSize();
        QString toHumanReadableByteSize(qint64 size);
        QString prepareDebugInformationLine(
            const QString &headline, QString data,
            bool withGitHubLineBreaks = true,
            const QString& typeText = QString());
        QString generateDebugInformation(bool withGitHubLineBreaks = true);
        bool regExpInListMatches(const QString& text, const QStringList &regExpList);
        bool isDarkModeIconTheme();
        void transformNextcloudPreviewImages(QString &html);
        void transformRemotePreviewImages(QString &html);
        QString remotePreviewImageTagToInlineImageTag(QString imageTag);
        QString createUuidString();
        QString localDictionariesPath();
    }  // namespace Misc
}  // namespace Utils


QDataStream& operator<<(QDataStream &os, const QPrinter &printer);
QDataStream& operator>>(QDataStream &is,  QPrinter &printer);
