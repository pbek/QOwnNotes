/*
 * Copyright (c) 2014-2017 Patrizio Bekerle -- http://www.bekerle.com
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

/*  Miscellaneous functions that can be useful */

namespace Utils {
    namespace Misc {
        void openPath(const QString& absolutePath);
        void openFolderSelect(const QString& absolutePath);
        QString removeIfStartsWith(QString text, QString removeString);
        QString removeIfEndsWith(QString text, QString removeString);
        QString prependIfDoesNotStartWith(QString text, QString startString);
        QString appendIfDoesNotEndWith(QString text, QString endString);
        bool startDetachedProcess(QString executablePath,
                                  QStringList parameters = QStringList(),
                                  QString workingDirectory = "");
        QString shorten(QString text, int length, QString sequence = "...");
        QString defaultNotesPath();
        QString dirSeparator();
        void waitMsecs(int msecs);
        QString portableDataPath();
        bool isInPortableMode();
        QString prependPortableDataPathIfNeeded(QString path,
                                                bool ifNotEmptyOnly = false);
        QString makePathRelativeToPortableDataPathIfNeeded(QString path);
        QString htmlToMarkdown(QString text);
        QByteArray startSynchronousProcess(
                QString executablePath, QStringList parameters,
                QByteArray data = QByteArray());
        QList<QObject *> getParents(QObject *object);
        QString appDataPath();
        QString logFilePath();
        QString transformLineFeeds(QString text);
        QString replaceOwnCloudText(QString text, bool useShortText = false);
        void restartApplication();
        void needRestart();
        bool downloadUrlToFile(QUrl url, QFile *file);
        QByteArray downloadUrl(QUrl url);
    }
}
