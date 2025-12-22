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

#include "scriptlistwidget.h"

#include <entities/script.h>
#include <services/scriptingservice.h>

ScriptListWidget::ScriptListWidget(QWidget *parent) { Q_UNUSED(parent); }

void ScriptListWidget::dropEvent(QDropEvent *e) {
    // finish the move event
    QListWidget::dropEvent(e);

    int itemCount = count();

    if (itemCount == 0) {
        return;
    }

    // update all priorities of the scripts
    for (int index = 0; index < itemCount; index++) {
        QListWidgetItem *listItem = item(index);
        int scriptId = listItem->data(Qt::UserRole).toInt();

        Script script = Script::fetch(scriptId);

        // store the new priority of the script
        if (script.isFetched()) {
            script.setPriority(index);
            script.store();
        }
    }

    // reload the scripting engine
    ScriptingService::instance()->reloadEngine();
}
