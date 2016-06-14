/*
 * Copyright (C) 2016 Patrizio Bekerle -- http://www.bekerle.com
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
#include <QDebug>
#include "misc.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif


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
QString Utils::Misc::removeIfStartsWith(QString text, QString removeString) {
    if (text.startsWith(removeString)) {
        text.remove(QRegularExpression(
                "^" + QRegularExpression::escape(removeString)));
    }

    return text;
}

/**
 * Removes a string from the end if it ends with it
 */
QString Utils::Misc::removeIfEndsWith(QString text, QString removeString) {
    if (text.endsWith(removeString)) {
        text.remove(QRegularExpression(
                QRegularExpression::escape(removeString) + "$"));
    }

    return text;
}

/**
 * Adds a string to the beginning of a string if it doesn't start with it
 */
QString Utils::Misc::prependIfDoesNotStartWith(
        QString text, QString startString) {
    if (!text.startsWith(startString)) {
        text.prepend(startString);
    }

    return text;
}

/**
 * Adds a string to the end of a string if it doesn't end with it
 */
QString Utils::Misc::appendIfDoesNotEndWith(
        QString text, QString endString) {
    if (!text.endsWith(endString)) {
        text.append(endString);
    }

    return text;
}

/**
 * Shortens text and adds a sequence string if text is too long
 */
QString Utils::Misc::shorten(
        QString text, int length, QString sequence) {
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
 * Starts an executable detached with parameters
 */
bool Utils::Misc::startDetachedProcess(
        QString executablePath, QStringList parameters) {
    QProcess process;

    // start executablePath detached with parameters
#ifdef Q_OS_MAC
    return process.startDetached(
            "open", QStringList() << executablePath << "--args" << parameters);
#else
    return process.startDetached(executablePath, parameters);
#endif
}
