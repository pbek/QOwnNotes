/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
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

#include "misc.h"

#include <entities/calendaritem.h>
#include <entities/cloudconnection.h>
#include <entities/note.h>
#include <entities/notefolder.h>
#include <entities/notesubfolder.h>
#include <entities/script.h>
#include <services/databaseservice.h>
#include <services/owncloudservice.h>
#include <services/updateservice.h>
#include <threads/scriptthread.h>

#include <QApplication>
#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QHttpMultiPart>
#include <QJSValue>
#include <QJsonDocument>
#include <QMimeDatabase>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPrinter>
#include <QProcess>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringBuilder>
#include <QTemporaryFile>
#include <QTextDocument>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QUuid>
#include <QXmlStreamReader>
#include <QtGui/QIcon>
#include <utility>

#include "services/settingsservice.h"
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
#include <QHostInfo>
#endif

#include "build_number.h"
#include "libraries/sonnet/src/core/speller.h"
#include "release.h"
#include "version.h"

#ifndef INTEGRATION_TESTS
#include <QScreen>
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
#include <QGuiApplication>
#endif
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#include <QStandardPaths>
#endif

enum SearchEngines {
    Google = 0,
    Bing = 1,
    DuckDuckGo = 2,
    Yahoo = 3,
    GoogleScholar = 4,
    Yandex = 5,
    AskDotCom = 6,
    Qwant = 7,
    Startpage = 8
};

/**
 * Open the given path with an appropriate application
 * (thank you to qBittorrent for the inspiration)
 */
void Utils::Misc::openPath(const QString &absolutePath) {
    const QString path = QDir::fromNativeSeparators(absolutePath);
    // Hack to access samba shares with QDesktopServices::openUrl
    if (path.startsWith(QStringLiteral("//")))
        QDesktopServices::openUrl(QDir::toNativeSeparators(QStringLiteral("file:") % path));
    else
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

/**
 * Opens the parent directory of the given path with a file manager and select
 * (if possible) the item at the given path
 * (thank you to qBittorrent for the inspiration)
 */
void Utils::Misc::openFolderSelect(const QString &absolutePath) {
    const QString path = QDir::fromNativeSeparators(absolutePath);
#ifdef Q_OS_WIN
    if (QFileInfo(path).exists()) {
        // Syntax is: explorer /select, "C:\Folder1\Folder2\file_to_select"
        // Dir separators MUST be win-style slashes

        // QProcess::startDetached() has an obscure bug. If the path has
        // no spaces and a comma(and maybe other special characters) it doesn't
        // get wrapped in quotes. So explorer.exe can't find the correct path
        // anddisplays the default one. If we wrap the path in quotes and pass
        // it to QProcess::startDetached() explorer.exe still shows the default
        // path. In this case QProcess::startDetached() probably puts its
        // own quotes around ours.

        STARTUPINFO startupInfo;
        ::ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);

        PROCESS_INFORMATION processInfo;
        ::ZeroMemory(&processInfo, sizeof(processInfo));

        QString cmd = QStringLiteral("explorer.exe /select,\"%1\"")
                          .arg(QDir::toNativeSeparators(absolutePath));
        LPWSTR lpCmd = new WCHAR[cmd.size() + 1];
        cmd.toWCharArray(lpCmd);
        lpCmd[cmd.size()] = 0;

        bool ret = ::CreateProcessW(NULL, lpCmd, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo,
                                    &processInfo);
        delete[] lpCmd;

        if (ret) {
            ::CloseHandle(processInfo.hProcess);
            ::CloseHandle(processInfo.hThread);
        }
    } else {
        // If the item to select doesn't exist, try to open its parent
        openPath(path.left(path.lastIndexOf("/")));
    }
#elif defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
    static const auto fullXdgMimePath = QStandardPaths::findExecutable(QStringLiteral("xdg-mime"));
    if (!fullXdgMimePath.isEmpty() && QFileInfo(path).exists()) {
        QProcess proc;
        QString output;
        proc.start(fullXdgMimePath, QStringList{"query", "default", "inode/directory"});
        proc.waitForFinished();
        output = proc.readLine().simplified();
        if (output == QStringLiteral("dolphin.desktop") ||
            output == QStringLiteral("org.kde.dolphin.desktop")) {
            proc.startDetached(QStringLiteral("dolphin"),
                               QStringList{"--select", QDir::toNativeSeparators(path)});
        } else if (output == QStringLiteral("nautilus.desktop") ||
                   output == QStringLiteral("org.gnome.Nautilus.desktop") ||
                   output == QStringLiteral("nautilus-folder-handler.desktop")) {
            proc.startDetached(QStringLiteral("nautilus"),
                               QStringList{"--no-desktop", QDir::toNativeSeparators(path)});
        } else if (output == QStringLiteral("caja-folder-handler.desktop")) {
            const QDir dir = QFileInfo(path).absoluteDir();
            const QString absDir = dir.absolutePath();
            proc.startDetached(QStringLiteral("caja"),
                               QStringList{"--no-desktop", QDir::toNativeSeparators(absDir)});
        } else if (output == QStringLiteral("nemo.desktop")) {
            proc.startDetached(QStringLiteral("nemo"),
                               QStringList{"--no-desktop", QDir::toNativeSeparators(path)});
        } else if (output == QStringLiteral("konqueror.desktop") ||
                   output == QStringLiteral("kfmclient_dir.desktop")) {
            proc.startDetached(QStringLiteral("konqueror"),
                               QStringList{"--select", QDir::toNativeSeparators(path)});
        } else {
            openPath(path.left(path.lastIndexOf(QLatin1Char('/'))));
        }
    } else {
        // if the item to select doesn't exist, try to open its parent
        openPath(path.left(path.lastIndexOf(QLatin1Char('/'))));
    }
#else
    openPath(path.left(path.lastIndexOf(QLatin1Char('/'))));
#endif
}

/**
 * Removes a string from the start if it starts with it
 */
QString Utils::Misc::removeIfStartsWith(QString text, const QString &removeString) {
    if (text.startsWith(removeString)) {
        text.remove(
            QRegularExpression(QStringLiteral("^") + QRegularExpression::escape(removeString)));
    }

    return text;
}

/**
 * Removes a string from the end if it ends with it
 */
QString Utils::Misc::removeIfEndsWith(QString text, const QString &removeString) {
    if (text.endsWith(removeString)) {
        text.remove(
            QRegularExpression(QRegularExpression::escape(removeString) + QStringLiteral("$")));
    }

    return text;
}

/**
 * Adds a string to the beginning of a string if it doesn't start with it
 */
QString Utils::Misc::prependIfDoesNotStartWith(QString text, const QString &startString) {
    if (!text.startsWith(startString)) {
        text.prepend(startString);
    }

    return text;
}

/**
 * Adds a string to the end of a string if it doesn't end with it
 */
QString Utils::Misc::appendIfDoesNotEndWith(QString text, const QString &endString) {
    if (!text.endsWith(endString)) {
        text.append(endString);
    }

    return text;
}

/**
 * Shortens text and adds a sequence string if text is too long
 */
QString Utils::Misc::shorten(QString text, int length, const QString &sequence) {
    if (text.length() > length) {
        int newLength = length - sequence.length();

        if (newLength < 0) {
            newLength = 0;
        }

        return (text.left(newLength) + sequence).left(length);
    }

    return text;
}

/**
 * Cycles text through lowercase, uppercase, start case, and sentence case
 */
QString Utils::Misc::cycleTextCase(const QString &text) {
    if (text.isEmpty()) return text;

    const QString asLower = text.toLower();
    const QString asUpper = text.toUpper();

    // OK no matter what
    if (text == asLower) {
        return asUpper;
    }

    const QString asStart = toStartCase(text);
    const QString asSentence = toSentenceCase(text);

    if (text == asUpper) {
        if (asUpper == asStart) {
            // text == asUpper == asStart == asSentence && text != asLower
            if (asUpper == asSentence) {
                return asLower;
            }
            // text == asUpper == asStart && text != asSentence
            else {
                return asSentence;
            }
        }
        // text == asUpper && text != asStart
        else {
            return asStart;
        }
    }

    if (text == asStart) {
        // text == asStart == asSentence && asSentence != asLower
        if (asStart == asSentence) {
            return asLower;
        }
        // text == asStart && text != asSentence
        else {
            return asSentence;
        }
    }

    return asLower;
}

/**
 * Converts text to sentence case
 */
QString Utils::Misc::toSentenceCase(const QString &text) {
    // A sentence is a string of characters immediately preceded by:
    // (beginning of string followed by any amount of horizontal or vertical
    //     whitespace) or
    // (any of [.?!] followed by at least one horizontal or vertical
    //     whitespace)
    static const QRegularExpression sentenceSplitter(
        QStringLiteral(R"((^[\s\v]*|[.?!][\s\v]+)\K)"));

    QStringList sentences = text.toLower().split(sentenceSplitter);

    for (QString &sentence : sentences) {
        if (sentence.length() > 0) {
            sentence = sentence.left(1).toUpper() + sentence.right(sentence.length() - 1);
        }
    }

    return sentences.join(QString());
}

/**
 * Converts text to start case
 */
QString Utils::Misc::toStartCase(const QString &text) {
    // A word is a string of characters immediately preceded by horizontal or
    // vertical whitespace
    static const QRegularExpression wordSplitter(QStringLiteral("(?<=[\\s\\v])"));

    QStringList words = text.toLower().split(wordSplitter);

    for (QString &word : words) {
        if (word.length() > 0) {
            word = word.at(0).toUpper() % word.right(word.length() - 1);
        }
    }

    return words.join(QString());
}

/**
 * Starts an executable detached with parameters
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param workingDirectory the directory to run the executable from (optional)
 * @return true on success, false otherwise
 */
bool Utils::Misc::startDetachedProcess(const QString &executablePath, const QStringList &parameters,
                                       QString workingDirectory) {
    QProcess process;

    if (workingDirectory.isEmpty()) {
        // set the directory to run the executable from
        // process.setWorkingDirectory() doesn't seem
        // to do anything under Windows
        workingDirectory = QCoreApplication::applicationDirPath();
    }

    // start executablePath detached with parameters
#ifdef Q_OS_MAC
    return process.startDetached(QStringLiteral("open"),
                                 QStringList() << executablePath << "--args" << parameters,
                                 workingDirectory);
#else
    return process.startDetached(executablePath, parameters, workingDirectory);
#endif
}

/**
 * Opens files via an executable
 *
 * @param executablePath the path of the executable
 * @param files a list of file strings
 * @param workingDirectory the directory to run the executable from
 * @return true on success, false otherwise
 */
bool Utils::Misc::openFilesWithApplication(const QString &executablePath, const QStringList &files,
                                           QString workingDirectory) {
#ifdef Q_OS_MAC
    QProcess process;

    if (workingDirectory.isEmpty()) {
        workingDirectory = QCoreApplication::applicationDirPath();
    }

    // start executablePath detached with parameters
    return process.startDetached(
        QStringLiteral("open"), QStringList() << "-a" << executablePath << files, workingDirectory);
#else
    return startDetachedProcess(executablePath, files, workingDirectory);
#endif
}

/**
 * Starts an executable synchronous with parameters
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param data the data that will be written to the process (optional)
 * @param workingDirectory the working directory to execute the process in (optional)
 * @return the text that was returned by the process
 */
QByteArray Utils::Misc::startSynchronousProcess(const QString &executablePath,
                                                const QStringList &parameters,
                                                const QByteArray &data,
                                                const QString &workingDirectory) {
    QProcess process;

    if (!workingDirectory.isEmpty()) {
        process.setWorkingDirectory(workingDirectory);
    }

    // start executablePath synchronous with parameters
#ifdef Q_OS_MAC
    process.start("open", QStringList() << executablePath << "--args" << parameters);
#else
    process.start(executablePath, parameters);
#endif

    if (!process.waitForStarted()) {
        qWarning() << __func__ << " - 'process.waitForStarted' returned false";
        return QByteArray();
    }

    process.write(data);
    process.closeWriteChannel();

    if (!process.waitForFinished()) {
        qWarning() << __func__ << " - 'process.waitForFinished' returned false";
        return QByteArray();
    }

    const QByteArray result = process.readAll();
    return result;
}

/**
 * Starts a synchronous process and waits for its result
 *
 * @param cmd the terminal command containing all input and output information
 * @return true on success, false otherwise
 */
bool Utils::Misc::startSynchronousResultProcess(TerminalCmd &cmd) {
    QProcess process;

    // start executablePath synchronous with parameters
#ifdef Q_OS_MAC
    process.start("open", QStringList() << cmd.executablePath << "--args" << cmd.parameters);
#else
    process.start(cmd.executablePath, cmd.parameters);
#endif

    if (!process.waitForStarted()) {
        qWarning() << __func__ << " - 'process.waitForStarted' returned false";
        return false;
    }

    process.write(cmd.data);
    process.closeWriteChannel();

    if (!process.waitForFinished()) {
        qWarning() << __func__ << " - 'process.waitForFinished' returned false";
        return false;
    }

    cmd.resultSet = process.readAll();
    cmd.exitCode = process.exitCode();
    return process.exitStatus() == QProcess::NormalExit;
}

/**
 * Returns the default notes path we are suggesting
 *
 * @return
 */
QString Utils::Misc::defaultNotesPath() {
    // it seems QDir::separator() is not needed,
    // because Windows also uses "/" in QDir::homePath()
    QString path = isInPortableMode() ? portableDataPath()
                                      : QDir::homePath() % Utils::Misc::dirSeparator() %
                                            QStringLiteral("ownCloud");

    // check if the ownCloud path already exists (because the user is using
    // ownCloud), if not use Nextcloud
    if (!isInPortableMode()) {
        // we don't want to use the regular sync path for debugging
#ifdef QT_DEBUG
        path.append(".test");
#endif

        // check if ownCloud path is found
        QDir dir(path);
        if (!dir.exists()) {
            // set Nextcloud path if ownCloud wasn't found
            path = QDir::homePath() % Utils::Misc::dirSeparator() % QStringLiteral("Nextcloud");
        }

        // check if Nextcloud path is found
        dir = QDir(path);
        if (!dir.exists()) {
            // set home directory path if Nextcloud wasn't found
            path = QDir::homePath();
        }
    }

    // add "Notes" to the base path
    path += Utils::Misc::dirSeparator() % QStringLiteral("Notes");

    // remove the snap path for Snapcraft builds
    static const QRegularExpression re(QStringLiteral(R"(snap\/qownnotes\/\w\d+\/)"));
    path.remove(re);

    return path;
}

/**
 * Returns the directory separator
 * Replaces QDir::separator() because it seems methods like
 * QDir::homePath() are using "/" under Windows too
 *
 * @return
 */
char Utils::Misc::dirSeparator() { return '/'; }

void Utils::Misc::waitMsecs(int msecs) {
    QTime dieTime = QTime::currentTime().addMSecs(msecs);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
}

/**
 * Returns (and creates) the portable data path
 *
 * @return the path
 */
QString Utils::Misc::portableDataPath() {
    QString path = QString();

    if (qApp != nullptr) {
        // for AppImages we best take out the path directly from the first
        // argument of the executable arguments, because the applicationDirPath
        // will be some temporary path
        if (Utils::Misc::isAppImage()) {
            const QFileInfo fileInfo(Utils::Misc::applicationPath());
            path = fileInfo.absolutePath();
        } else {
            path = QCoreApplication::applicationDirPath();
        }
    }

    // use a fallback if the QApplication object wasn't instantiated yet
    if (path.isEmpty()) {
        path = QStringLiteral(".");
    }

    // it seems QDir::separator() is not needed, because Windows also
    // uses "/" in QCoreApplication::applicationDirPath()
    path += dirSeparator() % QStringLiteral("Data");

    QDir dir;
    // create path if it doesn't exist yet
    dir.mkpath(path);

    return path;
}

/**
 * Returns true if the app is in portable mode
 *
 * @return
 */
bool Utils::Misc::isInPortableMode() {
    return qApp != nullptr ? qApp->property("portable").toBool() : false;
}

/**
 * Returns true if the app is an AppImage
 *
 * @return
 */
bool Utils::Misc::isAppImage() {
    return qApp->property("release").toString().contains(QStringLiteral("AppImage"));
}

/**
 * Prepends the portable data path if we are in portable mode
 *
 * @param path
 * @return
 */
QString Utils::Misc::prependPortableDataPathIfNeeded(QString path, bool ifNotEmptyOnly) {
    if (ifNotEmptyOnly && path.isEmpty()) {
        return QString();
    }

    if (isInPortableMode()) {
        QString portableDataPath(Utils::Misc::portableDataPath());

        // check if the path already starts with the portable data path
        if (!path.startsWith(portableDataPath)) {
            path = portableDataPath % QStringLiteral("/") % path;
        }
    }

    return path;
}

/**
 * Makes the path relative to the portable data path if we are in portable mode
 *
 * @param path
 * @return
 */
QString Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(QString path) {
    if (isInPortableMode()) {
        //        path.remove(QRegularExpression(
        //                "^" + QRegularExpression::escape(portableDataPath()) +
        //                        "[\\//]"));

        // make the path relative to the portable data path
        QDir dir(portableDataPath());
        path = dir.relativeFilePath(path);
    }

    return path;
}

/**
 * Converts html tags to Markdown
 *
 * @param text
 * @return Markdown text
 */
QString Utils::Misc::htmlToMarkdown(QString text) {
    // replace Windows line breaks
    static const QRegularExpression re(QStringLiteral("\r\n"));
    text.replace(re, QStringLiteral("\n"));

    // remove all null characters
    // we can get those from Google Chrome via the clipboard
    text.remove(QChar(0));

    // Protect existing Markdown code blocks from being processed
    QRegularExpression existingCodeBlockRe(QStringLiteral("```[^`]*```"),
                                           QRegularExpression::DotMatchesEverythingOption);
    QStringList existingCodeBlocks;
    QRegularExpressionMatchIterator codeBlockIt = existingCodeBlockRe.globalMatch(text);
    int placeholderIndex = 0;

    while (codeBlockIt.hasNext()) {
        QRegularExpressionMatch match = codeBlockIt.next();
        QString codeBlock = match.captured(0);
        existingCodeBlocks.append(codeBlock);
        QString placeholder =
            QStringLiteral("___CODE_BLOCK_PLACEHOLDER_%1___").arg(placeholderIndex++);
        text.replace(codeBlock, placeholder);
    }

    // remove some blocks
    text.remove(QRegularExpression(QStringLiteral("<head.*?>(.+?)<\\/head>"),
                                   QRegularExpression::CaseInsensitiveOption |
                                       QRegularExpression::DotMatchesEverythingOption));

    text.remove(QRegularExpression(QStringLiteral("<script.*?>(.+?)<\\/script>"),
                                   QRegularExpression::CaseInsensitiveOption |
                                       QRegularExpression::DotMatchesEverythingOption));

    text.remove(QRegularExpression(QStringLiteral("<style.*?>(.+?)<\\/style>"),
                                   QRegularExpression::CaseInsensitiveOption |
                                       QRegularExpression::DotMatchesEverythingOption));

    // Handle tables - must be done before other replacements
    // Convert HTML tables to Markdown tables
    QRegularExpression tableRe(
        QStringLiteral("<table[^>]*?>(.*?)<\\/table>"),
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatchIterator tableIt = tableRe.globalMatch(text);

    while (tableIt.hasNext()) {
        QRegularExpressionMatch tableMatch = tableIt.next();
        QString tableHtml = tableMatch.captured(1);
        QString markdownTable;

        // Extract rows (handle both thead/tbody and plain tr)
        QRegularExpression rowRe(QStringLiteral("<tr[^>]*?>(.*?)<\\/tr>"),
                                 QRegularExpression::CaseInsensitiveOption |
                                     QRegularExpression::DotMatchesEverythingOption);
        QRegularExpressionMatchIterator rowIt = rowRe.globalMatch(tableHtml);

        QStringList rows;
        bool isFirstRow = true;
        int columnCount = 0;

        while (rowIt.hasNext()) {
            QRegularExpressionMatch rowMatch = rowIt.next();
            QString rowHtml = rowMatch.captured(1);

            // Extract cells (th or td)
            QRegularExpression cellRe(QStringLiteral("<(th|td)[^>]*?>(.*?)<\\/\\1>"),
                                      QRegularExpression::CaseInsensitiveOption |
                                          QRegularExpression::DotMatchesEverythingOption);
            QRegularExpressionMatchIterator cellIt = cellRe.globalMatch(rowHtml);

            QStringList cells;
            while (cellIt.hasNext()) {
                QRegularExpressionMatch cellMatch = cellIt.next();
                QString cellContent = cellMatch.captured(2);

                // Remove inner HTML tags from cell content
                cellContent.remove(QRegularExpression(QStringLiteral("<[^>]+>"),
                                                      QRegularExpression::CaseInsensitiveOption));
                cellContent = cellContent.trimmed();

                // Escape pipe characters in cell content
                cellContent.replace(QStringLiteral("|"), QStringLiteral("\\|"));

                cells.append(cellContent);
            }

            if (!cells.isEmpty()) {
                columnCount = qMax(columnCount, cells.count());
                rows.append(QStringLiteral("| ") + cells.join(QStringLiteral(" | ")) +
                            QStringLiteral(" |"));

                // Add separator row after first row (header)
                if (isFirstRow) {
                    QStringList separators;
                    for (int i = 0; i < cells.count(); ++i) {
                        separators.append(QStringLiteral("---"));
                    }
                    rows.append(QStringLiteral("| ") + separators.join(QStringLiteral(" | ")) +
                                QStringLiteral(" |"));
                    isFirstRow = false;
                }
            }
        }

        if (!rows.isEmpty()) {
            markdownTable =
                QStringLiteral("\n\n") + rows.join(QStringLiteral("\n")) + QStringLiteral("\n\n");
            text.replace(tableMatch.captured(0), markdownTable);
        }
    }

    // Handle strikethrough
    text.replace(QRegularExpression(QStringLiteral("<(s|strike|del).*?>(.+?)<\\/\\1>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("~~\\2~~"));

    // Handle underline (convert to emphasis since Markdown doesn't have native underline)
    text.replace(QRegularExpression(QStringLiteral("<u.*?>(.+?)<\\/u>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("_\\1_"));

    // Handle mark/highlight
    text.replace(QRegularExpression(QStringLiteral("<mark.*?>(.+?)<\\/mark>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("==\\1=="));

    // Handle subscript and superscript
    text.replace(QRegularExpression(QStringLiteral("<sub.*?>(.+?)<\\/sub>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("~\\1~"));
    text.replace(QRegularExpression(QStringLiteral("<sup.*?>(.+?)<\\/sup>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("^\\1^"));

    // Handle blockquote
    text.replace(QRegularExpression(QStringLiteral("<blockquote.*?>(.+?)<\\/blockquote>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("\n\n> \\1\n\n"));

    // Handle horizontal rule
    text.replace(
        QRegularExpression(QStringLiteral("<hr.*?/?>"), QRegularExpression::CaseInsensitiveOption),
        QStringLiteral("\n\n---\n\n"));

    // Handle images - must be done before links
    text.replace(
        QRegularExpression(QStringLiteral("<img[^>]+src=\"([^\"]+)\"[^>]*alt=\"([^\"]+)\"[^>]*>"),
                           QRegularExpression::CaseInsensitiveOption),
        QStringLiteral("![\\2](\\1)"));
    text.replace(
        QRegularExpression(QStringLiteral("<img[^>]+alt=\"([^\"]+)\"[^>]*src=\"([^\"]+)\"[^>]*>"),
                           QRegularExpression::CaseInsensitiveOption),
        QStringLiteral("![\\1](\\2)"));
    text.replace(QRegularExpression(QStringLiteral("<img[^>]+src=\"([^\"]+)\"[^>]*>"),
                                    QRegularExpression::CaseInsensitiveOption),
                 QStringLiteral("![](\\1)"));

    // Handle code blocks with language first
    text.replace(QRegularExpression(QStringLiteral("<pre[^>]*><code[^>]+class=\"[^\"]*language-([^"
                                                   "\"\\s]+)[^\"]*\"[^>]*>(.+?)<\\/code><\\/pre>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("\n\n```\\1\n\\2\n```\n\n"));

    // Handle pre blocks
    text.replace(QRegularExpression(QStringLiteral("<pre.*?>(.+?)<\\/pre>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("\n\n```\n\\1\n```\n\n"));

    // Handle standalone code blocks
    text.replace(QRegularExpression(QStringLiteral("<code.*?>(.+?)<\\/code>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("`\\1`"));

    // replace some html tags with Markdown
    text.replace(QRegularExpression(QStringLiteral("<strong.*?>(.+?)<\\/strong>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("**\\1**"));
    text.replace(QRegularExpression(QStringLiteral("<b.*?>(.+?)<\\/b>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("**\\1**"));
    text.replace(QRegularExpression(QStringLiteral("<em.*?>(.+?)<\\/em>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("*\\1*"));
    text.replace(QRegularExpression(QStringLiteral("<i.*?>(.+?)<\\/i>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("*\\1*"));
    text.replace(QRegularExpression(QStringLiteral("<h1.*?>(.+?)<\\/h1>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("\n\n# \\1\n\n"));
    text.replace(QRegularExpression(QStringLiteral("<h2.*?>(.+?)<\\/h2>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("\n\n## \\1\n\n"));
    text.replace(QRegularExpression(QStringLiteral("<h3.*?>(.+?)<\\/h3>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("\n\n### \\1\n\n"));
    text.replace(QRegularExpression(QStringLiteral("<h4.*?>(.+?)<\\/h4>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("\n\n#### \\1\n\n"));
    text.replace(QRegularExpression(QStringLiteral("<h5.*?>(.+?)<\\/h5>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("\n\n##### \\1\n\n"));
    text.replace(QRegularExpression(QStringLiteral("<h6.*?>(.+?)<\\/h6>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("\n\n###### \\1\n\n"));

    // Handle ordered lists
    text.replace(QRegularExpression(QStringLiteral("<ol[^>]*?>(.*?)<\\/ol>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("\\1"));

    // Handle unordered lists
    text.replace(QRegularExpression(QStringLiteral("<ul[^>]*?>(.*?)<\\/ul>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("\\1"));

    text.replace(QRegularExpression(QStringLiteral("<li.*?>(.+?)<\\/li>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("- \\1\n"));

    text.replace(
        QRegularExpression(QStringLiteral("<br.*?>"), QRegularExpression::CaseInsensitiveOption),
        QStringLiteral("\n"));

    // Handle links (must be after images)
    text.replace(QRegularExpression(QStringLiteral("<a[^>]+href=\"(.+?)\".*?>(.+?)<\\/a>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("[\\2](\\1)"));

    // Handle div and span (just extract content)
    text.replace(
        QRegularExpression(QStringLiteral("<div.*?>"), QRegularExpression::CaseInsensitiveOption),
        QStringLiteral("\n"));
    text.replace(
        QRegularExpression(QStringLiteral("<\\/div>"), QRegularExpression::CaseInsensitiveOption),
        QStringLiteral("\n"));
    text.replace(
        QRegularExpression(QStringLiteral("<span.*?>"), QRegularExpression::CaseInsensitiveOption),
        QStringLiteral(""));
    text.replace(
        QRegularExpression(QStringLiteral("<\\/span>"), QRegularExpression::CaseInsensitiveOption),
        QStringLiteral(""));

    // Handle paragraphs
    text.replace(QRegularExpression(QStringLiteral("<p.*?>(.+?)</p>"),
                                    QRegularExpression::CaseInsensitiveOption |
                                        QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("\n\n\\1\n\n"));

    // Remove any remaining HTML tags
    text.remove(
        QRegularExpression(QStringLiteral("<[^>]+>"), QRegularExpression::CaseInsensitiveOption));

    // Decode HTML entities manually to preserve linebreaks
    // Common HTML entities
    text.replace(QStringLiteral("&amp;"), QStringLiteral("&"));
    text.replace(QStringLiteral("&lt;"), QStringLiteral("<"));
    text.replace(QStringLiteral("&gt;"), QStringLiteral(">"));
    text.replace(QStringLiteral("&quot;"), QStringLiteral("\""));
    text.replace(QStringLiteral("&apos;"), QStringLiteral("'"));
    text.replace(QStringLiteral("&#39;"), QStringLiteral("'"));
    text.replace(QStringLiteral("&nbsp;"), QStringLiteral(" "));

    // Decode numeric HTML entities (&#xxx; and &#xHH;)
    QRegularExpression numericEntityRe(QStringLiteral("&#(\\d+);"));
    QRegularExpressionMatchIterator it = numericEntityRe.globalMatch(text);
    QList<QPair<QString, QString>> replacements;

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        bool ok;
        int charCode = match.captured(1).toInt(&ok);
        if (ok && charCode > 0) {
            QString entity = match.captured(0);
            QString replacement = QString(QChar(charCode));
            replacements.append(qMakePair(entity, replacement));
        }
    }

    for (const auto &pair : replacements) {
        text.replace(pair.first, pair.second);
    }

    // Decode hex numeric HTML entities (&#xHH;)
    QRegularExpression hexEntityRe(QStringLiteral("&#x([0-9A-Fa-f]+);"));
    it = hexEntityRe.globalMatch(text);
    replacements.clear();

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        bool ok;
        int charCode = match.captured(1).toInt(&ok, 16);
        if (ok && charCode > 0) {
            QString entity = match.captured(0);
            QString replacement = QString(QChar(charCode));
            replacements.append(qMakePair(entity, replacement));
        }
    }

    for (const auto &pair : replacements) {
        text.replace(pair.first, pair.second);
    }

    // Clean up excessive whitespace
    text.replace(QRegularExpression(QStringLiteral("[ \\t]+\n")), QStringLiteral("\n"));
    text.replace(QRegularExpression(QStringLiteral("\n[ \\t]+")), QStringLiteral("\n"));

    // replace multiple line breaks
    text.replace(QRegularExpression(QStringLiteral("\n\n\n+")), QStringLiteral("\n\n"));

    // Restore protected code blocks
    for (int i = 0; i < existingCodeBlocks.size(); ++i) {
        QString placeholder = QStringLiteral("___CODE_BLOCK_PLACEHOLDER_%1___").arg(i);
        text.replace(placeholder, existingCodeBlocks[i]);
    }

    // Trim leading/trailing whitespace
    text = text.trimmed();

    return text;
}

/**
 * Returns new html with task lists by checkboxes
 *
 * @param html
 * @param clickable
 * @return
 */
QString Utils::Misc::parseTaskList(const QString &html, bool clickable) {
    auto text = html;

    // set a list item style for todo items
    // using a css class didn't work because the styling seems to affects the sub-items too
    const auto listTag = QStringLiteral("<li style=\"list-style-type:square\">");

    static const QRegularExpression re1(QStringLiteral(R"(<li>(\s*(<p>)*\s*)\[ ?\])"),
                                        QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression re2(QStringLiteral(R"(<li>(\s*(<p>)*\s*)\[[xX]\])"),
                                        QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression re3(QStringLiteral(R"(<li>(\s*(<p>)*\s*)\[-\])"),
                                        QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression reInputChecked(
        QStringLiteral(R"(<input[^>]*\btype\s*=\s*["']?checkbox["']?[^>]*\bchecked\b[^>]*>)"),
        QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression reInputUnchecked(
        QStringLiteral(R"(<input[^>]*\btype\s*=\s*["']?checkbox["']?[^>]*>)"),
        QRegularExpression::CaseInsensitiveOption);
    if (!clickable) {
        text.replace(reInputChecked, QStringLiteral("&#9745;"));
        text.replace(reInputUnchecked, QStringLiteral("&#9744;"));
        text.replace(re1, listTag % QStringLiteral("\\1&#9744;"));
        text.replace(re2, listTag % QStringLiteral("\\1&#9745;"));
        text.replace(re3, listTag % QStringLiteral("\\1&#10005;"));
        return text;
    }

    // TODO
    // to ensure the clicking behavior of checkboxes,
    // line numbers of checkboxes in the original Markdown text
    // should be provided by the Markdown parser

    const QString checkboxStart =
        QStringLiteral(R"(<a class="task-list-item-checkbox" href="checkbox://_)");
    text.replace(reInputChecked, checkboxStart % QStringLiteral("\">&#9745;</a>"));
    text.replace(reInputUnchecked, checkboxStart % QStringLiteral("\">&#9744;</a>"));
    text.replace(re3, listTag % QStringLiteral("\\1&#10005;"));
    text.replace(
        re1, listTag % QStringLiteral("\\1") % checkboxStart % QStringLiteral("\">&#9744;</a>"));
    text.replace(
        re2, listTag % QStringLiteral("\\1") % checkboxStart % QStringLiteral("\">&#9745;</a>"));

    int count = 0;
    int pos = 0;
    while (true) {
        pos = text.indexOf(checkboxStart % QStringLiteral("\""), pos);
        if (pos == -1) break;

        pos += checkboxStart.length();
        text.insert(pos, QString::number(count++));
    }

    return text;
}

/**
 * Returns a list of all parents until the top
 *
 * @param object
 * @return
 */
QList<QObject *> Utils::Misc::getParents(QObject *object) {
    QList<QObject *> list;
    QObject *parent = object->parent();

    if (parent != nullptr) {
        list = getParents(parent);
        list << parent;
    }

    return list;
}

/**
 * Returns the application data path
 *
 * @return
 */
QString Utils::Misc::appDataPath() {
    QString path = QString();

    if (isInPortableMode()) {
        path = portableDataPath();
    } else {
        QStandardPaths::StandardLocation location;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
        location = QStandardPaths::AppDataLocation;
#else
        location = QStandardPaths::GenericDataLocation;
#endif

        // get the path to store the database
        path = QStandardPaths::writableLocation(location);

        QDir dir;

        // create path if it doesn't exist yet
        dir.mkpath(path);
    }

    return path;
}

/**
 * Returns the log file path
 *
 * @return
 */
QString Utils::Misc::logFilePath() { return appDataPath() % QStringLiteral("/QOwnNotes.log"); }

/**
 * Transforms all line feeds to \n
 *
 * @param text
 * @return
 */
QString Utils::Misc::transformLineFeeds(QString text) {
    static const QRegularExpression re(QStringLiteral(R"((\r\n)|(\n\r)|\r|\n)"));
    return text.replace(re, QStringLiteral("\n"));
}

/**
 * Replaces the text "ownCloud" by "ownCloud / Nextcloud"
 *
 * @param text
 * @param useShortText
 * @return
 */
QString Utils::Misc::replaceOwnCloudText(QString text, bool useShortText) {
    if (text.contains(QStringLiteral("Nextcloud"))) {
        return text;
    }

    QString replaceText =
        useShortText ? QStringLiteral("NC / oC") : QStringLiteral("Nextcloud / ownCloud");
    return text.replace(QStringLiteral("ownCloud"), replaceText, Qt::CaseInsensitive);
}

/**
 * Declares that we need a restart
 */
void Utils::Misc::needRestart() { qApp->setProperty("needsRestart", true); }

/**
 * Restarts the application
 */
void Utils::Misc::restartApplication() {
    // QApplication::arguments() didn't contain any parameters!
    QStringList parameters = qApp->property("arguments").toStringList();
    const QString appPath = Utils::Misc::applicationPath();

    // we don't want to have our settings cleared again after a restart
    parameters.removeOne(QStringLiteral("--clear-settings"));

    // If only one app instance is allowed force allowing multiple for the
    // next launch, so we can launch the application before we quit the
    // current instance
    if (qApp->property("singleApplication").toBool() &&
        !parameters.contains("--allow-multiple-instances")) {
        parameters.append("--allow-multiple-instances");
    }

    startDetachedProcess(appPath, parameters);
    QApplication::quit();
}

QString Utils::Misc::applicationPath() {
    QString appPath;

    if (isAppImage()) {
        // qApp->property("arguments").toStringList() doesn't report the correct path for the
        // AppImages of the deployment process for Ubuntu 20.04!
        // (see https://github.com/pbek/QOwnNotes/issues/2728#issuecomment-1466522381)
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        appPath = env.value("APPIMAGE");
    } else {
        QStringList parameters = qApp->property("arguments").toStringList();
        appPath = parameters.takeFirst();
    }

    return appPath;
}

QString Utils::Misc::appendSingleAppInstanceTextIfNeeded(QString text) {
    if (SettingsService().value("allowOnlyOneAppInstance").toBool()) {
        text.append(QStringLiteral("\n\n") +
                    QObject::tr("You are using the single app instance mode, that "
                                "prevents the application be be started a second time. For the "
                                "next launch of the application the single app instance mode "
                                "will be disabled, so that the application can be restarted "
                                "before quitting this instance."));
    }

    return text;
}

QByteArray Utils::Misc::friendlyUserAgentString() {
    const auto pattern = QStringLiteral("%1 (QOwnNotes - %2)");

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    const auto userAgent = pattern.arg(QSysInfo::machineHostName(), platform());
#else
    const auto userAgent = pattern.arg(QHostInfo::localHostName(), platform());
#endif

    return userAgent.toUtf8();
}

QLatin1String Utils::Misc::platform() {
#if defined(Q_OS_WIN)
    return QLatin1String("Windows");
#elif defined(Q_OS_MAC)
    return QLatin1String("macOS");
#elif defined(Q_OS_LINUX)
    return QLatin1String("Linux");
#elif defined(Q_OS_FREEBSD) || defined(Q_OS_FREEBSD_KERNEL)
    return QLatin1String("FreeBSD");
#elif defined(Q_OS_NETBSD)
    return QLatin1String("NetBSD");
#elif defined(Q_OS_OPENBSD)
    return QLatin1String("OpenBSD");
#elif defined(Q_OS_SOLARIS)
    return QLatin1String("Solaris");
#else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    return QSysInfo::productType();
#else
    return " Qt " + QString(QT_VERSION_STR);
#endif
#endif
}

/**
 * Downloads an url and returns the data
 *
 * @param url
 * @return {QByteArray} the content of the downloaded url
 */
QByteArray Utils::Misc::downloadUrl(const QUrl &url, bool usePost, QByteArray postData) {
    int statusCode;

    return downloadUrlWithStatusCode(url, statusCode, usePost, std::move(postData));
}

QByteArray Utils::Misc::downloadUrlWithStatusCode(const QUrl &url, int &returnStatusCode,
                                                  bool usePost, QByteArray postData) {
    auto *manager = new QNetworkAccessManager();
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));

    // 10 sec timeout for the request
    timer.start(10000);

    QNetworkRequest networkRequest = QNetworkRequest(url);
    networkRequest.setHeader(QNetworkRequest::UserAgentHeader,
                             Utils::Misc::friendlyUserAgentString());

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#else
    networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
#endif

    QByteArray data;
    QNetworkReply *reply;

    if (usePost) {
        if (postData == nullptr) {
            postData = QByteArray();
        }

        networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                                 "application/x-www-form-urlencoded");
        reply = manager->post(networkRequest, postData);
    } else {
        reply = manager->get(networkRequest);
    }

    loop.exec();

    // if we didn't get a timeout let us return the content
    if (timer.isActive()) {
        returnStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // only get the data if the status code was "success"
        // see: https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
        if (returnStatusCode >= 200 && returnStatusCode < 300) {
            // get the data from the network reply
            data = reply->readAll();
        }
    }

    reply->deleteLater();
    delete (manager);

    return data;
}

/**
 * Downloads an url and stores it to a file
 */
bool Utils::Misc::downloadUrlToFile(const QUrl &url, QFile *file) {
    if (!file->open(QIODevice::WriteOnly)) {
        return false;
    }

    if (!file->isWritable()) {
        return false;
    }

    QByteArray data = downloadUrl(url);
    if (data.size() > 0) {
        file->write(data);
        return true;
    }

    return false;
}

/**
 * Returns generic CSS styles for displaying HTML in QTextBrowser, QLabel or
 * similar
 *
 * @return
 */
QString Utils::Misc::genericCSS() {
    SettingsService settings;
    const bool darkModeColors = settings.value(QStringLiteral("darkModeColors")).toBool();
    QString color = darkModeColors ? QStringLiteral("#ffd694") : QStringLiteral("#fc7600");
    QString cssStyles = QStringLiteral("a {color: ") % color % QStringLiteral("}");

    color = darkModeColors ? QStringLiteral("#5b5b5b") : QStringLiteral("#e8e8e8");
    cssStyles += QStringLiteral("kbd {background-color: ") % color % QStringLiteral("}");

    color = darkModeColors ? QStringLiteral("#336924") : QStringLiteral("#d6ffc7");
    cssStyles += QStringLiteral("ins {background-color: ") % color % QStringLiteral("}");

    color = darkModeColors ? QStringLiteral("#802c2c") : QStringLiteral("#ffd7d7");
    cssStyles += QStringLiteral("del {background-color: ") % color % QStringLiteral("}");
    return cssStyles;
}

/**
 * A vector of pairs, the first item in the pair is the search
 * engine name and the second item is the search url of the engine.
 */
QHash<int, Utils::Misc::SearchEngine> Utils::Misc::getSearchEnginesHashMap() {
    QHash<int, Utils::Misc::SearchEngine> searchEngines;
    searchEngines.insert(SearchEngines::Google, {QStringLiteral("Google"),
                                                 QStringLiteral("https://www.google.com/search?q="),
                                                 SearchEngines::Google});
    searchEngines.insert(SearchEngines::Bing,
                         {QStringLiteral("Bing"), QStringLiteral("https://www.bing.com/search?q="),
                          SearchEngines::Bing});
    searchEngines.insert(
        SearchEngines::DuckDuckGo,
        {QStringLiteral("DuckDuckGo"), QStringLiteral("https://duckduckgo.com/?t=qownnotes&q="),
         SearchEngines::DuckDuckGo});
    searchEngines.insert(
        SearchEngines::Yahoo,
        {QStringLiteral("Yahoo"), QStringLiteral("https://search.yahoo.com/search?p="),
         SearchEngines::Yahoo});
    searchEngines.insert(
        SearchEngines::GoogleScholar,
        {QStringLiteral("Google Scholar"),
         QStringLiteral("https://scholar.google.co.il/scholar?q="), SearchEngines::GoogleScholar});
    searchEngines.insert(
        SearchEngines::Yandex,
        {QStringLiteral("Yandex"), QStringLiteral("https://www.yandex.com/search/?text="),
         SearchEngines::Yandex});
    searchEngines.insert(SearchEngines::AskDotCom,
                         {QStringLiteral("Ask.com"), QStringLiteral("https://www.ask.com/web?q="),
                          SearchEngines::AskDotCom});
    searchEngines.insert(SearchEngines::Qwant,
                         {QStringLiteral("Qwant"), QStringLiteral("https://www.qwant.com/?q="),
                          SearchEngines::Qwant});
    searchEngines.insert(
        SearchEngines::Startpage,
        {QStringLiteral("Startpage"), QStringLiteral("https://www.startpage.com/do/dsearch?query="),
         SearchEngines::Startpage});
    return searchEngines;
}

/**
 * Returns the default search engine id
 *
 * @return
 */
int Utils::Misc::getDefaultSearchEngineId() { return SearchEngines::DuckDuckGo; }

/**
 * Returns a list of search engines in the order it should be displayed
 *
 * @return
 */
QList<int> Utils::Misc::getSearchEnginesIds() {
    return QList<int>{
        SearchEngines::DuckDuckGo, SearchEngines::Google,        SearchEngines::Bing,
        SearchEngines::Yahoo,      SearchEngines::GoogleScholar, SearchEngines::Yandex,
        SearchEngines::AskDotCom,  SearchEngines::Qwant,         SearchEngines::Startpage};
}

/**
 * Disables the automatic update dialog per default for repositories and
 * self-builds if nothing is already set
 */
void Utils::Misc::presetDisableAutomaticUpdateDialog() {
    SettingsService settings;

    // disable the automatic update dialog per default for repositories and
    // self-builds
    if (settings.value(QStringLiteral("disableAutomaticUpdateDialog")).toString().isEmpty()) {
        QString release = qApp->property("release").toString();
        bool enabled = release.contains(QStringLiteral("Travis")) ||
                       release.contains(QStringLiteral("AppVeyor")) ||
                       release.contains(QStringLiteral("AppImage"));
        settings.setValue(QStringLiteral("disableAutomaticUpdateDialog"), !enabled);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Write all available Attributes from QPrinter into stream
///////////////////////////////////////////////////////////////////////////////

template <typename T>
void writeStreamElement(QDataStream &os, const T param) {
    const int i = static_cast<int>(param);
    os << i;
}

template <>
void writeStreamElement<QString>(QDataStream &os, const QString s) {
    os << s;
}

/**
 * Save printer settings to a stream
 */
QDataStream &Utils::Misc::dataStreamWrite(QDataStream &os, const QPrinter &printer) {
    writeStreamElement(os, printer.printerName());
    writeStreamElement(os, printer.pageLayout().pageSize().id());
    writeStreamElement(os, printer.collateCopies());
    writeStreamElement(os, printer.colorMode());
    writeStreamElement(os, printer.copyCount());
    writeStreamElement(os, printer.creator());
    writeStreamElement(os, printer.docName());
    writeStreamElement(os, printer.duplex());
    writeStreamElement(os, printer.fontEmbeddingEnabled());
    writeStreamElement(os, printer.fullPage());
    writeStreamElement(os, printer.pageLayout().orientation());
    writeStreamElement(os, printer.outputFileName());
    writeStreamElement(os, printer.outputFormat());
    writeStreamElement(os, printer.pageOrder());
    writeStreamElement(os, printer.paperSource());
    writeStreamElement(os, printer.printProgram());
    writeStreamElement(os, printer.printRange());
    writeStreamElement(os, printer.printerName());
    writeStreamElement(os, printer.resolution());

    auto margins = printer.pageLayout().margins(QPageLayout::Unit::Millimeter);
    os << margins.left() << margins.top() << margins.right() << margins.bottom();

    Q_ASSERT_X(os.status() == QDataStream::Ok, __FUNCTION__,
               QStringLiteral("Stream status = %1").arg(os.status()).toStdString().c_str());
    return os;
}

///////////////////////////////////////////////////////////////////////////////
// Read all available Attributes from stream into QPrinter
///////////////////////////////////////////////////////////////////////////////

template <typename T>
T readStreamElement(QDataStream &is) {
    int i;
    is >> i;
    return static_cast<T>(i);
}

template <>
QString readStreamElement<QString>(QDataStream &is) {
    QString s;
    is >> s;
    return s;
}

/**
 * Load printer settings from a stream
 */
QDataStream &Utils::Misc::dataStreamRead(QDataStream &is, QPrinter &printer) {
    printer.setPrinterName(readStreamElement<QString>(is));

    QPageSize pageSize(readStreamElement<QPageSize::PageSizeId>(is));
    auto pageLayout = printer.pageLayout();
    pageLayout.setPageSize(pageSize);
    printer.setPageLayout(pageLayout);

    printer.setCollateCopies(readStreamElement<bool>(is));
    printer.setColorMode(readStreamElement<QPrinter::ColorMode>(is));
    printer.setCopyCount(readStreamElement<int>(is));
    printer.setCreator(readStreamElement<QString>(is));
    printer.setDocName(readStreamElement<QString>(is));
    printer.setDuplex(readStreamElement<QPrinter::DuplexMode>(is));
    printer.setFontEmbeddingEnabled(readStreamElement<bool>(is));
    printer.setFullPage(readStreamElement<bool>(is));
    printer.setPageOrientation(readStreamElement<QPageLayout::Orientation>(is));
    printer.setOutputFileName(readStreamElement<QString>(is));
    printer.setOutputFormat(readStreamElement<QPrinter::OutputFormat>(is));
    printer.setPageOrder(readStreamElement<QPrinter::PageOrder>(is));
    printer.setPaperSource(readStreamElement<QPrinter::PaperSource>(is));
    printer.setPrintProgram(readStreamElement<QString>(is));
    printer.setPrintRange(readStreamElement<QPrinter::PrintRange>(is));
    printer.setPrinterName(readStreamElement<QString>(is));
    printer.setResolution(readStreamElement<int>(is));

    qreal left, top, right, bottom;
    is >> left >> top >> right >> bottom;

    QMarginsF margins(left, top, right, bottom);
    printer.setPageMargins(margins, QPageLayout::Unit::Millimeter);

    Q_ASSERT_X(is.status() == QDataStream::Ok, __FUNCTION__,
               QStringLiteral("Stream status = %1").arg(is.status()).toStdString().c_str());

    return is;
}

/**
 * Stores the properties of a QPrinter to the settings
 *
 * @param printer
 * @param settingsKey
 */
void Utils::Misc::storePrinterSettings(QPrinter *printer, const QString &settingsKey) {
    QByteArray byteArr;
    QDataStream os(&byteArr, QIODevice::WriteOnly);
    dataStreamWrite(os, *printer);
    SettingsService().setValue(settingsKey, byteArr.toHex());
}

/**
 * Loads the properties of a QPrinter from the settings
 *
 * @param printer
 * @param settingsKey
 */
void Utils::Misc::loadPrinterSettings(QPrinter *printer, const QString &settingsKey) {
    SettingsService settings;

    if (!settings.value(settingsKey).isValid()) {
        return;
    }

    QByteArray printSetup = settings.value(settingsKey).toByteArray();
    printSetup = QByteArray::fromHex(printSetup);
    QDataStream is(&printSetup, QIODevice::ReadOnly);
    dataStreamRead(is, *printer);
}

/**
 * Returns if "allowNoteEditing" is turned on
 *
 * @return
 */
bool Utils::Misc::isNoteEditingAllowed() {
    return SettingsService().value(QStringLiteral("allowNoteEditing"), true).toBool();
}

/**
 * Returns if "hideIconsInMenus" is turned on
 *
 * @return
 */
bool Utils::Misc::areMenuIconsHidden() {
#ifdef Q_OS_MAC
    // hide icons in menus by default on macOS
    const bool defaultValue = true;
#else
    const bool defaultValue = false;
#endif

    return SettingsService().value(QStringLiteral("hideIconsInMenus"), defaultValue).toBool();
}

/**
 * Returns if "useInternalExportStyling" is turned on
 *
 * @return
 */
bool Utils::Misc::useInternalExportStylingForPreview() {
    return SettingsService()
        .value(QStringLiteral("MainWindow/noteTextView.useInternalExportStyling"), true)
        .toBool();
}

/**
 * Returns the preview font string
 *
 * @return
 */
QString Utils::Misc::previewFontString() {
    return SettingsService()
        .value(isPreviewUseEditorStyles() ? QStringLiteral("MainWindow/noteTextEdit.font")
                                          : QStringLiteral("MainWindow/noteTextView.font"))
        .toString();
}

/**
 * Returns the preview code font string
 *
 * @return
 */
QString Utils::Misc::previewCodeFontString() {
    return SettingsService()
        .value(isPreviewUseEditorStyles() ? QStringLiteral("MainWindow/noteTextEdit.code.font")
                                          : QStringLiteral("MainWindow/noteTextView.code.font"))
        .toString();
}

/**
 * Returns if "MainWindow/noteTextView.useEditorStyles" is turned on
 *
 * @return
 */
bool Utils::Misc::isPreviewUseEditorStyles() {
    return SettingsService()
        .value(QStringLiteral("MainWindow/noteTextView.useEditorStyles"), true)
        .toBool();
}

/**
 * Returns if "enableSocketServer" is turned on
 *
 * @return
 */
bool Utils::Misc::isSocketServerEnabled() {
    return SettingsService().value(QStringLiteral("enableSocketServer"), true).toBool();
}

/**
 * Returns if "enableWebAppSupport" is turned on
 *
 * @return
 */
bool Utils::Misc::isWebAppSupportEnabled() {
    return SettingsService().value(QStringLiteral("enableWebAppSupport"), false).toBool();
}

/**
 * Returns if "restoreCursorPosition" is turned on
 *
 * @return
 */
bool Utils::Misc::isRestoreCursorPosition() {
#ifdef Q_OS_MAC
    const bool defaultValue = false;
#else
    const bool defaultValue = true;
#endif

    return SettingsService().value(QStringLiteral("restoreCursorPosition"), defaultValue).toBool();
}

/**
 * Returns if "darkModeIconTheme" is turned on
 *
 * @return
 */
bool Utils::Misc::isDarkModeIconTheme() {
    const SettingsService settings;
    const bool darkMode = settings.value(QStringLiteral("darkMode")).toBool();
    return settings.value(QStringLiteral("darkModeIconTheme"), darkMode).toBool();
}

/**
 * Returns if "automaticNoteFolderDatabaseClosing" is turned on
 *
 * @return
 */
bool Utils::Misc::doAutomaticNoteFolderDatabaseClosing() {
    return SettingsService().value(QStringLiteral("automaticNoteFolderDatabaseClosing")).toBool();
}

/**
 * Returns if "noteListPreview" is turned on
 *
 * @return
 */
bool Utils::Misc::isNoteListPreview() {
    return SettingsService().value(QStringLiteral("noteListPreview")).toBool();
}

/**
 * Returns if "enableNoteTree" is turned on
 *
 * @return
 */
bool Utils::Misc::isEnableNoteTree() {
    return SettingsService().value(QStringLiteral("enableNoteTree")).toBool();
}

/**
 * Returns the characters to use to indent
 *
 * @return
 */
QString Utils::Misc::indentCharacters() {
    return SettingsService().value("Editor/useTabIndent").toBool()
               ? QStringLiteral("\t")
               : QStringLiteral(" ").repeated(indentSize());
}

/**
 * Returns the indent size
 *
 * @return
 */
int Utils::Misc::indentSize() { return SettingsService().value("Editor/indentSize", 4).toInt(); }

/**
 * Unescapes html special characters
 *
 * @param html
 * @return
 */
QString Utils::Misc::unescapeHtml(QString html, bool soft) {
    // It looks like "text.toPlainText();" also destroys "<a" tags, so we
    // also want a "soft" un-escaping method
    if (soft) {
        html.replace("&lt;", "<");
        html.replace("&gt;", ">");
        html.replace("&amp;", "&");
        html.replace("&quot;", "\"");
        html.replace("&apos;", "'");

        return html;
    }

    // text.toPlainText() will remove all line breaks, we don't want that
    html.replace(QStringLiteral("\n"), QStringLiteral("<br>"));

    QTextDocument text;
    text.setHtml(html);

    return text.toPlainText();
}

/**
 * Escapes some html special characters
 *
 * @param text
 * @return
 */
QString Utils::Misc::htmlspecialchars(QString text) {
    text.replace(QStringLiteral("&"), QStringLiteral("&amp;"));
    text.replace(QStringLiteral("\""), QStringLiteral("&quot;"));
    text.replace(QStringLiteral("'"), QStringLiteral("&apos;"));
    text.replace(QStringLiteral("<"), QStringLiteral("&lt;"));
    text.replace(QStringLiteral(">"), QStringLiteral("&gt;"));
    return text;
}

/**
 * Outputs an info text
 *
 * @param text
 */
void Utils::Misc::printInfo(const QString &text) { qInfo() << text; }

/**
 * Converts a byte size to a human readable size
 *
 * @param size
 * @return
 */
QString Utils::Misc::toHumanReadableByteSize(qint64 size) {
    static const QStringList units = {"bytes", "KB", "MB", "GB", "TB"};
    constexpr double threshold = 1024.0;

    double num = qAbs(static_cast<double>(size));
    int unitIndex = 0;

    while (num >= threshold && unitIndex < units.size() - 1) {
        num /= threshold;
        ++unitIndex;
    }

    // Handle special case for bytes
    if (unitIndex == 0) {
        return QStringLiteral("%1 %2").arg(size).arg(units[unitIndex]);
    }

    // Use QString::number for better performance and more control
    return QStringLiteral("%1 %2").arg(QString::number(num, 'f', 2), units[unitIndex]);
}

/**
 * @brief Prepares the debug information to output it as Markdown
 * @param headline
 * @param data
 */
QString Utils::Misc::prepareDebugInformationLine(const QString &headline, QString data,
                                                 bool withGitHubLineBreaks,
                                                 const QString &typeText) {
    // add two spaces if we don't want GitHub line breaks
    const QString spaces = withGitHubLineBreaks ? QString() : QStringLiteral("  ");

    if (data.contains(QStringLiteral("\n"))) {
        data = QStringLiteral("\n```\n") % data.trimmed() % QStringLiteral("\n```");
    } else {
        data = (data.isEmpty()) ? QStringLiteral("*empty*") : "`" % data % "`";
    }

    QString resultText = QStringLiteral("**") % headline % QStringLiteral("**");

    if (!typeText.isEmpty()) {
        resultText += QStringLiteral(" (") % typeText % QStringLiteral(")");
    }

    resultText += QStringLiteral(": ") % data % spaces % QStringLiteral("\n");
    return resultText;
}

QString Utils::Misc::generateDebugInformation(bool withGitHubLineBreaks) {
    const SettingsService settings;
    QString output;

    output += QStringLiteral("QOwnNotes Debug Information\n");
    output += QStringLiteral("===========================\n");

    QDateTime dateTime = QDateTime::currentDateTime();

    // add information about QOwnNotes
    output += QStringLiteral("\n## General Info\n\n");
    output += prepareDebugInformationLine(QStringLiteral("Current Date"), dateTime.toString(),
                                          withGitHubLineBreaks);
    output += prepareDebugInformationLine(QStringLiteral("Version"), QString(VERSION),
                                          withGitHubLineBreaks);
    output += prepareDebugInformationLine(QStringLiteral("Build date"), QString(__DATE__),
                                          withGitHubLineBreaks);
    output += prepareDebugInformationLine(QStringLiteral("Build number"), QString::number(BUILD),
                                          withGitHubLineBreaks);
    output += prepareDebugInformationLine(
        QStringLiteral("Platform"),
        QStringLiteral("%1 (%2)").arg(PLATFORM, QGuiApplication::platformName()),
        withGitHubLineBreaks);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    output += prepareDebugInformationLine(QStringLiteral("Operating System"),
                                          QSysInfo::prettyProductName(), withGitHubLineBreaks);
    output += prepareDebugInformationLine(QStringLiteral("Build architecture"),
                                          QSysInfo::buildCpuArchitecture(), withGitHubLineBreaks);
    output += prepareDebugInformationLine(QStringLiteral("Current architecture"),
                                          QSysInfo::currentCpuArchitecture(), withGitHubLineBreaks);
#endif
    output += prepareDebugInformationLine(
        QStringLiteral("Release"), qApp->property("release").toString(), withGitHubLineBreaks);
    output += prepareDebugInformationLine(QStringLiteral("Qt Version (build)"),
                                          QStringLiteral(QT_VERSION_STR), withGitHubLineBreaks);
    output += prepareDebugInformationLine(QStringLiteral("Qt Version (runtime)"), qVersion(),
                                          withGitHubLineBreaks);
    output += prepareDebugInformationLine(
        QStringLiteral("Portable mode"),
        Utils::Misc::isInPortableMode() ? QStringLiteral("yes") : QStringLiteral("no"),
        withGitHubLineBreaks);
    output += prepareDebugInformationLine(QStringLiteral("Settings path / key"),
                                          settings.fileName(), withGitHubLineBreaks);
    output += prepareDebugInformationLine(
        QStringLiteral("Application database path"),
        QDir::toNativeSeparators(DatabaseService::getDiskDatabasePath()), withGitHubLineBreaks);
    output += prepareDebugInformationLine(
        QStringLiteral("Application arguments"),
        qApp->property("arguments").toStringList().join(QStringLiteral("`, `")),
        withGitHubLineBreaks);

    QString debug = QStringLiteral("no");
#ifdef QT_DEBUG
    debug = QStringLiteral("yes");
#endif

    output += prepareDebugInformationLine(QStringLiteral("Qt Debug"), debug, withGitHubLineBreaks);

    QString systemBotan = QStringLiteral("no");
#ifdef USE_SYSTEM_BOTAN
    systemBotan = QStringLiteral("yes");
#endif

    output += prepareDebugInformationLine(QStringLiteral("System Botan"), systemBotan,
                                          withGitHubLineBreaks);

    QString useLibGit2 = QStringLiteral("no");
#ifdef USE_LIBGIT2
    useLibGit2 = QStringLiteral("yes");
#endif

    output +=
        prepareDebugInformationLine(QStringLiteral("Libgit2"), useLibGit2, withGitHubLineBreaks);

    QString useQLiteHtml = QStringLiteral("no");
#ifdef USE_QLITEHTML
    useQLiteHtml = QStringLiteral("yes");
#endif

    output += prepareDebugInformationLine(QStringLiteral("QLiteHtml"), useQLiteHtml,
                                          withGitHubLineBreaks);

    output += prepareDebugInformationLine(QStringLiteral("Locale (system)"),
                                          QLocale::system().name(), withGitHubLineBreaks);
    output += prepareDebugInformationLine(
        QStringLiteral("Locale (interface)"),
        settings.value(QStringLiteral("interfaceLanguage")).toString(), withGitHubLineBreaks);
#ifndef INTEGRATION_TESTS

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QScreen *primaryScreen = qApp->primaryScreen();
#else
    QScreen *primaryScreen = QGuiApplication::primaryScreen();
#endif
    if (primaryScreen != nullptr) {
        QString screenResolution = QString::number(primaryScreen->geometry().width()) + "x" +
                                   QString::number(primaryScreen->geometry().height());
        output += prepareDebugInformationLine(QStringLiteral("Primary screen resolution"),
                                              screenResolution, withGitHubLineBreaks);
    }

    QList<QScreen *> screens = qApp->screens();

    if (screens.count() > 1) {
        QStringList screenResolutions;

        Q_FOREACH (QScreen *screen, screens) {
            screenResolutions.append(QString::number(screen->geometry().width()) + "x" +
                                     QString::number(screen->geometry().height()));
        }

        output += prepareDebugInformationLine(QStringLiteral("Screen resolution(s)"),
                                              screenResolutions.join(QStringLiteral(", ")),
                                              withGitHubLineBreaks);
    }
#endif

    output += prepareDebugInformationLine(QStringLiteral("Icon theme"), QIcon::themeName(),
                                          withGitHubLineBreaks);
    output += prepareDebugInformationLine(QStringLiteral("Notes in current note folder"),
                                          QString::number(Note::countAll()), withGitHubLineBreaks);
    output += prepareDebugInformationLine(QStringLiteral("Calendar items"),
                                          QString::number(CalendarItem::countAll()),
                                          withGitHubLineBreaks);
    output +=
        prepareDebugInformationLine(QStringLiteral("Enabled scripts"),
                                    QString::number(Script::countEnabled()), withGitHubLineBreaks);
    output += prepareDebugInformationLine(QStringLiteral("Database drivers"),
                                          QSqlDatabase::drivers().join(QStringLiteral(", ")),
                                          withGitHubLineBreaks);

    // add information about the server
    output += QStringLiteral("\n## Server Info\n\n");
    output += prepareDebugInformationLine(QStringLiteral("serverUrl"),
                                          CloudConnection::currentCloudConnection().getServerUrl(),
                                          withGitHubLineBreaks);
    const bool appIsValid = settings.value(QStringLiteral("ownCloudInfo/appIsValid")).toBool();
    output += prepareDebugInformationLine(
        QStringLiteral("appIsValid"),
        QString(appIsValid ? QStringLiteral("yes") : QStringLiteral("no")), withGitHubLineBreaks);
    output += prepareDebugInformationLine(
        QStringLiteral("notesPathExists"),
        settings.value(QStringLiteral("ownCloudInfo/notesPathExistsText")).toString(),
        withGitHubLineBreaks);
    if (appIsValid) {
        output += prepareDebugInformationLine(
            QStringLiteral("serverVersion"),
            settings.value(QStringLiteral("ownCloudInfo/serverVersion")).toString(),
            withGitHubLineBreaks);
        output += prepareDebugInformationLine(
            QStringLiteral("appVersion"),
            settings.value(QStringLiteral("ownCloudInfo/appVersion")).toString(),
            withGitHubLineBreaks);
    } else {
        output += prepareDebugInformationLine(
            QStringLiteral("connectionErrorMessage"),
            settings.value(QStringLiteral("ownCloudInfo/connectionErrorMessage")).toString(),
            withGitHubLineBreaks);
    }

    // add spellchecker information

    output += QStringLiteral("\n## Spellchecking\n\n");
    output += prepareDebugInformationLine(
        QStringLiteral("Enabled"), settings.value(QStringLiteral("checkSpelling")).toString(),
        withGitHubLineBreaks);
    output += prepareDebugInformationLine(
        QStringLiteral("Selected language"),
        settings.value(QStringLiteral("spellCheckLanguage")).toString(), withGitHubLineBreaks);

#ifndef INTEGRATION_TESTS
    // auto *speller = new Sonnet::Speller();
    output += prepareDebugInformationLine(
        QStringLiteral("Language codes"),
        Sonnet::Speller::availableLanguages().join(QStringLiteral(", ")), withGitHubLineBreaks);
    output += prepareDebugInformationLine(
        QStringLiteral("Language names"),
        Sonnet::Speller::availableLanguageNames().join(QStringLiteral(", ")), withGitHubLineBreaks);
    // delete speller;
#endif
    output += prepareDebugInformationLine(
        QStringLiteral("Application dictionaries path"),
        QDir::toNativeSeparators(Utils::Misc::localDictionariesPath()), withGitHubLineBreaks);

    // add note folder information
    output += QStringLiteral("\n## Note folders\n\n");
    output += prepareDebugInformationLine(QStringLiteral("currentNoteFolderId"),
                                          QString::number(NoteFolder::currentNoteFolderId()),
                                          withGitHubLineBreaks);

    const QList<NoteFolder> &noteFolders = NoteFolder::fetchAll();
    if (noteFolders.count() > 0) {
        Q_FOREACH (const NoteFolder &noteFolder, noteFolders) {
            output += QStringLiteral("\n### Note folder `") % noteFolder.getName() +
                      QStringLiteral("`\n\n");
            output += prepareDebugInformationLine(
                QStringLiteral("id"), QString::number(noteFolder.getId()), withGitHubLineBreaks);
            output += prepareDebugInformationLine(
                QStringLiteral("isCurrent"),
                noteFolder.isCurrent() ? QStringLiteral("yes") : QStringLiteral("no"),
                withGitHubLineBreaks);
            output += prepareDebugInformationLine(QStringLiteral("activeTagId"),
                                                  QString::number(noteFolder.getActiveTagId()),
                                                  withGitHubLineBreaks);
            output += prepareDebugInformationLine(
                QStringLiteral("localPath"), QDir::toNativeSeparators(noteFolder.getLocalPath()),
                withGitHubLineBreaks);
            output += prepareDebugInformationLine(QStringLiteral("remotePath"),
                                                  noteFolder.getRemotePath(), withGitHubLineBreaks);
            output += prepareDebugInformationLine(
                QStringLiteral("cloudConnectionId"),
                QString::number(noteFolder.getCloudConnectionId()), withGitHubLineBreaks);
            output += prepareDebugInformationLine(
                QStringLiteral("isShowSubfolders"),
                noteFolder.isShowSubfolders() ? QStringLiteral("yes") : QStringLiteral("no"),
                withGitHubLineBreaks);
            output += prepareDebugInformationLine(
                QStringLiteral("isUseGit"),
                noteFolder.isUseGit() ? QStringLiteral("yes") : QStringLiteral("no"),
                withGitHubLineBreaks);
            output += prepareDebugInformationLine(
                QStringLiteral("allowDifferentNoteFileName"),
                noteFolder.settingsValue(QStringLiteral("allowDifferentNoteFileName")).toBool()
                    ? QStringLiteral("yes")
                    : QStringLiteral("no"),
                withGitHubLineBreaks);
            output += prepareDebugInformationLine(QStringLiteral("activeNoteSubFolder name"),
                                                  noteFolder.getActiveNoteSubFolder().getName(),
                                                  withGitHubLineBreaks);
            output += prepareDebugInformationLine(
                QStringLiteral("database file"),
                QDir::toNativeSeparators(noteFolder.getLocalPath() +
                                         QStringLiteral("/notes.sqlite")),
                withGitHubLineBreaks);
        }
    }

    // add cloud connection information
    output += QStringLiteral("\n## Cloud connections\n");

    Q_FOREACH (CloudConnection cloudConnection, CloudConnection::fetchAll()) {
        output += QStringLiteral("\n### Cloud connection `") % cloudConnection.getName() +
                  QStringLiteral("`\n\n");
        output += prepareDebugInformationLine(
            QStringLiteral("id"), QString::number(cloudConnection.getId()), withGitHubLineBreaks);
        output += prepareDebugInformationLine(
            QStringLiteral("isCurrent"),
            cloudConnection.isCurrent() ? QStringLiteral("yes") : QStringLiteral("no"),
            withGitHubLineBreaks);
        output += prepareDebugInformationLine(QStringLiteral("serverUrl"),
                                              cloudConnection.getServerUrl(), withGitHubLineBreaks);
        output += prepareDebugInformationLine(QStringLiteral("username"),
                                              cloudConnection.getUsername(), withGitHubLineBreaks);
        output += prepareDebugInformationLine(QStringLiteral("accountId"),
                                              cloudConnection.getAccountId(), withGitHubLineBreaks);

        if (cloudConnection.getNextcloudDeckEnabled()) {
            output += prepareDebugInformationLine(
                QStringLiteral("Nextcloud Deck boardId"),
                QString::number(cloudConnection.getNextcloudDeckBoardId()), withGitHubLineBreaks);
            output += prepareDebugInformationLine(
                QStringLiteral("Nextcloud Deck stackId"),
                QString::number(cloudConnection.getNextcloudDeckStackId()), withGitHubLineBreaks);
        }
    }

    // add script information
    output += QStringLiteral("\n## Enabled scripts\n");

    QList<Script> scripts = Script::fetchAll(true);
    if (noteFolders.count() > 0) {
        Q_FOREACH (Script script, scripts) {
            output += QStringLiteral("\n### Script `") % script.getName() + QStringLiteral("`\n\n");
            output += prepareDebugInformationLine(
                QStringLiteral("id"), QString::number(script.getId()), withGitHubLineBreaks);
            output += prepareDebugInformationLine(QStringLiteral("path"),
                                                  QDir::toNativeSeparators(script.getScriptPath()),
                                                  withGitHubLineBreaks);
            output += prepareDebugInformationLine(QStringLiteral("variablesJson"),
                                                  script.getSettingsVariablesJson(true),
                                                  withGitHubLineBreaks);
            if (script.isScriptFromRepository()) {
                ScriptInfoJson infoJson = script.getScriptInfoJson();

                output += prepareDebugInformationLine(QStringLiteral("identifier"),
                                                      script.getIdentifier(), withGitHubLineBreaks);
                output += prepareDebugInformationLine(QStringLiteral("version"), infoJson.version,
                                                      withGitHubLineBreaks);
                output += prepareDebugInformationLine(QStringLiteral("minAppVersion"),
                                                      infoJson.minAppVersion, withGitHubLineBreaks);
            }
        }
    } else {
        output += QStringLiteral("\nThere are no enabled scripts.\n");
    }

    // add information about the settings
    output += QStringLiteral("\n## Settings\n\n");

    // hide values of these keys
    QStringList keyHiddenList = {"cryptoKey",
                                 "ownCloud/todoCalendarCalDAVPassword",
                                 "PiwikClientId",
                                 "networking/proxyPassword",
                                 "ai/groq/apiKey",
                                 "ai/openai/apiKey",
                                 "webSocketServerService/token",
                                 "webAppClientService/token"};

    // under OS X we have to ignore some keys
#ifdef Q_OS_MAC
    QStringList keyIgnoreList;
    keyIgnoreList << "AKDeviceUnlockState"
                  << "Apple"
                  << "NS"
                  << "NavPanel"
                  << "com/apple";
#endif

    QListIterator<QString> itr(settings.allKeys());
    while (itr.hasNext()) {
        QString key = itr.next();
        QVariant value = settings.value(key);

        // under OS X we have to ignore some keys
#ifdef Q_OS_MAC
        bool ignoreKey = false;

        // ignore values of certain keys
        QListIterator<QString> itr2(keyIgnoreList);
        while (itr2.hasNext()) {
            QString pattern = itr2.next();
            if (key.startsWith(pattern)) {
                ignoreKey = true;
                break;
            }
        }

        // check if key has to be ignored
        if (ignoreKey) {
            continue;
        }
#endif

        // hide values of certain keys
        if (keyHiddenList.contains(key)) {
            output += prepareDebugInformationLine(key, QStringLiteral("<hidden>"),
                                                  withGitHubLineBreaks, value.typeName());
        } else {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            switch (value.typeId()) {
#else
            switch (value.type()) {
#endif
                case QMetaType::QStringList:
                    output += prepareDebugInformationLine(
                        key, value.toStringList().join(QStringLiteral(", ")), withGitHubLineBreaks,
                        value.typeName());
                    break;
                case QMetaType::QVariantList:
                    output +=
                        prepareDebugInformationLine(key,
                                                    QStringLiteral("<variant list with %1 item(s)>")
                                                        .arg(value.toList().count()),
                                                    withGitHubLineBreaks, value.typeName());
                    break;
                case QMetaType::QByteArray:
                case QMetaType::User:
                    output += prepareDebugInformationLine(key, QStringLiteral("<binary data>"),
                                                          withGitHubLineBreaks, value.typeName());
                    break;
                default:
                    output += prepareDebugInformationLine(key, value.toString(),
                                                          withGitHubLineBreaks, value.typeName());
            }
        }
    }

    // add information about the system environment
    output += QStringLiteral("\n## System environment\n\n");

    itr = QProcess::systemEnvironment();
    while (itr.hasNext()) {
        QStringList textList = itr.next().split(QStringLiteral("="));
        QString key = textList.first();
        textList.removeFirst();
        QString value = textList.join(QStringLiteral("="));
        output += prepareDebugInformationLine(key, value, withGitHubLineBreaks);
    }

    return output;
}

/**
 * Checks if text matches a regular expression in regExpList
 *
 * @param text
 * @param regExpList
 * @return
 */
bool Utils::Misc::regExpInListMatches(const QString &text, const QStringList &regExpList) {
    for (const QString &regExp : regExpList) {
        const QString trimmed = regExp.trimmed();
        if (trimmed.isEmpty()) {
            continue;
        }

        if (QRegularExpression(trimmed).match(text).hasMatch()) {
            return true;
        }
    }

    return false;
}

/**
 * Transforms Nextcloud preview image tags
 *
 * @param html
 */
void Utils::Misc::transformNextcloudPreviewImages(QString &html, int maxImageWidth,
                                                  ExternalImageHash *externalImageHash) {
    OwnCloudService *ownCloud = OwnCloudService::instance();

    static const QRegularExpression re(
        QStringLiteral(
            R"(<img src=\"(\/core\/preview\?fileId=.+#mimetype=[\w\d%]+&.+)\" alt=\".+\"\/?>)"),
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption);
    QRegularExpressionMatchIterator i = re.globalMatch(html);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        const QString imageTag = match.captured(0);
        QString inlineImageTag;
        int imageWidth = maxImageWidth;
        ExternalImageHashItem hashItem;

        if (externalImageHash->contains(imageTag)) {
            hashItem = externalImageHash->value(imageTag);
            inlineImageTag = hashItem.imageTag;
            imageWidth = hashItem.imageWidth;
        } else {
            inlineImageTag =
                ownCloud->nextcloudPreviewImageTagToInlineImageTag(imageTag, imageWidth);
            hashItem.imageTag = inlineImageTag;
            hashItem.imageWidth = imageWidth;
            externalImageHash->insert(imageTag, hashItem);
        }

        imageWidth = std::min(maxImageWidth, imageWidth);
        inlineImageTag.replace("/>",
                               QStringLiteral("width=\"%1\"/>").arg(QString::number(imageWidth)));

        html.replace(imageTag, inlineImageTag);
    }
}

/**
 * Transforms remote preview image tags
 *
 * @param html
 */
void Utils::Misc::transformRemotePreviewImages(QString &html, int maxImageWidth,
                                               ExternalImageHash *externalImageHash) {
    static const QRegularExpression re(QStringLiteral(R"(<img src=\"(https?:\/\/.+)\".*\/?>)"),
                                       QRegularExpression::CaseInsensitiveOption |
                                           QRegularExpression::MultilineOption |
                                           QRegularExpression::InvertedGreedinessOption);
    QRegularExpressionMatchIterator i = re.globalMatch(html);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString imageTag = match.captured(0);
        QString inlineImageTag;
        int imageWidth = maxImageWidth;
        ExternalImageHashItem hashItem;

        if (externalImageHash->contains(imageTag)) {
            hashItem = externalImageHash->value(imageTag);
            inlineImageTag = hashItem.imageTag;
            imageWidth = hashItem.imageWidth;
        } else {
            inlineImageTag = remotePreviewImageTagToInlineImageTag(imageTag, imageWidth);
            hashItem.imageTag = inlineImageTag;
            hashItem.imageWidth = imageWidth;
            externalImageHash->insert(imageTag, hashItem);
        }

        imageWidth = std::min(maxImageWidth, imageWidth);
        inlineImageTag.replace(">",
                               QStringLiteral("width=\"%1\">").arg(QString::number(imageWidth)));
        html.replace(imageTag, inlineImageTag);
    }
}

/**
 * Transforms a remote preview image tag to an inline image tag
 *
 * @param data
 * @param imageSuffix
 * @return
 */
QString Utils::Misc::remotePreviewImageTagToInlineImageTag(QString imageTag, int &imageWidth) {
    imageTag.replace(QStringLiteral("&amp;"), QStringLiteral("&"));

    static const QRegularExpression re(
        QStringLiteral(R"(<img src=\"(https?:\/\/.+)\")"),
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::InvertedGreedinessOption);

    QRegularExpressionMatch match = re.match(imageTag);
    if (!match.hasMatch()) {
        return imageTag;
    }

    const QString url = match.captured(1);
    const QByteArray data = downloadUrl(url);
    auto image = QImage::fromData(data);
    imageWidth = image.width();
    const QMimeDatabase db;
    const auto type = db.mimeTypeForData(data);

    // for now we do no caching, because we don't know when to invalidate the
    // cache
    const QString replace =
        QStringLiteral("data:") % type.name() % QStringLiteral(";base64,") % data.toBase64();
    return imageTag.replace(url, replace);
}

QString Utils::Misc::createUuidString() {
    const QUuid uuid = QUuid::createUuid();
    QString uuidString = uuid.toString();
    uuidString.replace(QStringLiteral("{"), QString()).replace(QStringLiteral("}"), QString());

    return uuidString;
}

/**
 * Returns the path where the local dictionaries will be stored
 *
 * @return
 */
QString Utils::Misc::localDictionariesPath() {
    QString path = Utils::Misc::appDataPath() % QStringLiteral("/dicts");
    QDir dir;

    // create path if it doesn't exist yet
    dir.mkpath(path);
    return path;
}

/**
 * Generates a SHA1 signature for a file
 *
 * @return
 */
QByteArray Utils::Misc::generateFileSha1Signature(const QString &path) {
    QCryptographicHash hash(QCryptographicHash::Sha1);
    QFile file(path);

    if (file.open(QIODevice::ReadOnly)) {
        hash.addData(file.readAll());
    } else {
        return QByteArray();
    }

    // retrieve the SHA1 signature of the file
    return hash.result();
}

/**
 * Checks if two files are the same
 *
 * @return
 */
bool Utils::Misc::isSameFile(const QString &path1, const QString &path2) {
    return generateFileSha1Signature(path1) == generateFileSha1Signature(path2);
}

QString Utils::Misc::generateRandomString(int length) {
    const QString possibleCharacters(
        QStringLiteral("ABCDEFGHKLMNPQRSTUVWXYZabcdefghkmnpqrstuvwxyz23456789"));

    QString randomString;
    for (int i = 0; i < length; ++i) {
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
        const int index = qrand() % possibleCharacters.length();
#else
        const quint32 index = QRandomGenerator::global()->generate() % possibleCharacters.length();
#endif
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }

    return randomString;
}

QString Utils::Misc::makeFileNameRandom(const QString &fileName, const QString &overrideSuffix) {
    const QFileInfo fileInfo(fileName);

    QString baseName = fileInfo.baseName()
                           .remove(QRegularExpression(QStringLiteral(R"([^\w\d\-_ ])")))
                           .replace(QChar(' '), QChar('-'));
    baseName.truncate(32);

    // find a more random name for the file
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
    const quint32 number = qrand();
#else
    const quint32 number = QRandomGenerator::global()->generate();
#endif

    const QString &suffix = overrideSuffix.isEmpty() ? fileInfo.suffix() : overrideSuffix;
    QString randomName = baseName + QChar('-') + QString::number(number);

    if (!suffix.isEmpty()) {
        randomName += QChar('.') + suffix;
    }

    return randomName;
}

/**
 * Returns a non-existing filename (like "my-image.jpg") in directoryPath
 * If the filename is already taken a number will be appended (like "my-image-1.jpg")
 */
QString Utils::Misc::findAvailableFileName(const QString &filePath, const QString &directoryPath,
                                           const QString &overrideSuffix) {
    const QFileInfo fileInfo(filePath);
    QString baseName = fileInfo.baseName();
    baseName.truncate(200);
    const QString newSuffix = fileInfo.suffix();
    QString newBaseName = baseName;
    QString newFileName = newBaseName;

    if (!newSuffix.isEmpty()) {
        newFileName += QChar('.') + newSuffix;
    }

    QString newFilePath = directoryPath + QDir::separator() + newFileName;
    QFile file(newFilePath);
    int nameCount = 0;

    // Check if a file with this filename already exists
    while (file.exists()) {
        // Find a new filename for the file
        newBaseName = baseName + QStringLiteral("-") + QString::number(++nameCount);
        newFileName = newBaseName;

        if (!newSuffix.isEmpty()) {
            newFileName += QChar('.') + newSuffix;
        }

        newFilePath = directoryPath + QDir::separator() + newFileName;
        file.setFileName(newFilePath);

        if (nameCount > 1000) {
            newFileName = makeFileNameRandom(filePath, overrideSuffix);
            break;
        }
    }

    return newFileName;
}

/**
 * Returns filename for a file path for inserting images
 */
QString Utils::Misc::fileNameForPath(const QString &filePath) {
    const QFileInfo fileInfo(filePath);
    QString baseName = fileInfo.fileName();
    baseName.truncate(200);

    return baseName;
}

/**
 * Checks if a filename exists like for findAvailableFileName
 */
bool Utils::Misc::fileNameExists(const QString &fileName, const QString &directoryPath) {
    QString filePath = directoryPath + QDir::separator() + fileNameForPath(fileName);
    QFile file(filePath);

    return file.exists();
}

/**
 * Strips all trailing spaces from str and returns the stripped text
 *
 * @param str
 * @return
 */
QString Utils::Misc::rstrip(const QString &str) {
    int n = str.size() - 1;

    for (; n >= 0; --n) {
        if (!str.at(n).isSpace()) {
            return str.left(n + 1);
        }
    }

    return "";
}

/**
 * Checks if file exists in the filesystem and is readable
 *
 * @return bool
 */
bool Utils::Misc::fileExists(const QString &path) {
    const QFile file(path);
    const QFileInfo fileInfo(file);
    return file.exists() && fileInfo.isFile() && fileInfo.isReadable();
}

static QString removeReducedCJKAccMark(const QString &label, int pos) {
    if (pos > 0 && pos + 1 < label.length() && label[pos - 1] == QLatin1Char('(') &&
        label[pos + 1] == QLatin1Char(')') && label[pos].isLetterOrNumber()) {
        // Check if at start or end, ignoring non-alphanumerics.
        int len = label.length();
        int p1 = pos - 2;
        while (p1 >= 0 && !label[p1].isLetterOrNumber()) {
            --p1;
        }
        ++p1;
        int p2 = pos + 2;
        while (p2 < len && !label[p2].isLetterOrNumber()) {
            ++p2;
        }
        --p2;

        if (p1 == 0) {
            return label.left(pos - 1) + label.mid(p2 + 1);
        } else if (p2 + 1 == len) {
            return label.left(p1) + label.mid(pos + 2);
        }
    }
    return label;
}

/**
 * @brief Removes accelarator markers from a label
 *
 * Taken from KDE's Ki18n library
 */
QString Utils::Misc::removeAcceleratorMarker(const QString &label_) {
    QString label = label_;

    int p = 0;
    bool accmarkRemoved = false;
    while (true) {
        p = label.indexOf(QLatin1Char('&'), p);
        if (p < 0 || p + 1 == label.length()) {
            break;
        }

        if (label[p + 1].isLetterOrNumber()) {
            // Valid accelerator.
            label = QString(label.left(p) + label.mid(p + 1));

            // May have been an accelerator in CJK-style "(&X)"
            // at the start or end of text.
            label = removeReducedCJKAccMark(label, p);

            accmarkRemoved = true;
        } else if (label[p + 1] == QLatin1Char('&')) {
            // Escaped accelerator marker.
            label = QString(label.left(p) + label.mid(p + 1));
        }

        ++p;
    }

    // If no marker was removed, and there are CJK characters in the label,
    // also try to remove reduced CJK marker -- something may have removed
    // ampersand beforehand.
    if (!accmarkRemoved) {
        bool hasCJK = false;
        for (const QChar c : asConst(label)) {
            if (c.unicode() >= 0x2e00) {    // rough, but should be sufficient
                hasCJK = true;
                break;
            }
        }
        if (hasCJK) {
            p = 0;
            while (true) {
                p = label.indexOf(QLatin1Char('('), p);
                if (p < 0) {
                    break;
                }
                label = removeReducedCJKAccMark(label, p + 1);
                ++p;
            }
        }
    }

    return label;
}

/**
 * Tries to find a suiting file extension for a mime type
 *
 * @param mimeType
 * @return
 */
QString Utils::Misc::fileExtensionForMimeType(const QString &mimeType) {
    if (mimeType == "image/jpg" || mimeType == "image/jpeg") {
        return "jpg";
    } else if (mimeType == "image/png") {
        return "png";
    } else if (mimeType == "image/gif") {
        return "gif";
    } else if (mimeType == "image/webp") {
        return "webp";
    } else if (mimeType == "application/pdf") {
        return "pdf";
    }

    return "";
}

void Utils::Misc::switchToDarkOrLightMode(bool darkMode) {
    SettingsService settings;
    settings.setValue("darkMode", darkMode);
    settings.setValue("darkModeColors", darkMode);
    settings.setValue("darkModeIconTheme", darkMode);
    settings.setValue("darkModeTrayIcon", darkMode);
    settings.setValue("Editor/CurrentSchemaKey",
                      darkMode ? "EditorColorSchema-cdbf28fc-1ddc-4d13-bb21-6a4043316a2f"
                               : "EditorColorSchema-6033d61b-cb96-46d5-a3a8-20d5172017eb");
}

void Utils::Misc::switchToDarkMode() { switchToDarkOrLightMode(true); }

void Utils::Misc::switchToLightMode() { switchToDarkOrLightMode(false); }

void Utils::Misc::unescapeEvernoteImportText(QString &content) {
    content.replace(QStringLiteral("\\\""), QStringLiteral("\""));

    // decode HTML entities
    content = Utils::Misc::unescapeHtml(std::move(content));
}

void Utils::Misc::transformEvernoteImportText(QString &content, bool withCleanup) {
    // add a newline in front of lists
    //            content.replace(QRegularExpression("<ul.*?>"),
    //            "\n<ul>");
    //            content.replace(QRegularExpression("<ol.*?>"),
    //            "\n<ol>");

    // remove web-clip code
    //            content.remove("<div
    //            style=\"-evernote-webclip:true;\">");

    // replace code blocks
    content.replace(
        QRegularExpression(
            QStringLiteral(
                R"(<div style="box-sizing.+?-en-codeblock:\s*true;"><div>(.+?)<\/div><\/div>)"),
            QRegularExpression::MultilineOption),
        QStringLiteral("\n```\n\\1\n```\n"));
    content.replace(
        QRegularExpression(
            QStringLiteral(R"(<div style="-en-codeblock:\s*true;.+?"><div>(.+?)<\/div><\/div>)"),
            QRegularExpression::MultilineOption),
        QStringLiteral("\n```\n\\1\n```\n"));

    // add a linebreak instead of div-containers
    content.replace(QRegularExpression(QStringLiteral("<\\/div>")), QStringLiteral("\n"));

    // preserve hyperlinks
    content.replace("<a ", "&lt;a ");
    content.replace("</a>", "&lt;/a&gt;");

    // convert remaining special characters
    content = Utils::Misc::unescapeHtml(std::move(content));

    // convert html tags to Markdown
    content = Utils::Misc::htmlToMarkdown(std::move(content));

    if (withCleanup) {
        cleanupEvernoteImportText(content);
    }
}

void Utils::Misc::cleanupEvernoteImportText(QString &content) {
    // remove all html tags
    content.remove(QRegularExpression(QStringLiteral("<.+?>")));

    // remove multiple \n
    content.replace(QRegularExpression(QStringLiteral("\n\n+")), QLatin1String("\n\n"));
    content.replace(QRegularExpression(QStringLiteral("\n\n\\s+")), QLatin1String("\n\n"));
}

QString Utils::Misc::testEvernoteImportText(const QString &data) {
    QXmlStreamReader xml(data);

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();

        // If token is just StartDocument, we'll go to next.
        if (token == QXmlStreamReader::StartDocument) {
            continue;
        }

        // If token is StartElement, we'll see if we can read it.
        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == QStringLiteral("en-export")) {
                continue;
            }

            if (xml.name() == QStringLiteral("note")) {
                // Let's check that we're really getting a note.
                if (xml.tokenType() != QXmlStreamReader::StartElement &&
                    xml.name() == QStringLiteral("note")) {
                    return {};
                }

                QString content;
                xml.readNext();

                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == QStringLiteral("note"))) {
                    if (xml.tokenType() == QXmlStreamReader::StartElement) {
                        if (xml.name() == QStringLiteral("content")) {
                            xml.readNext();
                            content = xml.text().toString().trimmed();
                            Utils::Misc::transformEvernoteImportText(content, true);

                            return content.trimmed();
                        }
                    }

                    xml.readNext();
                }
            }
        }
    }

    return {};
}

/**
 * Logs to the log file if allowed
 *
 * @param msgType
 * @param msg
 */
void Utils::Misc::logToFileIfAllowed(QtMsgType msgType, const QString &msg) {
    if (!SettingsService().value(QStringLiteral("Debug/fileLogging")).toBool()) {
        return;
    }

    QFile logFile(Utils::Misc::logFilePath());

    if (logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        QTextStream out(&logFile);
        QDateTime dateTime = QDateTime::currentDateTime();
        QString typeStr = logMsgTypeText(msgType);
        QString text = QStringLiteral("[%1] [%2]: %3\n")
                           .arg(dateTime.toString(QStringLiteral("MMM dd hh:mm:ss"))
                                    .remove(QStringLiteral(".")),
                                typeStr, msg);
        out << text;
        logFile.close();
    }
}

/**
 * Returns the text for a LogType
 *
 * @param logType
 * @return
 */
QString Utils::Misc::logMsgTypeText(QtMsgType logType) {
    QString type;

    switch (logType) {
        case QtMsgType::QtDebugMsg:
            type = QStringLiteral("Debug");
            break;
        case QtMsgType::QtInfoMsg:
            type = QStringLiteral("Info");
            break;
        case QtMsgType::QtWarningMsg:
            type = QStringLiteral("Warning");
            break;
        case QtMsgType::QtCriticalMsg:
            type = QStringLiteral("Critical");
            break;
        case QtMsgType::QtFatalMsg:
            type = QStringLiteral("Fatal");
            break;
        default:
            type = QStringLiteral("Unknown");
            break;
    }

    return type;
}

/**
 * Borrowed from
 * https://nasauber.de/blog/2019/levenshtein-distance-and-longest-common-subsequence-in-qt/
 *
 * @param source
 * @param target
 * @return
 */
int levenshteinDistance(const QString &source, const QString &target) {
    if (source == target) {
        return 0;
    }

    const int sourceCount = source.size();
    const int targetCount = target.size();

    if (source.isEmpty()) {
        return targetCount;
    }

    if (target.isEmpty()) {
        return sourceCount;
    }

    if (sourceCount > targetCount) {
        return levenshteinDistance(target, source);
    }

    QVector<int> column;
    column.fill(0, targetCount + 1);
    QVector<int> previousColumn;
    previousColumn.reserve(targetCount + 1);
    for (int i = 0; i < targetCount + 1; i++) {
        previousColumn.append(i);
    }

    for (int i = 0; i < sourceCount; i++) {
        column[0] = i + 1;
        for (int j = 0; j < targetCount; j++) {
            column[j + 1] =
                std::min<int>({1 + column.at(j), 1 + previousColumn.at(1 + j),
                               previousColumn.at(j) + ((source.at(i) == target.at(j)) ? 0 : 1)});
        }
        column.swap(previousColumn);
    }

    return previousColumn.at(targetCount);
}

/**
 * Returns if two strings are equal or similar
 *
 * @param str1
 * @param str2
 * @param threshold
 * @return
 */
bool Utils::Misc::isSimilar(const QString &str1, const QString &str2, int threshold) {
    if (str1 == str2) {
        return true;
    }

    if (threshold == 0) {
        return false;
    }

    // Checks if one string contains the other and just a few characters are different
    if ((str1.contains(str2) || str2.contains(str1)) &&
        (abs(str1.length() - str2.length()) <= threshold)) {
        return true;
    }

    // Use levenshtein for not too large notes if the middle part of the strings is different
    // Disabled, because it takes far too long!
    //    if ((str1.length() < 5000) && (levenshteinDistance(str1, str2) <= threshold)) {
    //        return true;
    //    }

    if (str1.length() != str2.length()) {
        return false;
    }

    int diff = 0;
    for (int i = 0; i < str1.length(); i++) {
        // QChar provides a 16-bit Unicode character, so Emojis will not be detected
        if (str1[i] != str2[i]) {
            diff++;
        }
    }

    return diff <= threshold;
}

/**
 * Returns the base url from a string
 * e.g. for "https://www.domain.com/test/page.html" it returns "https://www.domain.com"
 *
 * with withBasePath == true:
 * e.g. for "https://www.domain.com/test/page.html" it returns "https://www.domain.com/test/"
 */
QString Utils::Misc::getBaseUrlFromUrlString(const QString &urlString, bool withBasePath) {
    QUrl url(urlString);
    QString result = url.scheme() + QStringLiteral("://") + url.host();

    if (url.port() != -1) {
        result += QStringLiteral(":") + QString::number(url.port());
    }

    if (withBasePath) {
        result += url.path();

        // cut everything after the last slash
        result = result.left(result.lastIndexOf(QStringLiteral("/")) + 1);
    }

    return result;
}

/**
 * Attempts to transform links to the same webpage to absolute links with url
 * So for an url: https://www.example.com/path/to/file.html
 * This: <a href="/absolute.html">Link</a> <a href="relative.html">Link</a>
 * Would get: <a href="https://www.example.com/absolute.html">Link</a> <a
 * href="https://www.example.com/path/to/relative.html">Link</a>
 *
 * @param html
 * @param url
 * @return
 */
QString Utils::Misc::createAbsolutePathsInHtml(const QString &html, const QString &url) {
    const QString baseUrl = getBaseUrlFromUrlString(url);
    const QString baseUrlWithBasePath = getBaseUrlFromUrlString(url, true);
    QString result = html;

    QRegularExpression regex("(href|src)=\"(?!http)([^\"]+)\"");
    QRegularExpressionMatchIterator i = regex.globalMatch(html);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        const QString attribute = match.captured(1);
        const QString value = match.captured(2);
        const QString replaceUrl = value.startsWith('/') ? baseUrl : baseUrlWithBasePath;
        const QString newValue = QStringLiteral("%1=\"%2\"").arg(attribute, replaceUrl + value);

        result.replace(match.captured(0), newValue);
    }

    return result;
}

int Utils::Misc::getPreviewRefreshDebounceTime() {
    return SettingsService()
        .value(QStringLiteral("MainWindow/noteTextView.refreshDebounceTime"), 600)
        .toInt();
}

int Utils::Misc::getMaximumNoteFileSize() {
    return SettingsService().value(QStringLiteral("maxNoteFileSize"), 1048576).toInt();
}

/**
 * Percent encode each individual segment of a file path
 * @param filePath
 * @return
 */
QString Utils::Misc::encodeFilePath(const QString &filePath) {
    // Split the path into segments to preserve the directory separators
    QStringList segments = filePath.split('/');

    // URL encode each segment individually
    for (int i = 0; i < segments.size(); ++i) {
        // Use QUrl::toPercentEncoding to properly encode special characters
        segments[i] = QUrl::toPercentEncoding(segments[i], "/");
    }

    // Join the segments back together with forward slashes
    return segments.join('/');
}

QString Utils::Misc::detectFileFormat(const QString &text) {
    // Static regular expressions for format detection
    static const QRegularExpression xmlRegex(
        R"(^\s*(<\?xml\s+version\s*=\s*['"][0-9.]+['"](?:\s+encoding\s*=\s*['"][^'"]+['"])?(?:\s+standalone\s*=\s*['"](?:yes|no)['"])?\s*\?>|<[a-zA-Z][a-zA-Z0-9_\:-]*(?:\s+[a-zA-Z_][a-zA-Z0-9_\:-]*(?:\s*=\s*(?:"[^"]*"|'[^']*'))?)*\s*/?>))",
        QRegularExpression::DotMatchesEverythingOption);

    static const QRegularExpression htmlRegex(
        R"(^\s*(?:<!DOCTYPE\s+html[^>]*>|<html\b[^>]*>))",
        QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption);

    static const QRegularExpression cppRegex(
        R"(^\s*(?:#include\s*(?:<[^>]+>|"[^"]+")|#define\s+\w+\s|#pragma\s+\w+|#ifndef\s+\w+|using\s+namespace\s+\w+\s*;|class\s+\w+\s*[{:]\s|namespace\s+\w+\s*\{))",
        QRegularExpression::MultilineOption);

    static const QRegularExpression nixRegex(
        R"(^\s*(?:\{\s*pkgs\s*(?:\?|=)\s*import\s+<nixpkgs>\s*\{\}|with\s+pkgs\s*;|stdenv\.mkDerivation\s+\{))",
        QRegularExpression::MultilineOption);

    static const QRegularExpression sqlRegex(
        R"(^\s*(?:SELECT|INSERT|UPDATE|DELETE|CREATE|ALTER|DROP|WITH)\s+)",
        QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption);

    static const QRegularExpression pythonRegex(
        R"(^\s*(?:import\s+\w+\s*(?:,\s*\w+)*\s*(?:;|$)|from\s+\w+\s+import\s+\w+|def\s+\w+\s*\([^)]*\)\s*:|class\s+\w+(?:\s*\([\w\s,]*\))?\s*:))",
        QRegularExpression::MultilineOption);

    static const QRegularExpression javascriptRegex(
        R"(^\s*(?:function\s+\w+\s*\([^)]*\)\s*\{|const\s+\w+\s*=\s*(?:[^;]+;|\{)|let\s+\w+\s*=\s*(?:[^;]+;|\{)|var\s+\w+\s*=\s*(?:[^;]+;|\{)|import\s+(?:\{[^}]*\}\s+from\s+)?['"][^'"]+['"]))",
        QRegularExpression::MultilineOption);

    static const QRegularExpression iniRegex(
        R"(^\s*(?:(?:\[[\w\s\-\.]+\]\s*$)|(?:[\w\.\-]+\s*=\s*[^\r\n]*$)|(?:;[^\r\n]*$)))",
        QRegularExpression::MultilineOption);

    static const QRegularExpression yamlRegex(
        R"(^\s*(?:---\s*$|(?:[a-zA-Z0-9_-]+\s*:\s*(?:(?:[^#\r\n]*)?$|(?:\r?\n\s+[^\s#-][^\r\n]*)+))|-\s+[^\s#][^\r\n]*(?:\r?\n\s+[^\s#-][^\r\n]*)*$))",
        QRegularExpression::MultilineOption);

    static const QRegularExpression jsonRegex(R"(^\s*(\{[\s\S]*\}|\[[\s\S]*\])\s*$)",
                                              QRegularExpression::MultilineOption);

    static const QRegularExpression csvRegex(
        R"(^(?:[^=\r\n"]*|"(?:[^"]|"")*")(?:,(?:[^=\r\n"]*|"(?:[^"]|"")*"))+(?:\r?\n|$)(?:(?:[^=\r\n"]*|"(?:[^"]|"")*")(?:,(?:[^=\r\n"]*|"(?:[^"]|"")*"))+(?:\r?\n|$))*)",
        QRegularExpression::MultilineOption);

    // XML detection (specific, check early)
    if (xmlRegex.globalMatch(text).hasNext()) {
        return "xml";
    }

    // HTML detection (specific, check early)
    if (htmlRegex.globalMatch(text).hasNext()) {
        return "html";
    }

    // Nix detection (specific Nix syntax)
    if (nixRegex.globalMatch(text).hasNext()) {
        return "nix";
    }

    // SQL detection (SQL keywords)
    if (sqlRegex.globalMatch(text).hasNext()) {
        return "sql";
    }

    // JavaScript detection (JS-specific constructs)
    if (javascriptRegex.globalMatch(text).hasNext()) {
        return "js";
    }

    // INI detection (section headers or key-value pairs)
    if (iniRegex.globalMatch(text).hasNext()) {
        return "ini";
    }

    // YAML detection (key-value pairs or list items)
    if (yamlRegex.globalMatch(text).hasNext()) {
        return "yaml";
    }

    // JSON detection (object or array structure)
    if (jsonRegex.globalMatch(text).hasNext()) {
        return "json";
    }

    // CSV detection (comma-separated values with multiple lines)
    if (csvRegex.globalMatch(text).hasNext()) {
        return "csv";
    }

    // Default to plain text if no format is detected
    return "txt";
}

QString Utils::Misc::jsValueToJsonString(const QJSValue &value) {
    QVariant variant = value.toVariant();
    QJsonDocument doc = QJsonDocument::fromVariant(variant);
    return doc.toJson(QJsonDocument::Indented);
}

/**
 * Get the correct file open flags for reading note files
 * This ensures consistent newline handling across platforms, especially on Windows
 *
 * @param baseFlags - The base open mode flags (default: QIODevice::ReadOnly)
 * @return The complete open mode flags with Text flag added if useUNIXNewline is disabled
 */
QFile::OpenMode Utils::Misc::getNoteFileOpenFlags(QFile::OpenMode baseFlags) {
    const SettingsService settings;
    const bool useUNIXNewline = settings.value(QStringLiteral("useUNIXNewline")).toBool();

    QFile::OpenMode flags = baseFlags;
    if (!useUNIXNewline) {
        flags |= QIODevice::Text;
    }

    return flags;
}
