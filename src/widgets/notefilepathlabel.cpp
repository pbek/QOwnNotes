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

#include "mainwindow.h"

NoteFilePathLabel::NoteFilePathLabel(QWidget *parent) : QLabel(parent) {
    setText(QStringLiteral(""));
    setToolTip(tr("Absolute path of note, right-click to open context menu"));
    setContentsMargins(5, 0, 0, 0);
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
    if (noteFolder.isCurrentHasSubfolders()) {
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
