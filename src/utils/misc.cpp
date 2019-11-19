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

#include <QProcess>
#include <QDesktopServices>
#include <QDir>
#include <QUrl>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>
#include <QTime>
#include <QCoreApplication>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QSettings>
#include <QTemporaryFile>
#include <services/databaseservice.h>
#include <entities/note.h>
#include <entities/notefolder.h>
#include <entities/script.h>
#include <QtGui/QIcon>
#include <services/updateservice.h>
#include "build_number.h"
#include "version.h"
#include "release.h"
#include "misc.h"
#include <utility>
#include <services/owncloudservice.h>
#include <QMimeDatabase>
#include <QUuid>
#include <QTextDocument>

#ifdef Q_OS_WIN
#include <windows.h>
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
void Utils::Misc::openPath(const QString& absolutePath)
{
    const QString path = QDir::fromNativeSeparators(absolutePath);
    // Hack to access samba shares with QDesktopServices::openUrl
    if (path.startsWith("//"))
        QDesktopServices::openUrl(QDir::toNativeSeparators("file:" + path));
    else
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

/**
 * Opens the parent directory of the given path with a file manager and select
 * (if possible) the item at the given path
 * (thank you to qBittorrent for the inspiration)
 */
void Utils::Misc::openFolderSelect(const QString& absolutePath)
{
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

        QString cmd = QString("explorer.exe /select,\"%1\"")
            .arg(QDir::toNativeSeparators(absolutePath));
        LPWSTR lpCmd = new WCHAR[cmd.size() + 1];
        cmd.toWCharArray(lpCmd);
        lpCmd[cmd.size()] = 0;

        bool ret = ::CreateProcessW(
            NULL, lpCmd, NULL, NULL, FALSE, 0, NULL, NULL,
            &startupInfo, &processInfo);
        delete [] lpCmd;

        if (ret) {
            ::CloseHandle(processInfo.hProcess);
            ::CloseHandle(processInfo.hThread);
        }
    } else {
        // If the item to select doesn't exist, try to open its parent
        openPath(path.left(path.lastIndexOf("/")));
    }
#elif defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
    if (QFileInfo(path).exists()) {
        QProcess proc;
        QString output;
        proc.start(
                "xdg-mime",
                QStringList() << "query" << "default" << "inode/directory");
        proc.waitForFinished();
        output = proc.readLine().simplified();
        if (output == "dolphin.desktop" ||
                output == "org.kde.dolphin.desktop") {
            proc.startDetached(
                    "dolphin",
                    QStringList() << "--select"
                    << QDir::toNativeSeparators(path));
        } else if (output == "nautilus.desktop" ||
                    output == "org.gnome.Nautilus.desktop" ||
                    output == "nautilus-folder-handler.desktop") {
            proc.startDetached(
                    "nautilus",
                    QStringList() << "--no-desktop"
                    << QDir::toNativeSeparators(path));
        } else if (output == "caja-folder-handler.desktop") {
            proc.startDetached(
                    "caja",
                    QStringList() << "--no-desktop"
                    << QDir::toNativeSeparators(path));
        } else if (output == "nemo.desktop") {
            proc.startDetached(
                    "nemo",
                    QStringList() << "--no-desktop"
                    << QDir::toNativeSeparators(path));
        } else if (output == "konqueror.desktop" ||
                   output == "kfmclient_dir.desktop") {
            proc.startDetached(
                    "konqueror",
                    QStringList() << "--select"
                    << QDir::toNativeSeparators(path));
        } else {
            openPath(path.left(path.lastIndexOf("/")));
        }
    } else {
        // if the item to select doesn't exist, try to open its parent
        openPath(path.left(path.lastIndexOf("/")));
    }
#else
    openPath(path.left(path.lastIndexOf("/")));
#endif
}

/**
 * Removes a string from the start if it starts with it
 */
QString Utils::Misc::removeIfStartsWith(QString text, const QString& removeString) {
    if (text.startsWith(removeString)) {
        text.remove(QRegularExpression(
                "^" + QRegularExpression::escape(removeString)));
    }

    return text;
}

/**
 * Removes a string from the end if it ends with it
 */
QString Utils::Misc::removeIfEndsWith(QString text, const QString& removeString) {
    if (text.endsWith(removeString)) {
        text.remove(QRegularExpression(
                QRegularExpression::escape(removeString) + "$"));
    }

    return text;
}

/**
 * Adds a string to the beginning of a string if it doesn't start with it
 */
QString Utils::Misc::prependIfDoesNotStartWith(QString text, const QString& startString) {
    if (!text.startsWith(startString)) {
        text.prepend(startString);
    }

    return text;
}

/**
 * Adds a string to the end of a string if it doesn't end with it
 */
QString Utils::Misc::appendIfDoesNotEndWith(QString text, const QString& endString) {
    if (!text.endsWith(endString)) {
        text.append(endString);
    }

    return text;
}

/**
 * Shortens text and adds a sequence string if text is too long
 */
QString Utils::Misc::shorten(QString text, int length, const QString& sequence) {
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
QString Utils::Misc::cycleTextCase(const QString& text) {
    QString asLower = text.toLower();
    QString asUpper = text.toUpper();

    // OK no matter what
    if (text == asLower) {
        return asUpper;
    }

    QString asStart = toStartCase(text);
    QString asSentence = toSentenceCase(text);

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
QString Utils::Misc::toSentenceCase(const QString& text) {
    // A sentence is a string of characters immediately preceded by:
    // (beginning of string followed by any amount of horizontal or vertical
    //     whitespace) or
    // (any of [.?!] followed by at least one horizontal or vertical
    //     whitespace)
    QRegularExpression sentenceSplitter(R"((^[\s\v]*|[.?!][\s\v]+)\K)");

    QStringList sentences = text.toLower().split(sentenceSplitter);

    for (QString & sentence : sentences) {
        if (sentence.length() > 0) {
            sentence = sentence.left(1).toUpper() +
                    sentence.right(sentence.length() - 1);
        }
    }

    return sentences.join("");
}

/**
 * Converts text to start case
 */
QString Utils::Misc::toStartCase(const QString& text) {
    // A word is a string of characters immediately preceded by horizontal or
    // vertical whitespace
    QRegularExpression wordSplitter("(?<=[\\s\\v])");

    QStringList words = text.toLower().split(wordSplitter);

    for (QString & word : words) {
        if (word.length() > 0) {
            word = word.left(1).toUpper() + word.right(word.length() - 1);
        }
    }

    return words.join("");
}

/**
 * Starts an executable detached with parameters
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param workingDirectory the directory to run the executable from
 * @return true on success, false otherwise
 */
bool Utils::Misc::startDetachedProcess(const QString& executablePath,
                                       const QStringList& parameters,
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
    return process.startDetached(
            "open",
            QStringList() << executablePath << "--args" << parameters,
            workingDirectory);
#else
    return process.startDetached(executablePath, parameters, workingDirectory);
#endif
}

/**
 * Starts an executable synchronous with parameters
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param data the data that will be written to the process
 * @return the text that was returned by the process
 */
QByteArray Utils::Misc::startSynchronousProcess(
        const QString& executablePath, QStringList parameters, QByteArray data) {
    QProcess process;

    // start executablePath synchronous with parameters
#ifdef Q_OS_MAC
    process.start(
        "open", QStringList() << executablePath << "--args" << parameters);
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

    QByteArray result = process.readAll();
    return result;
}

/**
 * Returns the default notes path we are suggesting
 *
 * @return
 */
QString Utils::Misc::defaultNotesPath() {
    // it seems QDir::separator() is not needed,
    // because Windows also uses "/" in QDir::homePath()
    QString path = isInPortableMode() ?
                   portableDataPath() :
                   QDir::homePath() + Utils::Misc::dirSeparator() + "ownCloud";

    // check if the ownCloud path already exists (because the user is using
    // ownCloud), if not use Nextcloud
    if (!isInPortableMode()) {
        // we don't want to use the regular sync path for debugging
#ifdef QT_DEBUG
        path.append(".test");
#endif

        QDir dir(path);
        if (!dir.exists()) {
            path = QDir::homePath() + Utils::Misc::dirSeparator() + "Nextcloud";
        }
    }

    path += Utils::Misc::dirSeparator() + "Notes";

    // remove the snap path for Snapcraft builds
    path.remove(QRegularExpression(R"(snap\/qownnotes\/\w\d+\/)"));

    return path;
}

/**
 * Returns the directory separator
 * Replaces QDir::separator() because it seems methods like
 * QDir::homePath() are using "/" under Windows too
 *
 * @return
 */
QString Utils::Misc::dirSeparator() {
    return "/";
}

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
    QString path = "";

    if (qApp != Q_NULLPTR) {
        path = QCoreApplication::applicationDirPath();
    }

    // use a fallback if the QApplication object wasn't instantiated yet
    if (path.isEmpty()) {
        path = ".";
    }

    // it seems QDir::separator() is not needed, because Windows also
    // uses "/" in QCoreApplication::applicationDirPath()
    path += dirSeparator() + "Data";

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
 * Prepends the portable data path if we are in portable mode
 *
 * @param path
 * @return
 */
QString Utils::Misc::prependPortableDataPathIfNeeded(QString path,
                                                     bool ifNotEmptyOnly) {
    if (ifNotEmptyOnly && path.isEmpty()) {
        return "";
    }

    if (isInPortableMode()) {
        QString portableDataPath(Utils::Misc::portableDataPath());

        // check if the path already starts with the portable data path
        if (!path.startsWith(portableDataPath)) {
            path = portableDataPath + "/" + path;
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
 * Converts html tags to markdown
 *
 * @param text
 * @return markdown text
 */
QString Utils::Misc::htmlToMarkdown(QString text) {
    // replace Windows line breaks
    text.replace(QRegularExpression("\r\n"), "\n");

    // remove all null characters
    // we can get those from Google Chrome via the clipboard
    text.remove(QChar(0));

    // remove some blocks
    text.remove(QRegularExpression(
            "<head.*?>(.+?)<\\/head>",
            QRegularExpression::CaseInsensitiveOption |
                    QRegularExpression::DotMatchesEverythingOption));

    text.remove(QRegularExpression(
            "<script.*?>(.+?)<\\/script>",
            QRegularExpression::CaseInsensitiveOption |
                    QRegularExpression::DotMatchesEverythingOption));

    text.remove(QRegularExpression(
            "<style.*?>(.+?)<\\/style>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption));

    // replace some html tags with markdown
    text.replace(QRegularExpression(
            "<strong.*?>(.+?)<\\/strong>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "**\\1**");
    text.replace(QRegularExpression(
            "<b.*?>(.+?)<\\/b>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "**\\1**");
    text.replace(QRegularExpression(
            "<em.*?>(.+?)<\\/em>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "*\\1*");
    text.replace(QRegularExpression(
            "<i.*?>(.+?)<\\/i>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "*\\1*");
    text.replace(QRegularExpression(
            "<pre.*?>(.+?)<\\/pre>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption),
                 "\n```\n\\1\n```\n");
    text.replace(QRegularExpression(
            "<code.*?>(.+?)<\\/code>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption),
                 "\n```\n\\1\n```\n");
    text.replace(QRegularExpression(
            "<h1.*?>(.+?)<\\/h1>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "\n# \\1\n");
    text.replace(QRegularExpression(
            "<h2.*?>(.+?)<\\/h2>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "\n## \\1\n");
    text.replace(QRegularExpression(
            "<h3.*?>(.+?)<\\/h3>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "\n### \\1\n");
    text.replace(QRegularExpression(
            "<h4.*?>(.+?)<\\/h4>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "\n#### \\1\n");
    text.replace(QRegularExpression(
            "<h5.*?>(.+?)<\\/h5>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "\n##### \\1\n");
    text.replace(QRegularExpression(
            "<li.*?>(.+?)<\\/li>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "- \\1");
    text.replace(QRegularExpression(
            "<br.*?>",
            QRegularExpression::CaseInsensitiveOption), "\n");
    text.replace(QRegularExpression(
            "<a[^>]+href=\"(.+?)\".*?>(.+?)<\\/a>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "[\\2](\\1)");
    text.replace(QRegularExpression(
            "<p.*?>(.+?)</p>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "\n\n\\1\n\n");

    // replace multiple line breaks
    text.replace(QRegularExpression("\n\n+"), "\n\n");

    return text;
}

/**
 * Returns new html with task lists by checkboxes
 *
 * @param html
 * @param clickable
 * @return
 */
QString Utils::Misc::parseTaskList(const QString &html, bool clickable)
{
    auto text = html;

    if (!clickable) {
        text.replace(QRegExp(R"((<li>\s*(<p>)*\s*)\[ ?\])", Qt::CaseInsensitive), "\\1&#9744;");
        text.replace(QRegExp(R"((<li>\s*(<p>)*\s*)\[[xX]\])", Qt::CaseInsensitive), "\\1&#9745;");
        return text;
    }

    // TODO
    // to ensure the clicking behavior of checkboxes,
    // line numbers of checkboxes in the original markdown text
    // should be provided by the markdown parser

    const QString checkboxStart = R"(<a class="task-list-item-checkbox" href="checkbox://_)";
    text.replace(QRegExp(R"((<li>\s*(<p>)*\s*)\[ ?\])", Qt::CaseInsensitive), "\\1" + checkboxStart + "\">&#9744;</a>");
    text.replace(QRegExp(R"((<li>\s*(<p>)*\s*)\[[xX]\])", Qt::CaseInsensitive), "\\1" + checkboxStart + "\">&#9745;</a>");

    int count = 0;
    int pos = 0;
    while (true) {
        pos = text.indexOf(checkboxStart + "\"", pos);
        if (pos == -1)
            break;

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

    if (parent != Q_NULLPTR) {
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
    QString path = "";

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
QString Utils::Misc::logFilePath() {
    return appDataPath() + "/" + qAppName().replace(" ", "-") + ".log";
}

/**
 * Transforms all line feeds to \n
 *
 * @param text
 * @return
 */
QString Utils::Misc::transformLineFeeds(QString text) {
    return text.replace(QRegExp(R"((\r\n)|(\n\r)|\r|\n)"), "\n");
}

/**
 * Replaces the text "ownCloud" by "ownCloud / Nextcloud"
 *
 * @param text
 * @param useShortText
 * @return
 */
QString Utils::Misc::replaceOwnCloudText(QString text, bool useShortText) {
    if (text.contains("Nextcloud")) {
        return text;
    }

    QString replaceText = useShortText ? "NC / oC" : "Nextcloud / ownCloud";
    return text.replace("ownCloud", replaceText, Qt::CaseInsensitive);
}

/**
 * Declares that we need a restart
 */
void Utils::Misc::needRestart() {
    qApp->setProperty("needsRestart", true);
}

/**
 * Restarts the application
 */
void Utils::Misc::restartApplication() {
    QStringList parameters = QApplication::arguments();
    QString appPath = parameters.takeFirst();

    // we don't want to have our settings cleared again after a restart
    parameters.removeOne("--clear-settings");

    startDetachedProcess(appPath, parameters);
    QApplication::quit();
}

/**
 * Downloads an url and returns the data
 *
 * @param url
 * @return {QByteArray} the content of the downloaded url
 */
QByteArray Utils::Misc::downloadUrl(const QUrl& url) {
    auto *manager = new QNetworkAccessManager();
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)),
                     &loop, SLOT(quit()));

    // 10 sec timeout for the request
    timer.start(10000);

    QNetworkRequest networkRequest = QNetworkRequest(url);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute,
                                true);
#endif

    QByteArray data;
    QNetworkReply *reply = manager->get(networkRequest);
    loop.exec();

    // if we didn't get a timeout let us return the content
    if (timer.isActive()) {
        int statusCode = reply->attribute(
                QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // only get the data if the status code was "success"
        // see: https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
        if (statusCode >= 200 && statusCode < 300) {
            // get the data from the network reply
            data = reply->readAll();
        }
    }

    reply->deleteLater();
    delete(manager);

    return data;
}

/**
 * Downloads an url and stores it to a file
 */
bool Utils::Misc::downloadUrlToFile(QUrl url, QFile *file) {
    if (!file->open(QIODevice::WriteOnly)) {
        return false;
    }

    if (!file->isWritable()) {
        return false;
    }

    QByteArray data = downloadUrl(std::move(url));
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
    QSettings settings;
    bool darkModeColors = settings.value("darkModeColors").toBool();
    QString color = darkModeColors ? "#ffd694" : "#fc7600";
    QString cssStyles = "a {color: " + color +  "}";

    color = darkModeColors ? "#5b5b5b" : "#e8e8e8";
    cssStyles += "kbd {background-color: " + color +  "}";

    color = darkModeColors ? "#336924" : "#d6ffc7";
    cssStyles += "ins {background-color: " + color +  "}";

    color = darkModeColors ? "#802c2c" : "#ffd7d7";
    cssStyles += "del {background-color: " + color +  "}";
    return cssStyles;
}

/**
 * A vector of pairs, the first item in the pair is the search
 * engine name and the second item is the search url of the engine.
 */
QHash<int, Utils::Misc::SearchEngine> Utils::Misc::getSearchEnginesHashMap() {
    QHash<int, Utils::Misc::SearchEngine> searchEngines;
    searchEngines.insert(SearchEngines::Google,
                         {"Google", "https://www.google.com/search?q=",
                          SearchEngines::Google});
    searchEngines.insert(SearchEngines::Bing,
                         {"Bing", "https://www.bing.com/search?q=",
                          SearchEngines::Bing});
    searchEngines.insert(SearchEngines::DuckDuckGo,
                         {"DuckDuckGo",
                          "https://duckduckgo.com/?t=qownnotes&q=",
                          SearchEngines::DuckDuckGo});
    searchEngines.insert(SearchEngines::Yahoo,
                         {"Yahoo", "https://search.yahoo.com/search?p=",
                          SearchEngines::Yahoo});
    searchEngines.insert(SearchEngines::GoogleScholar,
                         {"Google Scholar",
                          "https://scholar.google.co.il/scholar?q=",
                          SearchEngines::GoogleScholar});
    searchEngines.insert(SearchEngines::Yandex,
                         {"Yandex", "https://www.yandex.com/search/?text=",
                          SearchEngines::Yandex});
    searchEngines.insert(SearchEngines::AskDotCom,
                         {"Ask.com", "https://www.ask.com/web?q=",
                          SearchEngines::AskDotCom});
    searchEngines.insert(SearchEngines::Qwant,
                         {"Qwant", "https://www.qwant.com/?q=",
                          SearchEngines::Qwant});
    searchEngines.insert(SearchEngines::Startpage,
                         {"Startpage",
                          "https://www.startpage.com/do/dsearch?query=",
                          SearchEngines::Startpage});
    return searchEngines;
}

/**
 * Returns the default search engine id
 *
 * @return
 */
int Utils::Misc::getDefaultSearchEngineId() {
    return SearchEngines::DuckDuckGo;
}

/**
 * Returns a list of search engines in the order it should be displayed
 *
 * @return
 */
QList<int> Utils::Misc::getSearchEnginesIds() {
    QList<int> list;
    list << SearchEngines::DuckDuckGo << SearchEngines::Google
         << SearchEngines::Bing << SearchEngines::Yahoo
         << SearchEngines::GoogleScholar << SearchEngines::Yandex
         << SearchEngines::AskDotCom << SearchEngines::Qwant
         << SearchEngines::Startpage;
    return list;
}

/**
 * Disables the automatic update dialog per default for repositories and
 * self-builds if nothing is already set
 */
void Utils::Misc::presetDisableAutomaticUpdateDialog() {
    QSettings settings;

    // disable the automatic update dialog per default for repositories and
    // self-builds
    if (settings.value("disableAutomaticUpdateDialog").toString().isEmpty()) {
        QString release = qApp->property("release").toString();
        bool enabled = release.contains("Travis") ||
                       release.contains("AppVeyor") || release.contains("AppImage");
        settings.setValue("disableAutomaticUpdateDialog", !enabled);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Write all available Attributes from QPrinter into stream
///////////////////////////////////////////////////////////////////////////////

template <typename t> void  writeStreamElement(QDataStream &os, t param) {
    int i = static_cast<int>(param);
    os << i;
}

template <> void writeStreamElement<QString>(QDataStream &os, QString s) {
    os << s;
}

QDataStream& Utils::Misc::dataStreamWrite(QDataStream &os,
                                          const QPrinter &printer)
{
    writeStreamElement(os, printer.printerName         ());
    writeStreamElement(os, printer.pageSize            ());
    writeStreamElement(os, printer.collateCopies       ());
    writeStreamElement(os, printer.colorMode           ());
    writeStreamElement(os, printer.copyCount           ());
    writeStreamElement(os, printer.creator             ());
    writeStreamElement(os, printer.docName             ());
    writeStreamElement(os, printer.doubleSidedPrinting ());
    writeStreamElement(os, printer.duplex              ());
    writeStreamElement(os, printer.fontEmbeddingEnabled());
    writeStreamElement(os, printer.fullPage            ());
    writeStreamElement(os, printer.orientation         ());
    writeStreamElement(os, printer.outputFileName      ());
    writeStreamElement(os, printer.outputFormat        ());
    writeStreamElement(os, printer.pageOrder           ());
    writeStreamElement(os, printer.paperSize           ());
    writeStreamElement(os, printer.paperSource         ());
    writeStreamElement(os, printer.printProgram        ());
    writeStreamElement(os, printer.printRange          ());
    writeStreamElement(os, printer.printerName         ());
    writeStreamElement(os, printer.resolution          ());
    writeStreamElement(os, printer.winPageSize         ());

    qreal left, top, right, bottom;
    printer.getPageMargins(&left, &top, &right, &bottom, QPrinter::Millimeter);
    os << left << top << right << bottom;

    Q_ASSERT_X(os.status() == QDataStream::Ok, __FUNCTION__,
               QString("Stream status = %1").arg(os.status()).toStdString().c_str());
    return os;
}


///////////////////////////////////////////////////////////////////////////////
// Read all available Attributes from stream into QPrinter
///////////////////////////////////////////////////////////////////////////////

template <typename t> t readStreamElement(QDataStream &is) {
    int i;
    is >> i;
    return static_cast<t>(i);
}

template <> QString readStreamElement<QString>(QDataStream &is) {
    QString s;
    is >> s;
    return s;
}

QDataStream& Utils::Misc::dataStreamRead(QDataStream &is, QPrinter &printer) {
    printer.setPrinterName              (readStreamElement<QString>                (is));
    printer.setPageSize                 (readStreamElement<QPrinter::PaperSize>    (is));
    printer.setCollateCopies            (readStreamElement<bool>                   (is));
    printer.setColorMode                (readStreamElement<QPrinter::ColorMode>    (is));
    printer.setCopyCount                (readStreamElement<int>                    (is));
    printer.setCreator                  (readStreamElement<QString>                (is));
    printer.setDocName                  (readStreamElement<QString>                (is));
    printer.setDoubleSidedPrinting      (readStreamElement<bool>                   (is));
    printer.setDuplex                   (readStreamElement<QPrinter::DuplexMode>   (is));
    printer.setFontEmbeddingEnabled     (readStreamElement<bool>                   (is));
    printer.setFullPage                 (readStreamElement<bool>                   (is));
    printer.setOrientation              (readStreamElement<QPrinter::Orientation>  (is));
    printer.setOutputFileName           (readStreamElement< QString >              (is));
    printer.setOutputFormat             (readStreamElement<QPrinter::OutputFormat> (is));
    printer.setPageOrder                (readStreamElement<QPrinter::PageOrder>    (is));
    printer.setPaperSize                (readStreamElement<QPrinter::PaperSize>    (is));
    printer.setPaperSource              (readStreamElement<QPrinter::PaperSource>  (is));
    printer.setPrintProgram             (readStreamElement<QString>                (is));
    printer.setPrintRange               (readStreamElement<QPrinter::PrintRange>   (is));
    printer.setPrinterName              (readStreamElement<QString>                (is));
    printer.setResolution               (readStreamElement<int>                    (is));
    printer.setWinPageSize              (readStreamElement<int>                    (is));

    qreal left, top, right, bottom;
    is >> left >> top >> right >> bottom;

    printer.setPageMargins(left, top, right, bottom, QPrinter::Millimeter);

    Q_ASSERT_X(is.status() == QDataStream::Ok, __FUNCTION__,
               QString("Stream status = %1").arg(is.status()).toStdString().c_str());

    return is;
}

/**
 * Stores the properties of a QPrinter to the settings
 *
 * @param printer
 * @param settingsKey
 */
void Utils::Misc::storePrinterSettings(QPrinter *printer, const QString& settingsKey) {
    QByteArray byteArr;
    QDataStream os(&byteArr, QIODevice::WriteOnly);
    dataStreamWrite(os, *printer);
    QSettings settings;
    settings.setValue(settingsKey, byteArr.toHex());
}

/**
 * Loads the properties of a QPrinter from the settings
 *
 * @param printer
 * @param settingsKey
 */
void Utils::Misc::loadPrinterSettings(QPrinter *printer, const QString& settingsKey) {
    QSettings settings;

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
    QSettings settings;
    return settings.value("allowNoteEditing", true).toBool();
}

/**
 * Returns if "allowNoteEditing" is turned on
 *
 * @return
 */
bool Utils::Misc::isSocketServerEnabled() {
    QSettings settings;
    return settings.value("enableSocketServer", true).toBool();
}

/**
 * Returns if "darkModeIconTheme" is turned on
 *
 * @return
 */
bool Utils::Misc::isDarkModeIconTheme() {
    QSettings settings;
    bool darkMode = settings.value("darkMode").toBool();
    return settings.value("darkModeIconTheme", darkMode).toBool();
}

/**
 * Returns if "automaticNoteFolderDatabaseClosing" is turned on
 *
 * @return
 */
bool Utils::Misc::doAutomaticNoteFolderDatabaseClosing() {
    QSettings settings;
    return settings.value("automaticNoteFolderDatabaseClosing").toBool();
}

/**
 * Returns if "noteListPreview" is turned on
 *
 * @return
 */
bool Utils::Misc::isNoteListPreview() {
    QSettings settings;
    return settings.value("noteListPreview").toBool();
}

/**
 * Returns if "enableNoteTree" is turned on
 *
 * @return
 */
bool Utils::Misc::isEnableNoteTree() {
    QSettings settings;
    return settings.value("enableNoteTree").toBool();
}

/**
 * Unescapes html special characters
 *
 * @param html
 * @return
 */
QString Utils::Misc::unescapeHtml(QString html) {
//    html.replace("&lt;","<");
//    html.replace("&gt;",">");
//    html.replace("&amp;","&");
//    return html;

    // text.toPlainText() will remove all line breaks, we don't want that
    html.replace("\n","<br>");

    QTextDocument text;
    text.setHtml(html);
    return text.toPlainText();
}

/**
 * Unescapes some html special characters
 *
 * @param text
 * @return
 */
QString Utils::Misc::htmlspecialchars(QString text) {
    text.replace("&", "&amp;");
    text.replace("\"", "&quot;");
    text.replace("'", "&apos;");
    text.replace("<", "&lt;");
    text.replace(">", "&gt;");
    return text;
}

/**
 * Outputs an info text
 *
 * @param text
 */
void Utils::Misc::printInfo(const QString& text) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    qInfo() << text;
#else
    printf("%s\n", text.toLatin1().data());
#endif
}

/**
 * Converts a byte size to a human readable size
 *
 * @param size
 * @return
 */
QString Utils::Misc::toHumanReadableByteSize(qint64 size)
{
    double num = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while (num >= 1024.0 && i.hasNext()) {
        unit = i.next();
        num /= 1024.0;
    }

    return QString().setNum(num, 'f', 2) + " " + unit;
}

/**
 * @brief Prepares the debug information to output it as markdown
 * @param headline
 * @param data
 */
QString Utils::Misc::prepareDebugInformationLine(
        const QString &headline, QString data, bool withGitHubLineBreaks,
        const QString& typeText) {
    // add two spaces if we don't want GitHub line breaks
    QString spaces = withGitHubLineBreaks ? "" : "  ";

    if (data.contains("\n")) {
        data = "\n```\n" + data.trimmed() + "\n```";
    } else {
        data = (data.isEmpty()) ? "*empty*" : "`" + data + "`";
    }

    QString resultText = "**" + headline + "**";

    if (!typeText.isEmpty()) {
        resultText += " (" + typeText + ")";
    }

    resultText += ": " + data + spaces + "\n";
    return resultText;
}

QString Utils::Misc::generateDebugInformation(bool withGitHubLineBreaks) {
    QSettings settings;
    QString output;

    output += "QOwnNotes Debug Information\n";
    output += "===========================\n";

    QDateTime dateTime = QDateTime::currentDateTime();

    // add information about QOwnNotes
    output += "\n## General Info\n\n";
    output += prepareDebugInformationLine("Current Date", dateTime.toString(),
            withGitHubLineBreaks);
    output += prepareDebugInformationLine("Version", QString(VERSION), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Build date", QString(__DATE__), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Build number",
                                          QString::number(BUILD), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Platform", QString(PLATFORM), withGitHubLineBreaks);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    output += prepareDebugInformationLine("Operating System",
                                          QSysInfo::prettyProductName(), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Build architecture",
                                          QSysInfo::buildCpuArchitecture(), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Current architecture",
                                          QSysInfo::currentCpuArchitecture(), withGitHubLineBreaks);
#endif
    output += prepareDebugInformationLine("Release",
                                          qApp->property("release").toString(), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Qt Version (build)", QT_VERSION_STR, withGitHubLineBreaks);
    output += prepareDebugInformationLine("Qt Version (runtime)", qVersion(), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Portable mode",
                                          Utils::Misc::isInPortableMode() ?
                                          "yes" : "no", withGitHubLineBreaks);
    output += prepareDebugInformationLine("Settings path / key",
                                          settings.fileName(), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Application database path",
                                          QDir::toNativeSeparators(DatabaseService::getDiskDatabasePath()), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Application arguments",
                                          qApp->property("arguments").toStringList().join("`, `"), withGitHubLineBreaks);

    QString debug = "0";
#ifdef QT_DEBUG
    debug = "1";
#endif

    output += prepareDebugInformationLine("Qt Debug", debug, withGitHubLineBreaks);

    output += prepareDebugInformationLine("Locale (system)",
                                          QLocale::system().name(), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Locale (interface)",
                                          settings.value("interfaceLanguage")
                                                  .toString(), withGitHubLineBreaks);

    output += prepareDebugInformationLine("Icon theme",
                                          QIcon::themeName(), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Notes in current note folder",
                                          QString::number(Note::countAll()), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Calendar items",
                                          QString::number(CalendarItem::countAll()), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Enabled scripts",
                                          QString::number(
                                                  Script::countEnabled()), withGitHubLineBreaks);

    // add information about the server
    output += "\n## Server Info\n\n";
    output += prepareDebugInformationLine(
            "serverUrl", CloudConnection::currentCloudConnection().getServerUrl(),
            withGitHubLineBreaks);
    const bool appIsValid = settings.value("ownCloudInfo/appIsValid").toBool();
    output += prepareDebugInformationLine(
            "appIsValid", QString(appIsValid ? "yes" : "no"), withGitHubLineBreaks);
    output += prepareDebugInformationLine(
            "notesPathExists",
            settings.value("ownCloudInfo/notesPathExistsText").toString(),
            withGitHubLineBreaks);
    if (appIsValid) {
        output += prepareDebugInformationLine(
                "serverVersion",
                settings.value("ownCloudInfo/serverVersion").toString(),
                withGitHubLineBreaks);
        output += prepareDebugInformationLine(
                "appVersion",
                settings.value("ownCloudInfo/appVersion").toString(),
                withGitHubLineBreaks);
    } else {
        output += prepareDebugInformationLine(
                "connectionErrorMessage",
                settings.value("ownCloudInfo/connectionErrorMessage").toString(),
                withGitHubLineBreaks);
    }

    // add spellchecker information
    output += "\n## Spellchecking\n\n";
    output += prepareDebugInformationLine("Enabled", settings.value("checkSpelling").toString(), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Selected language", settings.value("spellCheckLanguage").toString(), withGitHubLineBreaks);
    //auto *speller = new Sonnet::Speller();
    output += prepareDebugInformationLine("Language codes", Sonnet::Speller::availableLanguages().join(", "), withGitHubLineBreaks);
    output += prepareDebugInformationLine("Language names", Sonnet::Speller::availableLanguageNames().join(", "), withGitHubLineBreaks);
    //delete speller;
    output += prepareDebugInformationLine("Application dictionaries path",
                                          QDir::toNativeSeparators(Utils::Misc::localDictionariesPath()), withGitHubLineBreaks);

    // add note folder information
    output += "\n## Note folders\n\n";
    output += prepareDebugInformationLine(
            "currentNoteFolderId",
            QString::number(NoteFolder::currentNoteFolderId()), withGitHubLineBreaks);

    QList<NoteFolder> noteFolders = NoteFolder::fetchAll();
    if (noteFolders.count() > 0) {
        Q_FOREACH(NoteFolder noteFolder, noteFolders) {
                output += "\n### Note folder `" + noteFolder.getName() +
                          "`\n\n";
                output += prepareDebugInformationLine(
                        "id", QString::number(noteFolder.getId()), withGitHubLineBreaks);
                output += prepareDebugInformationLine(
                        "isCurrent",
                        noteFolder.isCurrent() ? "yes" : "no", withGitHubLineBreaks);
                output += prepareDebugInformationLine(
                        "activeTagId",
                        QString::number(noteFolder.getActiveTagId()), withGitHubLineBreaks);
                output += prepareDebugInformationLine(
                        "localPath", QDir::toNativeSeparators(
                                noteFolder.getLocalPath()), withGitHubLineBreaks);
                output += prepareDebugInformationLine(
                        "remotePath", noteFolder.getRemotePath(), withGitHubLineBreaks);
                output += prepareDebugInformationLine(
                        "cloudConnectionId",
                        QString::number(noteFolder.getCloudConnectionId()), withGitHubLineBreaks);
                output += prepareDebugInformationLine(
                        "isShowSubfolders",
                        noteFolder.isShowSubfolders() ? "yes" : "no", withGitHubLineBreaks);
                output += prepareDebugInformationLine(
                        "isUseGit",
                        noteFolder.isUseGit() ? "yes" : "no", withGitHubLineBreaks);
                output += prepareDebugInformationLine(
                        "activeNoteSubFolder name",
                        noteFolder.getActiveNoteSubFolder().getName(), withGitHubLineBreaks);
                output += prepareDebugInformationLine(
                        "database file",
                        QDir::toNativeSeparators(noteFolder.getLocalPath() +
                                                 "/notes.sqlite"), withGitHubLineBreaks);
            }
    }

    // add cloud connection information
    output += "\n## Cloud connections\n";

    Q_FOREACH(CloudConnection cloudConnection, CloudConnection::fetchAll()) {
        output += "\n### Cloud connection `" + cloudConnection.getName() +
                "`\n\n";
        output += prepareDebugInformationLine(
                    "id", QString::number(cloudConnection.getId()), withGitHubLineBreaks);
        output += prepareDebugInformationLine(
                    "isCurrent",
                    cloudConnection.isCurrent() ? "yes" : "no", withGitHubLineBreaks);
        output += prepareDebugInformationLine(
                    "serverUrl", cloudConnection.getServerUrl(), withGitHubLineBreaks);
        output += prepareDebugInformationLine(
                    "username", cloudConnection.getUsername(), withGitHubLineBreaks);
    }

    // add script information
    output += "\n## Enabled scripts\n";

    QList<Script> scripts = Script::fetchAll(true);
    if (noteFolders.count() > 0) {
        Q_FOREACH(Script script, scripts) {
                output += "\n### Script `" + script.getName() +
                          "`\n\n";
                output += prepareDebugInformationLine(
                        "id", QString::number(script.getId()), withGitHubLineBreaks);
                output += prepareDebugInformationLine(
                        "path", QDir::toNativeSeparators(
                                script.getScriptPath()), withGitHubLineBreaks);
                output += prepareDebugInformationLine(
                        "variablesJson", script.getSettingsVariablesJson(), withGitHubLineBreaks);
                if (script.isScriptFromRepository()) {
                    ScriptInfoJson infoJson = script.getScriptInfoJson();

                    output += prepareDebugInformationLine(
                            "identifier", script.getIdentifier(), withGitHubLineBreaks);
                    output += prepareDebugInformationLine(
                            "version", infoJson.version, withGitHubLineBreaks);
                    output += prepareDebugInformationLine(
                            "minAppVersion", infoJson.minAppVersion, withGitHubLineBreaks);
                }
            }
    } else {
        output += "\nThere are no enabled scripts.\n";
    }

    // add information about the settings
    output += "\n## Settings\n\n";

    // hide values of these keys
    QStringList keyHiddenList = (QStringList() <<
                                               "cryptoKey" <<
                                               "ownCloud/todoCalendarCalDAVPassword" <<
                                               "PiwikClientId" <<
                                               "networking/proxyPassword");

    // under OS X we have to ignore some keys
#ifdef Q_OS_MAC
    QStringList keyIgnoreList;
    keyIgnoreList << "AKDeviceUnlockState" << "Apple" << "NS" << "NavPanel"
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
            output += prepareDebugInformationLine(key, "<hidden>",
                    withGitHubLineBreaks, value.typeName());
        } else {
            switch (value.type()) {
                case QVariant::StringList:
                    output += prepareDebugInformationLine(
                            key, value.toStringList().join(", "),
                            withGitHubLineBreaks, value.typeName());
                    break;
                case QVariant::List:
                    output += prepareDebugInformationLine(key,
                            QString("<variant list with %1 item(s)>").arg(
                                    value.toList().count()),
                                    withGitHubLineBreaks, value.typeName());
                    break;
                case QVariant::ByteArray:
                case QVariant::UserType:
                    output += prepareDebugInformationLine(key, "<binary data>",
                            withGitHubLineBreaks, value.typeName());
                    break;
                default:
                    output += prepareDebugInformationLine(
                            key, value.toString(), withGitHubLineBreaks,
                            value.typeName());
            }
        }
    }

    // add information about the system environment
    output += "\n## System environment\n\n";

    itr = QProcess::systemEnvironment();
    while (itr.hasNext()) {
        QStringList textList = itr.next().split("=");
        QString key = textList.first();
        textList.removeFirst();
        QString value = textList.join("=");
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
bool Utils::Misc::regExpInListMatches(const QString& text, QStringList regExpList) {
    Q_FOREACH(QString regExp, regExpList) {
            regExp = regExp.trimmed();

            if (regExp.isEmpty()) {
                continue;
            }

            if (QRegularExpression(regExp).match(text).hasMatch()) {
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
void Utils::Misc::transformNextcloudPreviewImages(QString &html) {
    OwnCloudService *ownCloud = OwnCloudService::instance();

    QRegularExpression re(R"(<img src=\"(\/core\/preview\?fileId=.+#mimetype=[\w\d%]+&.+)\" alt=\".+\"\/?>)",
                          QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption);
    QRegularExpressionMatchIterator i = re.globalMatch(html);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString imageTag = match.captured(0);

        QString inlineImageTag = ownCloud->nextcloudPreviewImageTagToInlineImageTag(
                imageTag);

        html.replace(imageTag, inlineImageTag);
    }
}

/**
 * Transforms remote preview image tags
 *
 * @param html
 */
void Utils::Misc::transformRemotePreviewImages(QString &html) {
    QRegularExpression re(R"(<img src=\"(https?:\/\/.+)\".*\/?>)",
                          QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption | QRegularExpression::InvertedGreedinessOption);
    QRegularExpressionMatchIterator i = re.globalMatch(html);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString imageTag = match.captured(0);

        QString inlineImageTag = remotePreviewImageTagToInlineImageTag(imageTag);
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
QString Utils::Misc::remotePreviewImageTagToInlineImageTag(QString imageTag) {
    imageTag.replace("&amp;", "&");

    QRegularExpression re(R"(<img src=\"(https?:\/\/.+)\")",
                          QRegularExpression::CaseInsensitiveOption |
                          QRegularExpression::InvertedGreedinessOption);

    QRegularExpressionMatch match = re.match(imageTag);
    if (!match.hasMatch()) {
        return imageTag;
    }

    QString url = match.captured(1);
    QByteArray data = downloadUrl(url);
    QMimeDatabase db;
    auto type = db.mimeTypeForData(data);

    // for now we do no caching, because we don't know when to invalidate the cache
    const QString replace = "data:" + type.name() + ";base64," + data.toBase64();
    const QString inlineImageTag = imageTag.replace(url, replace);

    return inlineImageTag;
}

QString Utils::Misc::createUuidString() {
    QUuid uuid = QUuid::createUuid();
    QString uuidString = uuid.toString();
    uuidString.replace("{", "").replace("}", "");

    return uuidString;
}

/**
 * Returns the path where the local dictionaries will be stored
 *
 * @return
 */
QString Utils::Misc::localDictionariesPath() {
    QString path = Utils::Misc::appDataPath() + QStringLiteral("/dicts");
    QDir dir;

    // create path if it doesn't exist yet
    dir.mkpath(path);
    return path;
}
