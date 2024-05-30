/*
 * Copyright (c) 2014-2024 Patrizio Bekerle -- <patrizio@bekerle.com>
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
#include <QSettings>

#include "mainwindow.h"

NoteFilePathLabel::NoteFilePathLabel(QWidget *parent) : QLabel(parent) {
    setText(QLatin1String());
    setToolTip(tr("Absolute path of note, right-click to open context menu"));
    setContentsMargins(5, 0, 0, 0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    // Allows that the label can be resized below it desired width
    setMinimumWidth(10);
    setMaximumWidth(600);
    setTextFormat(Qt::RichText);
}

void NoteFilePathLabel::updateText() {
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == nullptr) {
        return;
    }

    const auto note = mainWindow->getCurrentNote();
    const auto separator = Utils::Misc::dirSeparator();
//    QString notePath = QSettings().value(QStringLiteral("showStatusBarRelativeNotePath")).toBool() ?
//        note.relativeNoteFilePath() : note.fullNoteFilePath();

    QString notePath = Utils::Misc::htmlspecialchars(note.getFileName());

    const NoteSubFolder noteSubFolder = note.getNoteSubFolder();
    if (noteSubFolder.isFetched()) {
        // TODO: Adapt color to light/dark theme
        notePath.prepend("<span style='color: #666666;'>" + Utils::Misc::htmlspecialchars(noteSubFolder.relativePath()) + "</span>" + separator);
    }

    if (!QSettings().value(QStringLiteral("showStatusBarRelativeNotePath")).toBool()) {
        auto noteFolderPath = NoteFolder::currentLocalPath();

#ifndef Q_OS_WIN
        QString homeDir = QDir::homePath();
        if (noteFolderPath.startsWith(homeDir)) {
            noteFolderPath.replace(0, homeDir.length(), "~");
        }
#endif

        // TODO: Adapt color to light/dark theme
        notePath.prepend("<span style='color: #aaaaaa;'>" + Utils::Misc::htmlspecialchars(noteFolderPath) + "</span>" + separator);
    }


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
        qDebug() << __func__ << " - 'subFolderPath': " << subFolderPath;

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

void NoteFilePathLabel::adjustTextToFit() {
    // Get the available width for the label
    int availableWidth = width();

    // Use QFontMetrics to elide the text if it's too long
    QFontMetrics metrics(font());
    QString elidedText = metrics.elidedText(originalText, Qt::ElideLeft, availableWidth);

    // Set the elided text
    QLabel::setText(elidedText);
}

