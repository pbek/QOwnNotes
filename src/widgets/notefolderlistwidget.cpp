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

#include "notefolderlistwidget.h"

#include <entities/notefolder.h>

NoteFolderListWidget::NoteFolderListWidget(QWidget *parent) { Q_UNUSED(parent); }

void NoteFolderListWidget::dropEvent(QDropEvent *e) {
    // finish the move event
    QListWidget::dropEvent(e);

    int itemCount = count();

    if (itemCount == 0) {
        return;
    }

    // update all priorities of the scripts
    for (int index = 0; index < itemCount; index++) {
        QListWidgetItem *listItem = item(index);
        int noteFolderId = listItem->data(Qt::UserRole).toInt();

        NoteFolder noteFolder = NoteFolder::fetch(noteFolderId);

        // store the new priority of the note folder
        if (noteFolder.isFetched()) {
            noteFolder.setPriority(index);
            noteFolder.store();
        }
    }
}
