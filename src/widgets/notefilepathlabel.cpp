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
 */

#include "notefilepathlabel.h"

#include <entities/notefolder.h>
#include <entities/notesubfolder.h>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QMenu>

#include "mainwindow.h"
#include "services/settingsservice.h"

NoteFilePathLabel::NoteFilePathLabel(QWidget *parent) : QLabel(parent) {
    // Empty the text
    setText(QLatin1String());
    // Add a little space on the left
    setContentsMargins(5, 0, 0, 0);
    // Allows that the label can be resized below it desired width
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setMinimumWidth(10);
    setMaximumWidth(600);
    // We want to use HTML tags
    setTextFormat(Qt::RichText);
}

void NoteFilePathLabel::updateText() {
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == nullptr) {
        return;
    }

    const auto note = mainWindow->getCurrentNote();
    const auto separator = Utils::Misc::dirSeparator();
    QString notePath = Utils::Misc::htmlspecialchars(note.getFileName());
    const bool darkModeColors = SettingsService().value(QStringLiteral("darkModeColors")).toBool();
    const auto subFolderColor =
        darkModeColors ? QStringLiteral("#999999") : QStringLiteral("#696969");
    const auto noteFolderColor =
        darkModeColors ? QStringLiteral("#707070") : QStringLiteral("#a0a0a0");

    const NoteSubFolder noteSubFolder = note.getNoteSubFolder();
    if (noteSubFolder.isFetched()) {
        notePath.prepend(QStringLiteral("<span style='color: %1;'>%2</span>%3")
                             .arg(subFolderColor,
                                  Utils::Misc::htmlspecialchars(noteSubFolder.relativePath()),
                                  QString(separator)));
    }

    bool showRelativeNotePath =
        SettingsService().value(QStringLiteral("showStatusBarRelativeNotePath")).toBool();
    if (!showRelativeNotePath) {
        auto noteFolderPath = NoteFolder::currentLocalPath();

#ifndef Q_OS_WIN
        QString homeDir = QDir::homePath();
        if (noteFolderPath.startsWith(homeDir)) {
            noteFolderPath.replace(0, homeDir.length(), "~");
        }
#endif

        notePath.prepend(QStringLiteral("<span style='color: %1;'>%2</span>%3")
                             .arg(noteFolderColor, Utils::Misc::htmlspecialchars(noteFolderPath),
                                  QString(separator)));
    }

    setToolTip(showRelativeNotePath
                   ? tr("Relative path of note, right-click to open context menu")
                   : tr("Absolute path of note, right-click to open context menu"));
    setText(notePath);
}

void NoteFilePathLabel::contextMenuEvent(QContextMenuEvent *event) {
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == nullptr) {
        return;
    }

    QMenu contextMenu(this);
    contextMenu.setToolTipsVisible(true);
    QClipboard *clipboard = QApplication::clipboard();
    auto noteFolder = NoteFolder::currentNoteFolder();

    QAction *copyFullPathAction = contextMenu.addAction(tr("Copy absolute path of note"));
    const QString notePath = mainWindow->getCurrentNote().fullNoteFilePath();
    copyFullPathAction->setToolTip(notePath);

    QAction *copySubFolderPathAction = nullptr;
    QString subFolderPath;
    if (NoteFolder::isCurrentHasSubfolders()) {
        copySubFolderPathAction = contextMenu.addAction(tr("Copy absolute path of note subfolder"));
        subFolderPath = NoteSubFolder::activeNoteSubFolder().fullPath();
        copySubFolderPathAction->setToolTip(subFolderPath);
    }

    QAction *copyNoteFolderPathAction =
        contextMenu.addAction(tr("Copy absolute path of note folder"));
    const QString noteFolderPath = noteFolder.getLocalPath();
    copyNoteFolderPathAction->setToolTip(noteFolderPath);

    QAction *selectedAction = contextMenu.exec(event->globalPos());
    if (selectedAction == copyFullPathAction) {
        clipboard->setText(notePath);
    } else if (selectedAction == copySubFolderPathAction) {
        clipboard->setText(subFolderPath);
    } else if (selectedAction == copyNoteFolderPathAction) {
        clipboard->setText(noteFolderPath);
    }
}

void NoteFilePathLabel::resizeEvent(QResizeEvent *event) {
    QLabel::resizeEvent(event);
    //    adjustTextToFit();
}

void NoteFilePathLabel::setText(const QString &text) {
    originalText = text;
    QLabel::setText(text);
    //    adjustTextToFit();
}

/**
 * Unfortunately that doesn't work any more, because of the usage of HTML
 */
void NoteFilePathLabel::adjustTextToFit() {
    // Get the available width for the label
    int availableWidth = width();

    // Use QFontMetrics to elide the text if it's too long
    QFontMetrics metrics(font());
    QString elidedText = metrics.elidedText(originalText, Qt::ElideLeft, availableWidth);

    // Set the elided text
    QLabel::setText(elidedText);
}
